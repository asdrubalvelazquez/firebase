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

#include "Firestore/core/src/firebase/firestore/local/leveldb_migrations.h"

#include <string>
#include <utility>

#include "Firestore/Protos/nanopb/firestore/local/mutation.nanopb.h"
#include "Firestore/Protos/nanopb/firestore/local/target.nanopb.h"
#include "Firestore/core/src/firebase/firestore/local/leveldb_key.h"
#include "Firestore/core/src/firebase/firestore/nanopb/reader.h"
#include "Firestore/core/src/firebase/firestore/nanopb/writer.h"
#include "absl/strings/match.h"

namespace firebase {
namespace firestore {
namespace local {

using leveldb::Iterator;
using leveldb::Slice;
using leveldb::Status;
using leveldb::WriteOptions;
using nanopb::Reader;
using nanopb::Writer;

namespace {

/**
 * Schema version for the iOS client.
 *
 * Note that tables aren't a concept in LevelDB. They exist in our schema as
 * just prefixes on keys. This means tables don't need to be created but they
 * also can't easily be dropped and re-created.
 *
 * Migrations:
 *   * Migration 1 used to ensure the target_global row existed, without
 *     clearing it. No longer required because migration 3 unconditionally
 *     clears it.
 *   * Migration 2 used to ensure that the target_global row had a correct count
 *     of targets. No longer required because migration 3 deletes them all.
 *   * Migration 3 deletes the entire query cache to deal with cache corruption
 *     related to limbo resolution. Addresses
 *     https://github.com/firebase/firebase-ios-sdk/issues/1548.
 */
const LevelDbMigrations::SchemaVersion kSchemaVersion = 3;

/**
 * Save the given version number as the current version of the schema of the
 * database.
 * @param version The version to save
 * @param transaction The transaction in which to save the new version number
 */
void SaveVersion(LevelDbMigrations::SchemaVersion version,
                 LevelDbTransaction* transaction) {
  std::string key = LevelDbVersionKey::Key();
  std::string version_string = std::to_string(version);
  transaction->Put(key, version_string);
}

void DeleteEverythingWithPrefix(const std::string& prefix, leveldb::DB* db) {
  bool more_deletes = true;
  while (more_deletes) {
    LevelDbTransaction transaction(db, "Delete everything with prefix");
    auto it = transaction.NewIterator();

    more_deletes = false;
    for (it->Seek(prefix); it->Valid() && absl::StartsWith(it->key(), prefix);
         it->Next()) {
      if (transaction.changed_keys() >= 1000) {
        more_deletes = true;
        break;
      }
      transaction.Delete(it->key());
    }

    transaction.Commit();
  }
}

/** Migration 3. */
void ClearQueryCache(leveldb::DB* db) {
  DeleteEverythingWithPrefix(LevelDbTargetKey::KeyPrefix(), db);
  DeleteEverythingWithPrefix(LevelDbDocumentTargetKey::KeyPrefix(), db);
  DeleteEverythingWithPrefix(LevelDbTargetDocumentKey::KeyPrefix(), db);
  DeleteEverythingWithPrefix(LevelDbQueryTargetKey::KeyPrefix(), db);

  LevelDbTransaction transaction(db, "Drop query cache");

  // Reset the target global entry too (to reset the target count).
  firestore_client_TargetGlobal target_global{};

  std::string bytes;
  Writer writer = Writer::Wrap(&bytes);
  writer.WriteNanopbMessage(firestore_client_TargetGlobal_fields,
                            &target_global);
  transaction.Put(LevelDbTargetGlobalKey::Key(), std::move(bytes));

  SaveVersion(3, &transaction);
  transaction.Commit();
}

void RemoveMutationDocuments(LevelDbTransaction *transaction,
                             absl::string_view user_id,
                             int32_t last_acknowledged_batch_id) {
  LevelDbDocumentMutationKey doc_key;
  std::string prefix = LevelDbDocumentMutationKey::KeyPrefix(user_id);

  auto it = transaction->NewIterator();
  it->Seek(prefix);
  for (; it->Valid() && absl::StartsWith(it->key(), prefix); it->Next()) {
    HARD_ASSERT(doc_key.Decode(it->key()),
                "Failed to decode document mutation key");
    if (doc_key.batch_id() <= last_acknowledged_batch_id) {
      transaction->Delete(it->key());
    }
  }
}

void RemoveMutationBatches(LevelDbTransaction *transaction,
                           absl::string_view user_id,
                           int32_t last_acknowledged_batch_id) {
  LevelDbMutationKey key;
  std::string mutations_key = LevelDbMutationKey::KeyPrefix(user_id);
  std::string last_key =
      LevelDbMutationKey::Key(user_id, last_acknowledged_batch_id);
  auto it = transaction->NewIterator();
  it->Seek(mutations_key);
  for (; it->Valid() && absl::StartsWith(it->key(), mutations_key) &&
         it->key() <= last_key;
       it->Next()) {
    transaction->Delete(it->key());
  }
}

/** Migration 4. */
void RemoveAcknowledgedMutations(leveldb::DB* db) {
  LevelDbTransaction transaction(db, "remove acknowledged mutations");
  std::string mutation_queue_start = LevelDbMutationQueueKey::KeyPrefix();

  LevelDbMutationQueueKey key;

  auto it = transaction.NewIterator();
  it->Seek(mutation_queue_start);
  for (; it->Valid() && absl::StartsWith(it->key(), mutation_queue_start);
       it->Next()) {
    HARD_ASSERT(key.Decode(it->key()), "Failed to decode mutation queue key");
    firestore_client_MutationQueue mutation_queue{};
    Reader reader = Reader::Wrap(it->value());
    reader.ReadNanopbMessage(firestore_client_MutationQueue_fields,
                             &mutation_queue);
    HARD_ASSERT(reader.status().ok(), "Failed to deserialize MutationQueue");
    RemoveMutationBatches(&transaction, key.user_id(),
                          mutation_queue.last_acknowledged_batch_id);
    RemoveMutationDocuments(&transaction, key.user_id(),
                            mutation_queue.last_acknowledged_batch_id);
  }

  SaveVersion(4, &transaction);
  transaction.Commit();
}

}  // namespace

LevelDbMigrations::SchemaVersion LevelDbMigrations::ReadSchemaVersion(
    LevelDbTransaction* transaction) {
  std::string key = LevelDbVersionKey::Key();
  std::string version_string;
  Status status = transaction->Get(key, &version_string);
  if (status.IsNotFound()) {
    return 0;
  } else {
    return stoi(version_string);
  }
}

void LevelDbMigrations::RunMigrations(leveldb::DB* db) {
  RunMigrations(db, kSchemaVersion);
}

void LevelDbMigrations::RunMigrations(leveldb::DB* db,
                                      SchemaVersion to_version) {
  LevelDbTransaction transaction{db, "Read schema version"};
  SchemaVersion from_version = ReadSchemaVersion(&transaction);

  // This must run unconditionally because schema migrations were added to iOS
  // after the first release. There may be clients that have never run any
  // migrations that have existing targets.
  if (from_version < 3 && to_version >= 3) {
    ClearQueryCache(db);
  }
  if (from_version < 4 && to_version >= 4) {
    RemoveAcknowledgedMutations(db);
  }
}

}  // namespace local
}  // namespace firestore
}  // namespace firebase
