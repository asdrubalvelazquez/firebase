/*
 * Copyright 2019 Google
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

#import <Foundation/Foundation.h>

#import <GoogleDataTransport/GDTClock.h>
#import <GoogleDataTransport/GDTStoredEvent.h>

/** Generates fake stored events. Beware, this is not threadsafe and methods shouldn't be called
 * concurrently.
 */
@interface GDTCCTEventGenerator : NSObject

/** All events generated by this instance. */
@property(nonatomic, readonly) NSMutableSet<GDTStoredEvent *> *allGeneratedEvents;

/** Deletes the empty files created in a temporary directory. */
- (void)deleteGeneratedFilesFromDisk;

/** Generates a GDTStoredEvent, complete with a file specified in the eventFileURL property.
 *
 * @param qosTier The QoS tier the event should have.
 * @return A newly allocated fake stored event.
 */
- (GDTStoredEvent *)generateStoredEvent:(GDTEventQoS)qosTier;

/** Generates a GDTStoredEvent, complete with a file specified in the eventFileURL property.
 *
 * @param qosTier The QoS tier the event should have.
 * @param fileURL The file URL containing bytes of some event.
 * @return A newly allocated fake stored event.
 */
- (GDTStoredEvent *)generateStoredEvent:(GDTEventQoS)qosTier fileURL:(NSURL *)fileURL;

/** Generates five consistent stored events.
 *
 * @return An array of five newly allocated but consistent GDTStoredEvents.
 */
- (NSArray<GDTStoredEvent *> *)generateTheFiveConsistentStoredEvents;

@end
