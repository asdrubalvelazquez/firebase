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

#ifndef FIRESTORE_CORE_SRC_FIREBASE_FIRESTORE_AUTH_TOKEN_H_
#define FIRESTORE_CORE_SRC_FIREBASE_FIRESTORE_AUTH_TOKEN_H_

#include <string>

#include "Firestore/core/src/firebase/firestore/auth/user.h"
#include "Firestore/core/src/firebase/firestore/util/firebase_assert.h"
#include "absl/strings/string_view.h"

namespace firebase {
namespace firestore {
namespace auth {

/**
 * The current User and the authentication token provided by the underlying
 * authentication mechanism. This is the result of calling
 * CredentialsProvider::GetToken().
 *
 * ## Portability notes: no TokenType on iOS
 *
 * The TypeScript client supports 1st party Oauth tokens (for the Firebase
 * Console to auth as the developer) and OAuth2 tokens for the node.js sdk to
 * auth with a service account. We don't have plans to support either case on
 * mobile so there's no TokenType here.
 */
// TODO(zxu123): Make this support token-type for desktop workflow.
class Token {
 public:
  Token(const absl::string_view token, const User& user);

  /** The actual raw token. */
  const std::string& token() const {
    FIREBASE_ASSERT(is_valid_);
    return token_;
  }

  /**
   * The user with which the token is associated (used for persisting user
   * state on disk, etc.).
   */
  const User& user() const {
    return user_;
  }

  /**
   * Whether the token is a valid one.
   *
   * ## Portability notes: Invalid token is the equivalent of nil in the iOS
   * token implementation. We use value instead of pointer for Token instance in
   * the C++ migration.
   */
  bool is_valid() const {
    return is_valid_;
  }

  /** Returns an invalid token. */
  static const Token& Invalid();

 private:
  Token();

  const std::string token_;
  const User user_;
  const bool is_valid_;
};

}  // namespace auth
}  // namespace firestore
}  // namespace firebase

#endif  // FIRESTORE_CORE_SRC_FIREBASE_FIRESTORE_AUTH_TOKEN_H_
