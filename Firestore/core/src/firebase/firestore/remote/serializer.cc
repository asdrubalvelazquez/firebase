/*
 * Copyright 2018 Google
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Firestore/core/src/firebase/firestore/remote/serializer.h"

#include <pb_decode.h>
#include <pb_encode.h>

#include <functional>
#include <map>
#include <string>
#include <utility>

#include "Firestore/Protos/nanopb/google/firestore/v1beta1/document.pb.h"
#include "Firestore/Protos/nanopb/google/firestore/v1beta1/firestore.pb.h"
#include "Firestore/core/include/firebase/firestore/firestore_errors.h"
#include "Firestore/core/include/firebase/firestore/timestamp.h"
#include "Firestore/core/src/firebase/firestore/model/document.h"
#include "Firestore/core/src/firebase/firestore/model/no_document.h"
#include "Firestore/core/src/firebase/firestore/model/resource_path.h"
#include "Firestore/core/src/firebase/firestore/model/snapshot_version.h"
#include "Firestore/core/src/firebase/firestore/nanopb/reader.h"
#include "Firestore/core/src/firebase/firestore/nanopb/tag.h"
#include "Firestore/core/src/firebase/firestore/nanopb/writer.h"
#include "Firestore/core/src/firebase/firestore/timestamp_internal.h"
#include "Firestore/core/src/firebase/firestore/util/firebase_assert.h"
#include "absl/memory/memory.h"

namespace firebase {
namespace firestore {
namespace remote {

using firebase::Timestamp;
using firebase::TimestampInternal;
using firebase::firestore::model::DatabaseId;
using firebase::firestore::model::Document;
using firebase::firestore::model::DocumentKey;
using firebase::firestore::model::FieldValue;
using firebase::firestore::model::MaybeDocument;
using firebase::firestore::model::NoDocument;
using firebase::firestore::model::ObjectValue;
using firebase::firestore::model::ResourcePath;
using firebase::firestore::model::SnapshotVersion;
using firebase::firestore::nanopb::Reader;
using firebase::firestore::nanopb::Tag;
using firebase::firestore::nanopb::Writer;
using firebase::firestore::util::Status;
using firebase::firestore::util::StatusOr;

namespace {

void EncodeObject(Writer* writer, const ObjectValue& object_value);

ObjectValue::Map DecodeObject(Reader* reader);

void EncodeTimestamp(Writer* writer, const Timestamp& timestamp_value) {
  google_protobuf_Timestamp timestamp_proto =
      google_protobuf_Timestamp_init_zero;
  timestamp_proto.seconds = timestamp_value.seconds();
  timestamp_proto.nanos = timestamp_value.nanoseconds();
  writer->WriteNanopbMessage(google_protobuf_Timestamp_fields,
                             &timestamp_proto);
}

Timestamp DecodeTimestamp(Reader* reader) {
  google_protobuf_Timestamp timestamp_proto =
      google_protobuf_Timestamp_init_zero;
  reader->ReadNanopbMessage(google_protobuf_Timestamp_fields, &timestamp_proto);

  // The Timestamp ctor will assert if we provide values outside the valid
  // range. However, since we're decoding, a single corrupt byte could cause
  // this to occur, so we'll verify the ranges before passing them in since we'd
  // rather not abort in these situations.
  if (timestamp_proto.seconds < TimestampInternal::Min().seconds()) {
    reader->set_status(Status(
        FirestoreErrorCode::DataLoss,
        "Invalid message: timestamp beyond the earliest supported date"));
    return {};
  } else if (TimestampInternal::Max().seconds() < timestamp_proto.seconds) {
    reader->set_status(
        Status(FirestoreErrorCode::DataLoss,
               "Invalid message: timestamp behond the latest supported date"));
    return {};
  } else if (timestamp_proto.nanos < 0 || timestamp_proto.nanos > 999999999) {
    reader->set_status(Status(
        FirestoreErrorCode::DataLoss,
        "Invalid message: timestamp nanos must be between 0 and 999999999"));
    return {};
  }
  return Timestamp{timestamp_proto.seconds, timestamp_proto.nanos};
}

// Named '..Impl' so as to not conflict with Serializer::EncodeFieldValue.
// TODO(rsgowman): Refactor to use a helper class that wraps the stream struct.
// This will help with error handling, and should eliminate the issue of two
// 'EncodeFieldValue' methods.
void EncodeFieldValueImpl(Writer* writer, const FieldValue& field_value) {
  // TODO(rsgowman): some refactoring is in order... but will wait until after a
  // non-varint, non-fixed-size (i.e. string) type is present before doing so.
  switch (field_value.type()) {
    case FieldValue::Type::Null:
      writer->WriteTag(
          {PB_WT_VARINT, google_firestore_v1beta1_Value_null_value_tag});
      writer->WriteNull();
      break;

    case FieldValue::Type::Boolean:
      writer->WriteTag(
          {PB_WT_VARINT, google_firestore_v1beta1_Value_boolean_value_tag});
      writer->WriteBool(field_value.boolean_value());
      break;

    case FieldValue::Type::Integer:
      writer->WriteTag(
          {PB_WT_VARINT, google_firestore_v1beta1_Value_integer_value_tag});
      writer->WriteInteger(field_value.integer_value());
      break;

    case FieldValue::Type::String:
      writer->WriteTag(
          {PB_WT_STRING, google_firestore_v1beta1_Value_string_value_tag});
      writer->WriteString(field_value.string_value());
      break;

    case FieldValue::Type::Timestamp:
      writer->WriteTag(
          {PB_WT_STRING, google_firestore_v1beta1_Value_timestamp_value_tag});
      writer->WriteNestedMessage([&field_value](Writer* writer) {
        EncodeTimestamp(writer, field_value.timestamp_value());
      });
      break;

    case FieldValue::Type::Object:
      writer->WriteTag(
          {PB_WT_STRING, google_firestore_v1beta1_Value_map_value_tag});
      EncodeObject(writer, field_value.object_value());
      break;

    default:
      // TODO(rsgowman): implement the other types
      abort();
  }
}

FieldValue DecodeFieldValueImpl(Reader* reader) {
  Tag tag = reader->ReadTag();
  if (!reader->status().ok()) return FieldValue::NullValue();

  // Ensure the tag matches the wire type
  switch (tag.field_number) {
    case google_firestore_v1beta1_Value_null_value_tag:
    case google_firestore_v1beta1_Value_boolean_value_tag:
    case google_firestore_v1beta1_Value_integer_value_tag:
      if (tag.wire_type != PB_WT_VARINT) {
        reader->set_status(
            Status(FirestoreErrorCode::DataLoss,
                   "Input proto bytes cannot be parsed (mismatch between "
                   "the wiretype and the field number (tag))"));
      }
      break;

    case google_firestore_v1beta1_Value_string_value_tag:
    case google_firestore_v1beta1_Value_timestamp_value_tag:
    case google_firestore_v1beta1_Value_map_value_tag:
      if (tag.wire_type != PB_WT_STRING) {
        reader->set_status(
            Status(FirestoreErrorCode::DataLoss,
                   "Input proto bytes cannot be parsed (mismatch between "
                   "the wiretype and the field number (tag))"));
      }
      break;

    default:
      // We could get here for one of two reasons; either because the input
      // bytes are corrupt, or because we're attempting to parse a tag that we
      // haven't implemented yet. Long term, the latter reason should become
      // less likely (especially in production), so we'll assume former.

      // TODO(rsgowman): While still in development, we'll contradict the above
      // and assume the latter. Remove the following assertion when we're
      // confident that we're handling all the tags in the protos.
      FIREBASE_ASSERT_MESSAGE(
          false,
          "Unhandled message field number (tag): %i. (Or possibly "
          "corrupt input bytes)",
          tag.field_number);
      reader->set_status(Status(
          FirestoreErrorCode::DataLoss,
          "Input proto bytes cannot be parsed (invalid field number (tag))"));
  }

  if (!reader->status().ok()) return FieldValue::NullValue();

  switch (tag.field_number) {
    case google_firestore_v1beta1_Value_null_value_tag:
      reader->ReadNull();
      return FieldValue::NullValue();
    case google_firestore_v1beta1_Value_boolean_value_tag:
      return FieldValue::BooleanValue(reader->ReadBool());
    case google_firestore_v1beta1_Value_integer_value_tag:
      return FieldValue::IntegerValue(reader->ReadInteger());
    case google_firestore_v1beta1_Value_string_value_tag:
      return FieldValue::StringValue(reader->ReadString());
    case google_firestore_v1beta1_Value_timestamp_value_tag:
      return FieldValue::TimestampValue(
          reader->ReadNestedMessage<Timestamp>(DecodeTimestamp));
    case google_firestore_v1beta1_Value_map_value_tag:
      return FieldValue::ObjectValueFromMap(DecodeObject(reader));

    default:
      // This indicates an internal error as we've already ensured that this is
      // a valid field_number.
      FIREBASE_ASSERT_MESSAGE(
          false,
          "Somehow got an unexpected field number (tag) after verifying that "
          "the field number was expected.");
  }
}

/**
 * Encodes a 'FieldsEntry' object, within a FieldValue's map_value type.
 *
 * In protobuf, maps are implemented as a repeated set of key/values. For
 * instance, this:
 *   message Foo {
 *     map<string, Value> fields = 1;
 *   }
 * would be written (in proto text format) as:
 *   {
 *     fields: {key:"key string 1", value:{<Value message here>}}
 *     fields: {key:"key string 2", value:{<Value message here>}}
 *     ...
 *   }
 *
 * This method writes an individual entry from that list. It is expected that
 * this method will be called once for each entry in the map.
 *
 * @param kv The individual key/value pair to write.
 */
