// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#import <OCMock/OCMock.h>

#import "FirebaseCore/Sources/Private/FIROptionsInternal.h"
#import "SharedTestUtilities/FIROptionsFake.h"

@implementation FIROptionsFake

// Swift Package manager does not allow a test project to override a bundle in an app (or library).

+ (FIROptions *)fakeFIROptions {
  NSString *const kAndroidClientID = @"correct_android_client_id";
  // FIS requires 39 characters starting with A.
  NSString *const kAPIKey = @"A23456789012345678901234567890123456789";
  NSString *const kCustomizedAPIKey = @"customized_api_key";
  NSString *const kClientID = @"correct_client_id";
  NSString *const kTrackingID = @"correct_tracking_id";
  NSString *const kGCMSenderID = @"correct_gcm_sender_id";
  NSString *const kGoogleAppID = @"1:123:ios:123abc";
  NSString *const kDatabaseURL = @"https://abc-xyz-123.firebaseio.com";
  NSString *const kStorageBucket = @"project-id-123.storage.firebase.com";
  NSString *const kDeepLinkURLScheme = @"comgoogledeeplinkurl";
  NSString *const kNewDeepLinkURLScheme = @"newdeeplinkurlfortest";
  NSString *const kBundleID = @"com.google.FirebaseSDKTests";
  NSString *const kProjectID = @"Mocked Project ID";
  return [[FIROptions alloc] initInternalWithOptionsDictionary:@{
    kFIRAPIKey : kAPIKey,
    kFIRBundleID : kBundleID,
    kFIRClientID : kClientID,
    kFIRDatabaseURL : kDatabaseURL,
    kFIRGCMSenderID : kGCMSenderID,
    kFIRGoogleAppID : kGoogleAppID,
    kFIRProjectID : kProjectID,
    kFIRStorageBucket : kStorageBucket,
    kFIRTrackingID : kTrackingID
  }];
}

@end
