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

#ifndef FIRESTORE_CORE_SRC_FIREBASE_FIRESTORE_NANOPB_BYTE_STRING_H_
#define FIRESTORE_CORE_SRC_FIREBASE_FIRESTORE_NANOPB_BYTE_STRING_H_

#if __OBJC__
#import <Foundation/Foundation.h>
#endif

#include <pb.h>

#include <cstdint>
#include <initializer_list>
#include <iosfwd>
#include <string>
#include <utility>
#include <vector>

#include "Firestore/core/src/firebase/firestore/util/comparison.h"
#include "absl/strings/string_view.h"

namespace firebase {
namespace firestore {
namespace nanopb {

/**
 * An immutable string-like object backed by a nanopb byte array. `ByteString`
 * owns its memory, has deep copy semantics, and creates a copy of any input
 * given to its constructors.
 *
 * `ByteString` is similar in spirit to com.google.protobuf.ByteString. It
 * serves mostly the same purpose: it's a holder of a byte array that's
 * compatible with the proto marshaling layer.
 *
 * Unlike protobuf's ByteString, nanopb doesn't supply this class so this class
 * additionally makes it possible to cheaply translate to and from raw
 * `pb_bytes_array_t*` values. ByteString allows taking values directly from
 * nanopb messages and avoids copying while doing so.
 */
class ByteString : public util::Comparable<ByteString> {
 public:
  ByteString() = default;

  /**
   * Creates a new ByteString whose backing byte array is a copy of the given
   * bytes.
   */
  ByteString(const void* value, size_t size);
  explicit ByteString(const std::vector<uint8_t>& value);

  /**
   * Creates a new ByteString whose backing byte array is a copy of the given
   * string.
   */
  explicit ByteString(const std::string& value);

  /**
   * Creates a new ByteString whose backing byte array is a copy of the given
   * string_view.
   */
  explicit ByteString(absl::string_view value);

#if __OBJC__
  /**
   * Creates a new ByteString whose backing byte array is a copy of the bytes
   * in the given NSData buffer.
   */
  explicit ByteString(NSData* value)
      : ByteString(value.bytes, static_cast<size_t>(value.length)) {
  }
#endif

  ByteString(std::initializer_list<uint8_t> value);

  /**
   * Creates a new ByteString whose backing byte array is a copy of the given
   * C string. The length is computed with `strlen`.
   */
  explicit ByteString(const char* value);

  ByteString(const ByteString& other);

  ByteString(ByteString&& other) noexcept;

  ~ByteString();

  ByteString& operator=(ByteString other) {
    swap(*this, other);
    return *this;
  }

  /**
   * Creates a new ByteString that copies the contents of the given byte array.
   */
  static ByteString Copy(const pb_bytes_array_t* bytes);

  /**
   * Creates a new ByteString that takes ownership of the given byte array. If
   * taking from a nanopb-created message struct, the caller should null out
   * the pointer there so that pb_release won't free the buffer that the
   * returned ByteString now owns.
   */
  static ByteString Take(pb_bytes_array_t* bytes);

  /**
   * Returns a pointer to the character data backing this ByteString. The
   * returned buffer is always non-null, even if the nanopb byte array is null.
   */
  const uint8_t* data() const;

  size_t size() const {
    return bytes_ ? bytes_->size : 0;
  }

  bool empty() const {
    return bytes_ == nullptr || bytes_->size == 0;
  }

  const uint8_t* begin() const {
    return data();
  }

  const uint8_t* end() const {
    return data() + size();
  }

  /**
   * Returns a const view of the raw underlying byte array pointer.
   *
   * This value may be null because nanopb (and protobuf generally) treat null
   * and empty byte arrays as equivalent.
   *
   * For actually reading the data in the buffer, prefer `data()` and `size()`
   * or `begin()` and `end()`, which handle this nullability for you.
   */
  const pb_bytes_array_t* get() const {
    return bytes_;
  }

  /**
   * Releases ownership of the backing byte array, and returns it to the caller.
   * The backing byte array is set to null.
   *
   * This value may be null because nanopb (and protobuf generally) treat null
   * and empty byte arrays as equivalent. Assigning a null value to a nanopb
   * message field will be treated as empty.
   */
  pb_bytes_array_t* release();

  /**
   * Copies the backing byte array into a new vector of bytes.
   */
  std::vector<uint8_t> ToVector() const;

#if __OBJC__
  NSData* ToNSData() const {
    return [[NSData alloc] initWithBytes:data() length:size()];
  }
#endif

  /**
   * Returns an absl::string_view view of this ByteString (without changing
   * ownership).
   */
  explicit operator absl::string_view() const;

  /**
   * Swaps the contents of the given Strings.
   */
  friend void swap(ByteString& lhs, ByteString& rhs) noexcept;

  util::ComparisonResult CompareTo(const ByteString& rhs) const;

  size_t Hash() const;

  std::string ToString() const;
  friend std::ostream& operator<<(std::ostream& out, const ByteString& str);

 private:
  explicit ByteString(pb_bytes_array_t* bytes) : bytes_{bytes} {
  }

  pb_bytes_array_t* bytes_ = nullptr;
};

}  // namespace nanopb
}  // namespace firestore
}  // namespace firebase

#endif  // FIRESTORE_CORE_SRC_FIREBASE_FIRESTORE_NANOPB_BYTE_STRING_H_