void EncodeFieldsEntry(Writer* writer, const ObjectValue::Map::value_type& kv) {
  // Write the key (string)
  writer->WriteTag(
      {PB_WT_STRING, google_firestore_v1beta1_MapValue_FieldsEntry_key_tag});
  writer->WriteString(kv.first);

  // Write the value (FieldValue)
  writer->WriteTag(
      {PB_WT_STRING, google_firestore_v1beta1_MapValue_FieldsEntry_value_tag});
  writer->WriteNestedMessage(
      [&kv](Writer* writer) { EncodeFieldValueImpl(writer, kv.second); });
}

ObjectValue::Map::value_type DecodeFieldsEntry(Reader* reader) {
  Tag tag = reader->ReadTag();
  if (!reader->status().ok()) return {};

  // TODO(rsgowman): figure out error handling: We can do better than a failed
  // assertion.
  FIREBASE_ASSERT(tag.field_number ==
                  google_firestore_v1beta1_MapValue_FieldsEntry_key_tag);
  FIREBASE_ASSERT(tag.wire_type == PB_WT_STRING);
  std::string key = reader->ReadString();

  tag = reader->ReadTag();
  if (!reader->status().ok()) return {};
  FIREBASE_ASSERT(tag.field_number ==
                  google_firestore_v1beta1_MapValue_FieldsEntry_value_tag);
  FIREBASE_ASSERT(tag.wire_type == PB_WT_STRING);

  FieldValue value =
      reader->ReadNestedMessage<FieldValue>(DecodeFieldValueImpl);

  return ObjectValue::Map::value_type{key, value};
}

