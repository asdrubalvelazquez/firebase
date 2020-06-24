// Copyright 2020 Google
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

#import "ABTExperimentPayload.h"

static NSString *const kExperimentPayloadKeyExperimentID = @"experimentId";
static NSString *const kExperimentPayloadKeyVariantID = @"variantId";
static NSString *const kExperimentPayloadKeyExperimentStartTime = @"experimentStartTime";
static NSString *const kExperimentPayloadKeyTriggerEvent = @"triggerEvent";
static NSString *const kExperimentPayloadKeyTriggerTimeoutMillis = @"triggerTimeoutMillis";
static NSString *const kExperimentPayloadKeyTimeToLiveMillis = @"timeToLiveMillis";
static NSString *const kExperimentPayloadKeySetEventToLog = @"setEventToLog";
static NSString *const kExperimentPayloadKeyActivateEventToLog = @"activateEventToLog";
static NSString *const kExperimentPayloadKeyClearEventToLog = @"clearEventToLog";
static NSString *const kExperimentPayloadKeyTimeoutEventToLog = @"timeoutEventToLog";
static NSString *const kExperimentPayloadKeyTTLExpiryEventToLog = @"ttlExpiryEventToLog";
static NSString *const kExperimentPayloadKeyOverflowPolicy = @"overflowPolicy";
static NSString *const kExperimentPayloadKeyOngoingExperiments = @"ongoingExperiments";

@implementation ABTExperimentLite

- (instancetype)initWithExperimentId:(NSString *)experimentId {
  if (self = [super init]) {
    _experimentId = experimentId;
  }
  return self;
}

@end

@implementation ABTExperimentPayload

+ (NSDateFormatter *)experimentStartTimeFormatter {
  NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
  [dateFormatter setDateFormat:@"yyyy-MM-dd'T'HH:mm:ss.SSS'Z'"];
  [dateFormatter setTimeZone:[NSTimeZone timeZoneForSecondsFromGMT:0]];
  // Locale needs to be hardcoded. See
  // https://developer.apple.com/library/ios/#qa/qa1480/_index.html for more details.
  [dateFormatter setLocale:[[NSLocale alloc] initWithLocaleIdentifier:@"en_US_POSIX"]];
  [dateFormatter setTimeZone:[NSTimeZone timeZoneWithAbbreviation:@"UTC"]];
  return dateFormatter;
}

+ (instancetype)parseFromData:(NSData *)data {
  NSError *error;
  NSDictionary *experimentDictionary =
      [NSJSONSerialization JSONObjectWithData:data
                                      options:NSJSONReadingAllowFragments
                                        error:&error];
  if (error != nil) {
    return nil;
  } else {
    return [[ABTExperimentPayload alloc] initWithDictionary:experimentDictionary];
  }
}

- (instancetype)initWithDictionary:(NSDictionary<NSString *, id> *)dictionary {
  if (self = [super init]) {
    // Strings.
    _experimentId = dictionary[kExperimentPayloadKeyExperimentID];
    _variantId = dictionary[kExperimentPayloadKeyVariantID];
    _triggerEvent = dictionary[kExperimentPayloadKeyTriggerEvent];
    _setEventToLog = dictionary[kExperimentPayloadKeySetEventToLog];
    _activateEventToLog = dictionary[kExperimentPayloadKeyActivateEventToLog];
    _clearEventToLog = dictionary[kExperimentPayloadKeyClearEventToLog];
    _timeoutEventToLog = dictionary[kExperimentPayloadKeyTimeoutEventToLog];
    _ttlExpiryEventToLog = dictionary[kExperimentPayloadKeyTTLExpiryEventToLog];

    // Other.
    NSDate *experimentStartTime = [[[self class] experimentStartTimeFormatter]
        dateFromString:dictionary[kExperimentPayloadKeyExperimentStartTime]];
    _experimentStartTimeMillis =
        [@([experimentStartTime timeIntervalSince1970] * 1000) longLongValue];

    _triggerTimeoutMillis = [dictionary[kExperimentPayloadKeyTriggerTimeoutMillis] longLongValue];
    _timeToLiveMillis = [dictionary[kExperimentPayloadKeyTimeToLiveMillis] longLongValue];
    _overflowPolicy = [dictionary[kExperimentPayloadKeyOverflowPolicy] intValue];

    NSMutableArray<ABTExperimentLite *> *ongoingExperiments = [[NSMutableArray alloc] init];

    NSArray<NSDictionary<NSString *, NSString *> *> *ongoingExperimentsArray =
        dictionary[kExperimentPayloadKeyOngoingExperiments];

    for (NSDictionary<NSString *, NSString *> *experimentDictionary in ongoingExperimentsArray) {
      NSString *experimentId = experimentDictionary[kExperimentPayloadKeyExperimentID];
      if (experimentId) {
        ABTExperimentLite *liteExperiment =
            [[ABTExperimentLite alloc] initWithExperimentId:experimentId];
        [ongoingExperiments addObject:liteExperiment];
      }
    }

    _ongoingExperiments = [ongoingExperiments copy];
  }
  return self;
}

- (void)clearTriggerEvent {
  _triggerEvent = nil;
}

- (BOOL)overflowPolicyIsValid {
  return self.overflowPolicy == ABTExperimentPayloadExperimentOverflowPolicyIgnoreNewest ||
         self.overflowPolicy == ABTExperimentPayloadExperimentOverflowPolicyDiscardOldest;
}

- (void)setOverflowPolicy:(ABTExperimentPayloadExperimentOverflowPolicy)overflowPolicy {
  _overflowPolicy = overflowPolicy;
}

@end
