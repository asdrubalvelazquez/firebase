/*
 * Copyright 2024 Google LLC
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

// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: google/type/latlng.proto

#include "google/type/latlng.pb.h"

#include <algorithm>
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/extension_set.h"
#include "google/protobuf/wire_format_lite.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/generated_message_reflection.h"
#include "google/protobuf/reflection_ops.h"
#include "google/protobuf/wire_format.h"
#include "google/protobuf/generated_message_tctable_impl.h"
// @@protoc_insertion_point(includes)

// Must be included last.
#include "google/protobuf/port_def.inc"
PROTOBUF_PRAGMA_INIT_SEG
namespace _pb = ::google::protobuf;
namespace _pbi = ::google::protobuf::internal;
namespace _fl = ::google::protobuf::internal::field_layout;
namespace google {
namespace type {

inline constexpr LatLng::Impl_::Impl_(
    ::_pbi::ConstantInitialized) noexcept
      : latitude_{0},
        longitude_{0},
        _cached_size_{0} {}

template <typename>
PROTOBUF_CONSTEXPR LatLng::LatLng(::_pbi::ConstantInitialized)
    : _impl_(::_pbi::ConstantInitialized()) {}
struct LatLngDefaultTypeInternal {
  PROTOBUF_CONSTEXPR LatLngDefaultTypeInternal() : _instance(::_pbi::ConstantInitialized{}) {}
  ~LatLngDefaultTypeInternal() {}
  union {
    LatLng _instance;
  };
};

PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT
    PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 LatLngDefaultTypeInternal _LatLng_default_instance_;
}  // namespace type
}  // namespace google
static ::_pb::Metadata file_level_metadata_google_2ftype_2flatlng_2eproto[1];
static constexpr const ::_pb::EnumDescriptor**
    file_level_enum_descriptors_google_2ftype_2flatlng_2eproto = nullptr;
static constexpr const ::_pb::ServiceDescriptor**
    file_level_service_descriptors_google_2ftype_2flatlng_2eproto = nullptr;
const ::uint32_t TableStruct_google_2ftype_2flatlng_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(
    protodesc_cold) = {
    ~0u,  // no _has_bits_
    PROTOBUF_FIELD_OFFSET(::google::type::LatLng, _internal_metadata_),
    ~0u,  // no _extensions_
    ~0u,  // no _oneof_case_
    ~0u,  // no _weak_field_map_
    ~0u,  // no _inlined_string_donated_
    ~0u,  // no _split_
    ~0u,  // no sizeof(Split)
    PROTOBUF_FIELD_OFFSET(::google::type::LatLng, _impl_.latitude_),
    PROTOBUF_FIELD_OFFSET(::google::type::LatLng, _impl_.longitude_),
};

static const ::_pbi::MigrationSchema
    schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
        {0, -1, -1, sizeof(::google::type::LatLng)},
};

static const ::_pb::Message* const file_default_instances[] = {
    &::google::type::_LatLng_default_instance_._instance,
};
const char descriptor_table_protodef_google_2ftype_2flatlng_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
    "\n\030google/type/latlng.proto\022\013google.type\""
    "-\n\006LatLng\022\020\n\010latitude\030\001 \001(\001\022\021\n\tlongitude"
    "\030\002 \001(\001B`\n\017com.google.typeB\013LatLngProtoP\001"
    "Z8google.golang.org/genproto/googleapis/"
    "type/latlng;latlng\242\002\003GTPb\006proto3"
};
static ::absl::once_flag descriptor_table_google_2ftype_2flatlng_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_google_2ftype_2flatlng_2eproto = {
    false,
    false,
    192,
    descriptor_table_protodef_google_2ftype_2flatlng_2eproto,
    "google/type/latlng.proto",
    &descriptor_table_google_2ftype_2flatlng_2eproto_once,
    nullptr,
    0,
    1,
    schemas,
    file_default_instances,
    TableStruct_google_2ftype_2flatlng_2eproto::offsets,
    file_level_metadata_google_2ftype_2flatlng_2eproto,
    file_level_enum_descriptors_google_2ftype_2flatlng_2eproto,
    file_level_service_descriptors_google_2ftype_2flatlng_2eproto,
};

// This function exists to be marked as weak.
// It can significantly speed up compilation by breaking up LLVM's SCC
// in the .pb.cc translation units. Large translation units see a
// reduction of more than 35% of walltime for optimized builds. Without
// the weak attribute all the messages in the file, including all the
// vtables and everything they use become part of the same SCC through
// a cycle like:
// GetMetadata -> descriptor table -> default instances ->
//   vtables -> GetMetadata
// By adding a weak function here we break the connection from the
// individual vtables back into the descriptor table.
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_google_2ftype_2flatlng_2eproto_getter() {
  return &descriptor_table_google_2ftype_2flatlng_2eproto;
}
// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2
static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_google_2ftype_2flatlng_2eproto(&descriptor_table_google_2ftype_2flatlng_2eproto);
namespace google {
namespace type {
// ===================================================================

class LatLng::_Internal {
 public:
};

LatLng::LatLng(::google::protobuf::Arena* arena)
    : ::google::protobuf::Message(arena) {
  SharedCtor(arena);
  // @@protoc_insertion_point(arena_constructor:google.type.LatLng)
}
LatLng::LatLng(
    ::google::protobuf::Arena* arena, const LatLng& from)
    : LatLng(arena) {
  MergeFrom(from);
}
inline PROTOBUF_NDEBUG_INLINE LatLng::Impl_::Impl_(
    ::google::protobuf::internal::InternalVisibility visibility,
    ::google::protobuf::Arena* arena)
      : _cached_size_{0} {}

inline void LatLng::SharedCtor(::_pb::Arena* arena) {
  new (&_impl_) Impl_(internal_visibility(), arena);
  ::memset(reinterpret_cast<char *>(&_impl_) +
               offsetof(Impl_, latitude_),
           0,
           offsetof(Impl_, longitude_) -
               offsetof(Impl_, latitude_) +
               sizeof(Impl_::longitude_));
}
LatLng::~LatLng() {
  // @@protoc_insertion_point(destructor:google.type.LatLng)
  _internal_metadata_.Delete<::google::protobuf::UnknownFieldSet>();
  SharedDtor();
}
inline void LatLng::SharedDtor() {
  ABSL_DCHECK(GetArena() == nullptr);
  _impl_.~Impl_();
}

PROTOBUF_NOINLINE void LatLng::Clear() {
// @@protoc_insertion_point(message_clear_start:google.type.LatLng)
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  ::memset(&_impl_.latitude_, 0, static_cast<::size_t>(
      reinterpret_cast<char*>(&_impl_.longitude_) -
      reinterpret_cast<char*>(&_impl_.latitude_)) + sizeof(_impl_.longitude_));
  _internal_metadata_.Clear<::google::protobuf::UnknownFieldSet>();
}

const char* LatLng::_InternalParse(
    const char* ptr, ::_pbi::ParseContext* ctx) {
  ptr = ::_pbi::TcParser::ParseLoop(this, ptr, ctx, &_table_.header);
  return ptr;
}


PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1
const ::_pbi::TcParseTable<1, 2, 0, 0, 2> LatLng::_table_ = {
  {
    0,  // no _has_bits_
    0, // no _extensions_
    2, 8,  // max_field_number, fast_idx_mask
    offsetof(decltype(_table_), field_lookup_table),
    4294967292,  // skipmap
    offsetof(decltype(_table_), field_entries),
    2,  // num_field_entries
    0,  // num_aux_entries
    offsetof(decltype(_table_), field_names),  // no aux_entries
    &_LatLng_default_instance_._instance,
    ::_pbi::TcParser::GenericFallback,  // fallback
  }, {{
    // double longitude = 2;
    {::_pbi::TcParser::FastF64S1,
     {17, 63, 0, PROTOBUF_FIELD_OFFSET(LatLng, _impl_.longitude_)}},
    // double latitude = 1;
    {::_pbi::TcParser::FastF64S1,
     {9, 63, 0, PROTOBUF_FIELD_OFFSET(LatLng, _impl_.latitude_)}},
  }}, {{
    65535, 65535
  }}, {{
    // double latitude = 1;
    {PROTOBUF_FIELD_OFFSET(LatLng, _impl_.latitude_), 0, 0,
    (0 | ::_fl::kFcSingular | ::_fl::kDouble)},
    // double longitude = 2;
    {PROTOBUF_FIELD_OFFSET(LatLng, _impl_.longitude_), 0, 0,
    (0 | ::_fl::kFcSingular | ::_fl::kDouble)},
  }},
  // no aux_entries
  {{
  }},
};

::uint8_t* LatLng::_InternalSerialize(
    ::uint8_t* target,
    ::google::protobuf::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:google.type.LatLng)
  ::uint32_t cached_has_bits = 0;
  (void)cached_has_bits;

  // double latitude = 1;
  static_assert(sizeof(::uint64_t) == sizeof(double),
                "Code assumes ::uint64_t and double are the same size.");
  double tmp_latitude = this->_internal_latitude();
  ::uint64_t raw_latitude;
  memcpy(&raw_latitude, &tmp_latitude, sizeof(tmp_latitude));
  if (raw_latitude != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteDoubleToArray(
        1, this->_internal_latitude(), target);
  }

  // double longitude = 2;
  static_assert(sizeof(::uint64_t) == sizeof(double),
                "Code assumes ::uint64_t and double are the same size.");
  double tmp_longitude = this->_internal_longitude();
  ::uint64_t raw_longitude;
  memcpy(&raw_longitude, &tmp_longitude, sizeof(tmp_longitude));
  if (raw_longitude != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteDoubleToArray(
        2, this->_internal_longitude(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target =
        ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
            _internal_metadata_.unknown_fields<::google::protobuf::UnknownFieldSet>(::google::protobuf::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:google.type.LatLng)
  return target;
}

::size_t LatLng::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:google.type.LatLng)
  ::size_t total_size = 0;

  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // double latitude = 1;
  static_assert(sizeof(::uint64_t) == sizeof(double),
                "Code assumes ::uint64_t and double are the same size.");
  double tmp_latitude = this->_internal_latitude();
  ::uint64_t raw_latitude;
  memcpy(&raw_latitude, &tmp_latitude, sizeof(tmp_latitude));
  if (raw_latitude != 0) {
    total_size += 9;
  }

  // double longitude = 2;
  static_assert(sizeof(::uint64_t) == sizeof(double),
                "Code assumes ::uint64_t and double are the same size.");
  double tmp_longitude = this->_internal_longitude();
  ::uint64_t raw_longitude;
  memcpy(&raw_longitude, &tmp_longitude, sizeof(tmp_longitude));
  if (raw_longitude != 0) {
    total_size += 9;
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::google::protobuf::Message::ClassData LatLng::_class_data_ = {
    LatLng::MergeImpl,
    nullptr,  // OnDemandRegisterArenaDtor
};
const ::google::protobuf::Message::ClassData* LatLng::GetClassData() const {
  return &_class_data_;
}

void LatLng::MergeImpl(::google::protobuf::Message& to_msg, const ::google::protobuf::Message& from_msg) {
  auto* const _this = static_cast<LatLng*>(&to_msg);
  auto& from = static_cast<const LatLng&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:google.type.LatLng)
  ABSL_DCHECK_NE(&from, _this);
  ::uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  static_assert(sizeof(::uint64_t) == sizeof(double),
                "Code assumes ::uint64_t and double are the same size.");
  double tmp_latitude = from._internal_latitude();
  ::uint64_t raw_latitude;
  memcpy(&raw_latitude, &tmp_latitude, sizeof(tmp_latitude));
  if (raw_latitude != 0) {
    _this->_internal_set_latitude(from._internal_latitude());
  }
  static_assert(sizeof(::uint64_t) == sizeof(double),
                "Code assumes ::uint64_t and double are the same size.");
  double tmp_longitude = from._internal_longitude();
  ::uint64_t raw_longitude;
  memcpy(&raw_longitude, &tmp_longitude, sizeof(tmp_longitude));
  if (raw_longitude != 0) {
    _this->_internal_set_longitude(from._internal_longitude());
  }
  _this->_internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(from._internal_metadata_);
}

void LatLng::CopyFrom(const LatLng& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:google.type.LatLng)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

PROTOBUF_NOINLINE bool LatLng::IsInitialized() const {
  return true;
}

::_pbi::CachedSize* LatLng::AccessCachedSize() const {
  return &_impl_._cached_size_;
}
void LatLng::InternalSwap(LatLng* PROTOBUF_RESTRICT other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::google::protobuf::internal::memswap<
      PROTOBUF_FIELD_OFFSET(LatLng, _impl_.longitude_)
      + sizeof(LatLng::_impl_.longitude_)
      - PROTOBUF_FIELD_OFFSET(LatLng, _impl_.latitude_)>(
          reinterpret_cast<char*>(&_impl_.latitude_),
          reinterpret_cast<char*>(&other->_impl_.latitude_));
}

::google::protobuf::Metadata LatLng::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_google_2ftype_2flatlng_2eproto_getter, &descriptor_table_google_2ftype_2flatlng_2eproto_once,
      file_level_metadata_google_2ftype_2flatlng_2eproto[0]);
}
// @@protoc_insertion_point(namespace_scope)
}  // namespace type
}  // namespace google
namespace google {
namespace protobuf {
}  // namespace protobuf
}  // namespace google
// @@protoc_insertion_point(global_scope)
#include "google/protobuf/port_undef.inc"