void EncodeObject(Writer* writer, const ObjectValue& object_value) {
  return writer->WriteNestedMessage([&object_value](Writer* writer) {
    // Write each FieldsEntry (i.e. key-value pair.)
    for (const auto& kv : object_value.internal_value) {
      writer->WriteTag({PB_WT_STRING,
                        google_firestore_v1beta1_MapValue_FieldsEntry_key_tag});
      writer->WriteNestedMessage(
          [&kv](Writer* writer) { return EncodeFieldsEntry(writer, kv); });
    }
  });
}

ObjectValue::Map DecodeObject(Reader* reader) {
  if (!reader->status().ok()) return ObjectValue::Map();

  return reader->ReadNestedMessage<ObjectValue::Map>(
      [](Reader* reader) -> ObjectValue::Map {
        ObjectValue::Map result;
        if (!reader->status().ok()) return result;

        while (reader->bytes_left()) {
          Tag tag = reader->ReadTag();
          if (!reader->status().ok()) return result;
          FIREBASE_ASSERT(tag.field_number ==
                          google_firestore_v1beta1_MapValue_fields_tag);
          FIREBASE_ASSERT(tag.wire_type == PB_WT_STRING);

          ObjectValue::Map::value_type fv =
              reader->ReadNestedMessage<ObjectValue::Map::value_type>(
                  DecodeFieldsEntry);

          // Sanity check: ensure that this key doesn't already exist in the
          // map.
          // TODO(rsgowman): figure out error handling: We can do better than a
          // failed assertion.
          if (!reader->status().ok()) return result;
          FIREBASE_ASSERT(result.find(fv.first) == result.end());

          // Add this key,fieldvalue to the results map.
          result.emplace(std::move(fv));
        }
        return result;
      });
}

