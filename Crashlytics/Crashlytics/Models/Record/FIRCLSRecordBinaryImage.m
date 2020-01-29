/*
 * Copyright 2020 Google
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

#import "FIRCLSRecordBinaryImage.h"

@implementation FIRCLSRecordBinaryImage

+ (NSArray<FIRCLSRecordBinaryImage *> *)binaryImagesFromDictionaries:
    (NSArray<NSDictionary *> *)dicts {
  NSMutableArray<FIRCLSRecordBinaryImage *> *images =
      [[NSMutableArray<FIRCLSRecordBinaryImage *> alloc] init];
  for (NSDictionary *dict in dicts) {
    [images addObject:[[FIRCLSRecordBinaryImage alloc] initWithDict:dict[@"load"]]];
  }
  return images;
}

- (instancetype)initWithDict:(NSDictionary *)dict {
  self = [super initWithDict:dict];
  if (self) {
    _path = dict[@"path"];
    _uuid = dict[@"uuid"];
    _base = [dict[@"base"] unsignedIntegerValue];
    _size = [dict[@"size"] unsignedIntegerValue];
  }
  return self;
}

@end
