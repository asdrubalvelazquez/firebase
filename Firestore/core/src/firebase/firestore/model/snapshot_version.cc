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

#include "Firestore/core/src/firebase/firestore/model/snapshot_version.h"

#include <chrono>  // NOLINT(build/c++11)

namespace firebase {
namespace firestore {
namespace model {

SnapshotVersion::SnapshotVersion(const Timestamp& timestamp)
    : timestamp_(timestamp) {
}

const SnapshotVersion& SnapshotVersion::None() {
  static const SnapshotVersion kNone(Timestamp{});
  return kNone;
}

int64_t SnapshotVersion::ToMicroseconds() const {
  namespace chr = std::chrono;

  auto microseconds = chr::duration_cast<chr::microseconds>(
      timestamp_.ToTimePoint().time_since_epoch());
  return static_cast<int64_t>(microseconds.count());
}

}  // namespace model
}  // namespace firestore
}  // namespace firebase