/**
 * Creates the prefix for a fully qualified resource path, without a local path
 * on the end.
 */
ResourcePath EncodeDatabaseId(const DatabaseId& database_id) {
  return ResourcePath{"projects", database_id.project_id(), "databases",
                      database_id.database_id()};
}

/**
 * Encodes a databaseId and resource path into the following form:
 * /projects/$projectId/database/$databaseId/documents/$path
 */
std::string EncodeResourceName(const DatabaseId& database_id,
                               const ResourcePath& path) {
  return EncodeDatabaseId(database_id)
      .Append("documents")
      .Append(path)
      .CanonicalString();
}

/**
 * Validates that a path has a prefix that looks like a valid encoded
 * databaseId.
 */
bool IsValidResourceName(const ResourcePath& path) {
  // Resource names have at least 4 components (project ID, database ID)
  // and commonly the (root) resource type, e.g. documents
  return path.size() >= 4 && path[0] == "projects" && path[2] == "databases";
}

/**
 * Decodes a fully qualified resource name into a resource path and validates
 * that there is a project and database encoded in the path. There are no
 * guarantees that a local path is also encoded in this resource name.
 */
ResourcePath DecodeResourceName(absl::string_view encoded) {
  ResourcePath resource = ResourcePath::FromString(encoded);
  FIREBASE_ASSERT_MESSAGE(IsValidResourceName(resource),
                          "Tried to deserialize invalid key %s",
                          resource.CanonicalString().c_str());
  return resource;
}

/**
 * Decodes a fully qualified resource name into a resource path and validates
 * that there is a project and database encoded in the path along with a local
 * path.
 */
ResourcePath ExtractLocalPathFromResourceName(
    const ResourcePath& resource_name) {
  FIREBASE_ASSERT_MESSAGE(
      resource_name.size() > 4 && resource_name[4] == "documents",
      "Tried to deserialize invalid key %s",
      resource_name.CanonicalString().c_str());
  return resource_name.PopFirst(5);
}

}  // namespace

Status Serializer::EncodeFieldValue(const FieldValue& field_value,
                                    std::vector<uint8_t>* out_bytes) {
  Writer writer = Writer::Wrap(out_bytes);
  EncodeFieldValueImpl(&writer, field_value);
  return writer.status();
}

StatusOr<FieldValue> Serializer::DecodeFieldValue(const uint8_t* bytes,
                                                  size_t length) {
  Reader reader = Reader::Wrap(bytes, length);
  FieldValue fv = DecodeFieldValueImpl(&reader);
  if (reader.status().ok()) {
    return fv;
  } else {
    return reader.status();
  }
}

std::string Serializer::EncodeKey(const DocumentKey& key) const {
  return EncodeResourceName(database_id_, key.path());
}

DocumentKey Serializer::DecodeKey(absl::string_view name) const {
  ResourcePath resource = DecodeResourceName(name);
  FIREBASE_ASSERT_MESSAGE(resource[1] == database_id_.project_id(),
                          "Tried to deserialize key from different project.");
  FIREBASE_ASSERT_MESSAGE(resource[3] == database_id_.database_id(),
                          "Tried to deserialize key from different database.");
  return DocumentKey{ExtractLocalPathFromResourceName(resource)};
}

util::Status Serializer::EncodeDocument(const DocumentKey& key,
                                        const ObjectValue& value,
                                        std::vector<uint8_t>* out_bytes) const {
  Writer writer = Writer::Wrap(out_bytes);
  EncodeDocument(&writer, key, value);
  return writer.status();
}

void Serializer::EncodeDocument(Writer* writer,
                                const DocumentKey& key,
                                const ObjectValue& object_value) const {
  // Encode Document.name
  writer->WriteTag({PB_WT_STRING, google_firestore_v1beta1_Document_name_tag});
  writer->WriteString(EncodeKey(key));

  // Encode Document.fields (unless it's empty)
  if (!object_value.internal_value.empty()) {
    writer->WriteTag(
        {PB_WT_STRING, google_firestore_v1beta1_Document_fields_tag});
    EncodeObject(writer, object_value);
  }

  // Skip Document.create_time and Document.update_time, since they're
  // output-only fields.
}

