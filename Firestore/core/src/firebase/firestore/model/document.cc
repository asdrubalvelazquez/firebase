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

#include "Firestore/core/src/firebase/firestore/model/document.h"

#include "Firestore/core/src/firebase/firestore/util/firebase_assert.h"

namespace firebase {
namespace firestore {
namespace model {

Document::Document(const FieldValue& data,
                   const DocumentKey& key,
                   const SnapshotVersion& version,
                   bool has_local_mutations)
    : MaybeDocument(key, version),
      data_(data),
      has_local_mutations_(has_local_mutations) {
  type_ = Type::Document;
  FIREBASE_ASSERT(FieldValue::Type::Object == data.type());
}

}  // namespace model
}  // namespace firestore
}  // namespace firebase
