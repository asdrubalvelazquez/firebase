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

#include "Firestore/core/src/firebase/firestore/api/query_snapshot.h"

#include <utility>

#import "Firestore/Source/API/FIRDocumentChange+Internal.h"
#import "Firestore/Source/API/FIRDocumentSnapshot+Internal.h"
#import "Firestore/Source/API/FIRFirestore+Internal.h"
#import "Firestore/Source/API/FIRQuery+Internal.h"
#import "Firestore/Source/Core/FSTQuery.h"
#import "Firestore/Source/Model/FSTDocument.h"

#include "Firestore/core/src/firebase/firestore/api/input_validation.h"
#include "Firestore/core/src/firebase/firestore/core/view_snapshot.h"
#include "Firestore/core/src/firebase/firestore/model/document_set.h"
#include "Firestore/core/src/firebase/firestore/util/hard_assert.h"
#include "Firestore/core/src/firebase/firestore/util/objc_compatibility.h"
#include "absl/types/optional.h"

NS_ASSUME_NONNULL_BEGIN

namespace firebase {
namespace firestore {
namespace api {

namespace objc = util::objc;
using api::Firestore;
using core::DocumentViewChange;
using core::ViewSnapshot;
using model::DocumentSet;

bool operator==(const QuerySnapshot& lhs, const QuerySnapshot& rhs) {
  return lhs.firestore_ == rhs.firestore_ &&
         objc::Equals(lhs.internal_query_, rhs.internal_query_) &&
         lhs.snapshot_ == rhs.snapshot_ && lhs.metadata_ == rhs.metadata_;
}

size_t QuerySnapshot::Hash() const {
  return util::Hash(firestore_, internal_query_, snapshot_, metadata_);
}

void QuerySnapshot::ForEachDocument(
    const std::function<void(DocumentSnapshot)>& callback) const {
  DocumentSet documentSet = snapshot_.documents();
  bool from_cache = metadata_.from_cache();

  for (FSTDocument* document : documentSet) {
    bool has_pending_writes = snapshot_.mutated_keys().contains(document.key);
    DocumentSnapshot snap(firestore_, document.key, document, from_cache,
                          has_pending_writes);
    callback(std::move(snap));
  }
}

static DocumentChange::Type DocumentChangeTypeForChange(
    const DocumentViewChange& change) {
  switch (change.type()) {
    case DocumentViewChange::Type::kAdded:
      return DocumentChange::Type::Added;
    case DocumentViewChange::Type::kModified:
    case DocumentViewChange::Type::kMetadata:
      return DocumentChange::Type::Modified;
    case DocumentViewChange::Type::kRemoved:
      return DocumentChange::Type::Removed;
  }

  HARD_FAIL("Unknown DocumentViewChange::Type: %s", change.type());
}

void QuerySnapshot::ForEachChange(
    bool includeMetadataChanges,
    const std::function<void(DocumentChange)>& callback) const {
  if (includeMetadataChanges && snapshot_.excludes_metadata_changes()) {
    ThrowInvalidArgument("To include metadata changes with your document "
                         "changes, you must call "
                         "addSnapshotListener(includeMetadataChanges:true).");
  }

  if (snapshot_.old_documents().empty()) {
    // Special case the first snapshot because index calculation is easy and
    // fast. Also all changes on the first snapshot are adds so there are also
    // no metadata-only changes to filter out.
    FSTDocument* lastDocument = nil;
    size_t index = 0;
    for (const DocumentViewChange& change : snapshot_.document_changes()) {
      FSTDocument* doc = change.document();
      SnapshotMetadata metadata = SnapshotMetadata(
          /*hasPendingWrites=*/snapshot_.mutated_keys().contains(doc.key),
          /*fromCache=*/snapshot_.from_cache());
      DocumentSnapshot document =
          DocumentSnapshot(firestore_, doc.key, doc, std::move(metadata));

      HARD_ASSERT(change.type() == DocumentViewChange::Type::kAdded,
                  "Invalid event type for first snapshot");
      HARD_ASSERT(!lastDocument || snapshot_.query().comparator(
                                       lastDocument, change.document()) ==
                                       NSOrderedAscending,
                  "Got added events in wrong order");

      callback(DocumentChange(DocumentChange::Type::Added, document,
                              DocumentChange::npos, index++));
    }

  } else {
    // A DocumentSet that is updated incrementally as changes are applied to use
    // to lookup the index of a document.
    DocumentSet indexTracker = snapshot_.old_documents();
    for (const DocumentViewChange& change : snapshot_.document_changes()) {
      if (!includeMetadataChanges &&
          change.type() == DocumentViewChange::Type::kMetadata) {
        continue;
      }

      FSTDocument* doc = change.document();
      SnapshotMetadata metadata = SnapshotMetadata(
          /*hasPendingWrites=*/snapshot_.mutated_keys().contains(doc.key),
          /*fromCache=*/snapshot_.from_cache());
      DocumentSnapshot document =
          DocumentSnapshot(firestore_, doc.key, doc, std::move(metadata));

      size_t oldIndex = DocumentChange::npos;
      size_t newIndex = DocumentChange::npos;
      if (change.type() != DocumentViewChange::Type::kAdded) {
        oldIndex = indexTracker.IndexOf(change.document().key);
        HARD_ASSERT(oldIndex != DocumentSet::npos,
                    "Index for document not found");
        indexTracker = indexTracker.erase(change.document().key);
      }
      if (change.type() != DocumentViewChange::Type::kRemoved) {
        indexTracker = indexTracker.insert(change.document());
        newIndex = indexTracker.IndexOf(change.document().key);
      }

      DocumentChange::Type type = DocumentChangeTypeForChange(change);
      callback(DocumentChange(type, document, oldIndex, newIndex));
    }
  }
}

}  // namespace api
}  // namespace firestore
}  // namespace firebase

NS_ASSUME_NONNULL_END