util::StatusOr<std::unique_ptr<model::MaybeDocument>>
Serializer::DecodeMaybeDocument(const uint8_t* bytes, size_t length) const {
  Reader reader = Reader::Wrap(bytes, length);
  std::unique_ptr<MaybeDocument> maybeDoc =
      DecodeBatchGetDocumentsResponse(&reader);

  if (reader.status().ok()) {
    return maybeDoc;
  } else {
    return reader.status();
  }
}

std::unique_ptr<MaybeDocument> Serializer::DecodeBatchGetDocumentsResponse(
    Reader* reader) const {
  Tag tag = reader->ReadTag();
  if (!reader->status().ok()) return nullptr;

  // Ensure the tag matches the wire type
  switch (tag.field_number) {
    case google_firestore_v1beta1_BatchGetDocumentsResponse_found_tag:
    case google_firestore_v1beta1_BatchGetDocumentsResponse_missing_tag:
      if (tag.wire_type != PB_WT_STRING) {
        reader->set_status(
            Status(FirestoreErrorCode::DataLoss,
                   "Input proto bytes cannot be parsed (mismatch between "
                   "the wiretype and the field number (tag))"));
      }
      break;

    default:
      reader->set_status(Status(
          FirestoreErrorCode::DataLoss,
          "Input proto bytes cannot be parsed (invalid field number (tag))"));
  }

  if (!reader->status().ok()) return nullptr;

  switch (tag.field_number) {
    case google_firestore_v1beta1_BatchGetDocumentsResponse_found_tag:
      return reader->ReadNestedMessage<std::unique_ptr<MaybeDocument>>(
          nullptr, [this](Reader* reader) -> std::unique_ptr<MaybeDocument> {
            return DecodeDocument(reader);
          });
    case google_firestore_v1beta1_BatchGetDocumentsResponse_missing_tag:
      // TODO(rsgowman): Right now, we only support Document (and don't support
      // NoDocument). That should change in the next PR or so.
      abort();
    default:
      // This indicates an internal error as we've already ensured that this is
      // a valid field_number.
      FIREBASE_ASSERT_MESSAGE(
          false,
          "Somehow got an unexpected field number (tag) after verifying that "
          "the field number was expected.");
  }
}

std::unique_ptr<Document> Serializer::DecodeDocument(Reader* reader) const {
  std::string name;
  FieldValue fields = FieldValue::ObjectValueFromMap({});
  SnapshotVersion version = SnapshotVersion::None();

  while (reader->bytes_left()) {
    Tag tag = reader->ReadTag();
    if (!reader->status().ok()) return nullptr;
    FIREBASE_ASSERT(tag.wire_type == PB_WT_STRING);
    switch (tag.field_number) {
      case google_firestore_v1beta1_Document_name_tag:
        name = reader->ReadString();
        break;
      case google_firestore_v1beta1_Document_fields_tag:
        // TODO(rsgowman): Rather than overwriting, we should instead merge with
        // the existing FieldValue (if any).
        fields = DecodeFieldValueImpl(reader);
        break;
      case google_firestore_v1beta1_Document_create_time_tag:
        // This field is ignored by the client sdk, but we still need to extract
        // it.
        reader->ReadNestedMessage<Timestamp>(DecodeTimestamp);
        break;
      case google_firestore_v1beta1_Document_update_time_tag:
        // TODO(rsgowman): Rather than overwriting, we should instead merge with
        // the existing SnapshotVersion (if any). Less relevant here, since it's
        // just two numbers which are both expected to be present, but if the
        // proto evolves that might change.
        version = SnapshotVersion{
            reader->ReadNestedMessage<Timestamp>(DecodeTimestamp)};
        break;
      default:
        // TODO(rsgowman): Error handling. (Invalid tags should fail to decode,
        // but shouldn't cause a crash.)
        abort();
    }
  }

  return absl::make_unique<Document>(std::move(fields), DecodeKey(name),
                                     version,
                                     /*has_local_modifications=*/false);
}

}  // namespace remote
}  // namespace firestore
}  // namespace firebase
