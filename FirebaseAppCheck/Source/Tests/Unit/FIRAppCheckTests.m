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

#import <XCTest/XCTest.h>

// TODO: Consider using manually implemented fakes instead of OCMock
// (see also go/srl-dev/why-fakes#no-ocmock)
#import <OCMock/OCMock.h>

#import "FBLPromise+Testing.h"

#import <FirebaseAppCheckInterop/FIRAppCheckInterop.h>
#import <FirebaseAppCheckInterop/FIRAppCheckTokenInterop.h>
#import "FirebaseAppCheck/Source/Library/Core/Private/FIRAppCheckInternal.h"

#import "FirebaseAppCheck/Source/Library/Core/FIRAppCheckToken+Interop.h"
#import "FirebaseAppCheck/Source/Library/Core/Storage/FIRAppCheckStorage.h"

@interface FIRAppCheck (Tests) <FIRAppCheckInterop>
- (instancetype)initWithAppName:(NSString *)appName
               appCheckProvider:(id<FIRAppCheckProvider>)appCheckProvider
                        storage:(id<FIRAppCheckStorageProtocol>)storage;
@end

@interface FIRAppCheckTests : XCTestCase

@property(nonatomic) NSString *appName;
@property(nonatomic) OCMockObject<FIRAppCheckStorageProtocol> *mockStorage;
@property(nonatomic) OCMockObject<FIRAppCheckProvider> *mockAppCheckProvider;
@property(nonatomic) FIRAppCheck<FIRAppCheckInterop> *appCheck;

@end

@implementation FIRAppCheckTests

- (void)setUp {
  [super setUp];

  self.appName = @"FIRAppCheckTests";
  self.mockStorage = OCMProtocolMock(@protocol(FIRAppCheckStorageProtocol));
  self.mockAppCheckProvider = OCMProtocolMock(@protocol(FIRAppCheckProvider));
  self.appCheck = [[FIRAppCheck alloc] initWithAppName:self.appName
                                      appCheckProvider:self.mockAppCheckProvider
                                               storage:self.mockStorage];
}

- (void)tearDown {
  self.appCheck = nil;
  [self.mockAppCheckProvider stopMocking];
  self.mockAppCheckProvider = nil;
  [self.mockStorage stopMocking];
  self.mockStorage = nil;

  [super tearDown];
}

- (void)testGetToken_WhenNoCache_Success {
  // 1. Expect token to be requested from storage.
  OCMExpect([self.mockStorage getToken]).andReturn([FBLPromise resolvedWith:nil]);

  // 2. Expect token requested from app check provider.
  FIRAppCheckToken *tokenToReturn = [[FIRAppCheckToken alloc] initWithToken:@"valid"
                                                             expirationDate:[NSDate distantFuture]];
  id completionArg = [OCMArg invokeBlockWithArgs:tokenToReturn, [NSNull null], nil];
  OCMExpect([self.mockAppCheckProvider getTokenWithCompletion:completionArg]);

  // 3. Expect new token to be stored.
  OCMExpect([self.mockStorage setToken:tokenToReturn])
      .andReturn([FBLPromise resolvedWith:tokenToReturn]);

  // 4. Request token.
  XCTestExpectation *getTokenExpectation = [self expectationWithDescription:@"getToken"];
  [self.appCheck getTokenWithCompletion:^(id<FIRAppCheckTokenInterop> _Nullable token,
                                          NSError *_Nullable error) {
    [getTokenExpectation fulfill];

    XCTAssertNil(error);
    XCTAssertNotNil(token);
    XCTAssertEqualObjects(token.token, tokenToReturn.token);
    XCTAssertEqualObjects(token.expirationDate, tokenToReturn.expirationDate);
  }];

  // 5. Wait for expectations and validate mocks.
  [self waitForExpectations:@[ getTokenExpectation ] timeout:0.5];
  OCMVerifyAll(self.mockStorage);
  OCMVerifyAll(self.mockAppCheckProvider);
}

