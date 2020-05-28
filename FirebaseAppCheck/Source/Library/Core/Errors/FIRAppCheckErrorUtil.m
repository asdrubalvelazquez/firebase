/*
 * Copyright 2020 Google LLC
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

#import "FIRAppCheckErrorUtil.h"

@implementation FIRAppCheckErrorUtil

+ (NSError *)cachedTokenNotFound {
  // TODO: Implement
  return [self internalError];
}

+ (NSError *)cachedTokenExpired {
  // TODO: Implement
  return [self internalError];
}

+ (NSError *)APIErrorWithHTTPResponse:(NSHTTPURLResponse *)HTTPResponse
                                 data:(nullable NSData *)data {
  // TODO: Implement
  return [self internalError];
}

+ (NSError *)appCheckTokenResponseErrorWithMissingField:(NSString *)fieldName {
  // TODO: Implement
  return [self internalError];
}

+ (NSError *)JSONSerializationError:(NSError *)error {
  // TODO: Implement
  return [self internalError];
}

+ (NSError *)internalError {
  // TODO: Implement
  return [NSError errorWithDomain:@"AppCheck" code:-1 userInfo:nil];
}

@end
