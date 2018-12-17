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

#import "GDLLogEvent.h"

#import "GDLLogClock.h"

NS_ASSUME_NONNULL_BEGIN

@interface GDLLogEvent ()

/** The serialized bytes of the log object. */
@property(nonatomic) NSData *extensionBytes;

/** The quality of service tier this log belongs to. */
@property(nonatomic) GDLLogQoS qosTier;

/** The clock snapshot at the time of logging. */
@property(nonatomic) GDLLogClockSnapshot clockSnapshot;

@end

NS_ASSUME_NONNULL_END