- (void)testGetToken_WhenChachedTokenIsValid_Success {
  FIRAppCheckToken *cachedToken = [[FIRAppCheckToken alloc] initWithToken:@"valid"
                                                           expirationDate:[NSDate distantFuture]];

  // 1. Expect token to be requested from storage.
  OCMExpect([self.mockStorage getToken]).andReturn([FBLPromise resolvedWith:cachedToken]);

  // 2. Don't expect token requested from app check provider.
  OCMReject([self.mockAppCheckProvider getTokenWithCompletion:[OCMArg any]]);

  // 3. Request token.
  XCTestExpectation *getTokenExpectation = [self expectationWithDescription:@"getToken"];
  [self.appCheck getTokenWithCompletion:^(id<FIRAppCheckTokenInterop> _Nullable token,
                                          NSError *_Nullable error) {
    [getTokenExpectation fulfill];

    XCTAssertNil(error);
    XCTAssertNotNil(token);
    XCTAssertEqualObjects(token.token, cachedToken.token);
    XCTAssertEqualObjects(token.expirationDate, cachedToken.expirationDate);
  }];

  // 4. Wait for expectations and validate mocks.
  [self waitForExpectations:@[ getTokenExpectation ] timeout:0.5];
  OCMVerifyAll(self.mockStorage);
  OCMVerifyAll(self.mockAppCheckProvider);
}

- (void)testGetToken_WhenCachedTokenExpired_Success {
  FIRAppCheckToken *cachedToken = [[FIRAppCheckToken alloc] initWithToken:@"valid"
                                                           expirationDate:[NSDate date]];

  // 1. Expect token to be requested from storage.
  OCMExpect([self.mockStorage getToken]).andReturn([FBLPromise resolvedWith:cachedToken]);

  // 2. Expect token requested from app check provider.
  FIRAppCheckToken *tokenToReturn = [[FIRAppCheckToken alloc] initWithToken:@"valid"
                                                             expirationDate:[NSDate distantFuture]];
  id completionArg = [OCMArg invokeBlockWithArgs:tokenToReturn, [NSNull null], nil];
  OCMExpect([self.mockAppCheckProvider getTokenWithCompletion:completionArg]);

  // 3. Expect new token to be stored.
  OCMExpect([self.mockStorage setToken:tokenToReturn])
      .andReturn([FBLPromise resolvedWith:tokenToReturn]);

  // 4. Request token.
  XCTestExpectation *getTokenExpectation = [self expectationWithDescription:@"getToken"];
  [self.appCheck getTokenWithCompletion:^(id<FIRAppCheckTokenInterop> _Nullable token,
                                          NSError *_Nullable error) {
    [getTokenExpectation fulfill];

    XCTAssertNil(error);
    XCTAssertNotNil(token);
    XCTAssertEqualObjects(token.token, tokenToReturn.token);
    XCTAssertEqualObjects(token.expirationDate, tokenToReturn.expirationDate);
  }];

  // 5. Wait for expectations and validate mocks.
  [self waitForExpectations:@[ getTokenExpectation ] timeout:0.5];
  OCMVerifyAll(self.mockStorage);
  OCMVerifyAll(self.mockAppCheckProvider);
}

- (void)testGetToken_AppCheckProviderError {
  // 1. Expect token to be requested from storage.
  OCMExpect([self.mockStorage getToken]).andReturn([FBLPromise resolvedWith:nil]);

  // 2. Expect token requested from app check provider.
  NSError *providerError = [NSError errorWithDomain:@"FIRAppCheckTests" code:-1 userInfo:nil];
  id completionArg = [OCMArg invokeBlockWithArgs:[NSNull null], providerError, nil];
  OCMExpect([self.mockAppCheckProvider getTokenWithCompletion:completionArg]);

  // 3. Don't expect token requested from app check provider.
  OCMReject([self.mockAppCheckProvider getTokenWithCompletion:[OCMArg any]]);

  // 4. Request token.
  XCTestExpectation *getTokenExpectation = [self expectationWithDescription:@"getToken"];
  [self.appCheck getTokenWithCompletion:^(id<FIRAppCheckTokenInterop> _Nullable token,
                                          NSError *_Nullable error) {
    [getTokenExpectation fulfill];

    XCTAssertNil(token);

    // TODO: Expect a public domain error to be returned - not the internal one.
    XCTAssertEqualObjects(error, providerError);
  }];

  // 5. Wait for expectations and validate mocks.
  [self waitForExpectations:@[ getTokenExpectation ] timeout:0.5];
  OCMVerifyAll(self.mockStorage);
  OCMVerifyAll(self.mockAppCheckProvider);
}

@end
