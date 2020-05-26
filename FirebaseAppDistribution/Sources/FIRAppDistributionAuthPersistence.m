// Copyright 2020 Google LLC
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
#import "FIRAppDistributionAuthPersistence+Private.h"

#import <GoogleUtilities/GULKeychainUtils.h>


NS_ASSUME_NONNULL_BEGIN

NSString *const kFIRAppDistributionAuthPersistenceErrorDomain =
    @"com.firebase.app_distribution.auth_persistence";

@implementation FIRAppDistributionAuthPersistence

+ (void)handleAuthStateError:(NSError **_Nullable)error
                 description:(NSString *)description
                        code:(FIRAppDistributionKeychainError)code
             underlyingError: (NSError *_Nullable) underlyingError {
  if (error) {
    NSDictionary *userInfo = underlyingError?  @{NSLocalizedDescriptionKey : description, NSUnderlyingErrorKey: underlyingError} : @{NSLocalizedDescriptionKey : description};
    *error = [NSError errorWithDomain:kFIRAppDistributionAuthPersistenceErrorDomain
                                 code:code
                             userInfo:userInfo];
  }
}

+ (BOOL)clearAuthState:(NSError **_Nullable)error {
  NSMutableDictionary *keychainQuery = [self getKeyChainQuery];
  NSError *keychainError;
  BOOL success = [GULKeychainUtils removeItemWithQuery:keychainQuery error:&keychainError];

  if (!success) {
    NSString *description = NSLocalizedString(
        @"Failed to clear auth state from keychain. Tester will overwrite data on sign in.",
        @"Error message for failure to retrieve auth state from keychain");
    [self handleAuthStateError:error
                   description:description
                          code:FIRAppDistributionErrorTokenDeletionFailure
               underlyingError:keychainError];
    return NO;
  }

  return YES;
}

+ (OIDAuthState *)retrieveAuthState:(NSError **_Nullable)error {
  NSMutableDictionary *keychainQuery = [self getKeyChainQuery];
  [keychainQuery setObject:(id)kCFBooleanTrue forKey:(id)kSecReturnData];
  [keychainQuery setObject:(id)kSecMatchLimitOne forKey:(id)kSecMatchLimit];
    
  NSError *keychainError;
  NSData *passwordData = [GULKeychainUtils getItemWithQuery:keychainQuery error:&keychainError];
  NSData *result = nil;

  if (!passwordData) {
    NSString *description = NSLocalizedString(
        @"Failed to retrieve auth state from keychain. Tester will have to sign in again.",
        @"Error message for failure to retrieve auth state from keychain. ");
    [self handleAuthStateError:error
                   description:description
                          code:FIRAppDistributionErrorTokenRetrievalFailure
               underlyingError:keychainError];
    return nil;
  }

  result = [passwordData copy];

  if (!result) {
    NSString *description =
        NSLocalizedString(@"Failed to unarchive auth state. Tester will have to sign in again.",
                          @"Error message for failure to retrieve auth state from keychain");
    [self handleAuthStateError:error
                   description:description
                          code:FIRAppDistributionErrorTokenRetrievalFailure
               underlyingError:nil];
    return nil;
  }

  OIDAuthState *authState = [self unarchiveKeychainResult:result];

  return authState;
}

+ (BOOL)persistAuthState:(OIDAuthState *)authState error:(NSError **_Nullable)error {
  NSData *authorizationData = [self archiveDataForKeychain:authState];
  NSMutableDictionary *keychainQuery = [self getKeyChainQuery];
  NSError *keychainError;
  // setItem performs an up-sert. Will automatically update the keychain enytry if it already exists.
  BOOL success = [GULKeychainUtils setItem:authorizationData withQuery:keychainQuery error:&keychainError];

  if (!success) {
    NSString *description = NSLocalizedString(
        @"Failed to persist auth state. Tester will have to sign in again after app close.",
        @"Error message for failure to persist auth state to keychain");
      
    [self handleAuthStateError:error
                   description:description
                          code:FIRAppDistributionErrorTokenPersistenceFailure
               underlyingError:keychainError];
    return NO;
  }

  return YES;
}

+ (NSMutableDictionary *)getKeyChainQuery {
  NSMutableDictionary *keychainQuery = [NSMutableDictionary
      dictionaryWithObjectsAndKeys:(id)kSecClassGenericPassword, (id)kSecClass, @"OAuth",
                                   (id)kSecAttrGeneric, @"OAuth", (id)kSecAttrAccount,
                                   @"fire-fad-auth", (id)kSecAttrService,
                                   (id)kSecAttrAccessibleAfterFirstUnlockThisDeviceOnly,
                                   (id)kSecAttrAccessible, nil];
  return keychainQuery;
}

+ (OIDAuthState *)unarchiveKeychainResult:(NSData *)result {
  return (OIDAuthState *)[NSKeyedUnarchiver unarchiveObjectWithData:result];
}

+ (NSData *)archiveDataForKeychain:(OIDAuthState *)data {
  return [NSKeyedArchiver archivedDataWithRootObject:data];
}

@end

NS_ASSUME_NONNULL_END
