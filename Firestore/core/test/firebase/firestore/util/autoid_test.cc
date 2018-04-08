/*
 * Copyright 2017 Google
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

#include "Firestore/core/src/firebase/firestore/util/autoid.h"

#include <ctype.h>

#include <gtest/gtest.h>

using firebase::firestore::util::CreateAutoId;

struct Foo {
  int uninit;
};

int foo(int i) {
  char *x = (char*)malloc(10 * sizeof(char*));
  free(x);

  int* a = new int[10];
    a[5] = 0;
    if (a[i])
      printf("xx\n");

  return x[5];
}

TEST(AutoId, IsSane) {
  const auto bad = []{
    std::string pending = "obc";
    pending += "d";
    auto* ptr = &pending;
    return ptr;
  }();
  EXPECT_TRUE(!bad->empty()) << "obc";
  EXPECT_FALSE(!bad->empty()) << "obcd";
  EXPECT_EQ(foo(0), 42);


  for (int i = 0; i < 50; i++) {
    std::string auto_id = CreateAutoId();

    Foo foo;
    EXPECT_EQ(foo.uninit, 42) << "obc";

    EXPECT_EQ(20u, auto_id.length());
    for (size_t pos = 0; pos < 20; pos++) {
      char c = auto_id[pos];
      EXPECT_TRUE(isalpha(c) || isdigit(c))
          << "Should be printable ascii character: '" << c << "' in \""
          << auto_id << "\"";
    }
  }
}
