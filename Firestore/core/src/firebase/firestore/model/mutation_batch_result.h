/*
 * Copyright 2019 Google
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

#ifndef FIRESTORE_CORE_SRC_FIREBASE_FIRESTORE_MODEL_MUTATION_BATCH_RESULT_H_
#define FIRESTORE_CORE_SRC_FIREBASE_FIRESTORE_MODEL_MUTATION_BATCH_RESULT_H_

#include <iosfwd>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Firestore/core/include/firebase/firestore/timestamp.h"
#include "Firestore/core/src/firebase/firestore/model/document_key.h"
#include "Firestore/core/src/firebase/firestore/model/document_key_set.h"
#include "Firestore/core/src/firebase/firestore/model/document_map.h"
#include "Firestore/core/src/firebase/firestore/model/mutation.h"
#include "Firestore/core/src/firebase/firestore/model/mutation_batch.h"
#include "Firestore/core/src/firebase/firestore/model/types.h"

namespace firebase {
namespace firestore {
namespace model {

using DocumentVersionMap =
    std::unordered_map<DocumentKey, SnapshotVersion, DocumentKeyHash>;

/** The result of applying a mutation batch to the backend. */
class MutationBatchResult {
 public:
  /**
   * Creates a new FSTMutationBatchResult for the given batch and results. There
   * must be one result for each mutation in the batch. This static factory
   * caches a document=>version mapping (as docVersions).
   */
  MutationBatchResult(MutationBatch batch,
                      SnapshotVersion commit_version,
                      std::vector<MutationResult> mutation_results,
                      absl::optional<nanopb::ByteString> stream_token);

  const MutationBatch& batch() const {
    return batch_;
  }

  const SnapshotVersion& commit_version() const {
    return commit_version_;
  }

  const std::vector<MutationResult>& mutation_results() const {
    return mutation_results_;
  }

  const absl::optional<nanopb::ByteString> stream_token() const {
    return stream_token_;
  }

  const DocumentVersionMap& doc_versions() const {
    return doc_versions_;
  }

  std::string ToString() const;

  friend std::ostream& operator<<(std::ostream& os,
                                  const MutationBatchResult& result);

 private:
  MutationBatch batch_;
  SnapshotVersion commit_version_;
  std::vector<MutationResult> mutation_results_;
  absl::optional<nanopb::ByteString> stream_token_;
  DocumentVersionMap doc_versions_;
};

}  // namespace model
}  // namespace firestore
}  // namespace firebase

#endif  // FIRESTORE_CORE_SRC_FIREBASE_FIRESTORE_MODEL_MUTATION_BATCH_RESULT_H_
