// Copyright 2019 Google
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

// TODO: Remove this class after the uploading of reports via GoogleDataTransport is no longer an
// experiment

#import "Crashlytics/Crashlytics/Models/FIRCLSInternalReport.h"

#import "Crashlytics/Crashlytics/Components/FIRCLSAppMemory.h"
#import "Crashlytics/Crashlytics/Components/FIRCLSUserLogging.h"
#import "Crashlytics/Crashlytics/Handlers/FIRCLSException.h"
#import "Crashlytics/Crashlytics/Handlers/FIRCLSSignal.h"
#import "Crashlytics/Crashlytics/Helpers/FIRCLSFile.h"
#import "Crashlytics/Crashlytics/Helpers/FIRCLSLogger.h"
#import "Crashlytics/Crashlytics/Models/FIRCLSFileManager.h"
#import "Crashlytics/Crashlytics/Models/Record/FIRCLSReportAdapter.h"

NSString *const FIRCLSCustomFatalIndicatorFile = @"custom_fatal.clsrecord";
NSString *const FIRCLSReportBinaryImageFile = @"binary_images.clsrecord";
NSString *const FIRCLSReportExceptionFile = @"exception.clsrecord";
NSString *const FIRCLSReportCustomExceptionAFile = @"custom_exception_a.clsrecord";
NSString *const FIRCLSReportCustomExceptionBFile = @"custom_exception_b.clsrecord";
NSString *const FIRCLSReportSignalFile = @"signal.clsrecord";
NSString *const FIRCLSMetricKitFatalReportFile = @"metric_kit_fatal.clsrecord";
NSString *const FIRCLSMetricKitNonfatalReportFile = @"metric_kit_nonfatal.clsrecord";
#if CLS_MACH_EXCEPTION_SUPPORTED
NSString *const FIRCLSReportMachExceptionFile = @"mach_exception.clsrecord";
#endif
NSString *const FIRCLSReportMetadataFile = @"metadata.clsrecord";
NSString *const FIRCLSReportErrorAFile = @"errors_a.clsrecord";
NSString *const FIRCLSReportErrorBFile = @"errors_b.clsrecord";
NSString *const FIRCLSReportLogAFile = @"log_a.clsrecord";
NSString *const FIRCLSReportLogBFile = @"log_b.clsrecord";
NSString *const FIRCLSReportInternalIncrementalKVFile = @"internal_incremental_kv.clsrecord";
NSString *const FIRCLSReportInternalCompactedKVFile = @"internal_compacted_kv.clsrecord";
NSString *const FIRCLSReportUserIncrementalKVFile = @"user_incremental_kv.clsrecord";
NSString *const FIRCLSReportUserCompactedKVFile = @"user_compacted_kv.clsrecord";
NSString *const FIRCLSReportRolloutsFile = @"rollouts.clsrecord";

@interface FIRCLSInternalReport () {
  NSString *_identifier;
  NSString *_path;
  NSArray *_metadataSections;
}

@end

@implementation FIRCLSInternalReport

+ (instancetype)reportWithPath:(NSString *)path {
  return [[self alloc] initWithPath:path];
}

#pragma mark - Initialization
/**
 * Initializes a new report, i.e. one without metadata on the file system yet.
 */
- (instancetype)initWithPath:(NSString *)path executionIdentifier:(NSString *)identifier {
  self = [super init];
  if (!self) {
    return self;
  }

  if (!path || !identifier) {
    return nil;
  }

  [self setPath:path];

  _identifier = [identifier copy];

  [self _checkAndWriteOOMOfRequired];

  return self;
}

// Load the reports internal kv store
- (NSDictionary<NSString *, NSString *> *)_loadInternalBreadcrumbs {
  NSString *path = [self.path stringByAppendingPathComponent:FIRCLSReportInternalIncrementalKVFile];
  NSArray *sections = FIRCLSFileReadSections(path.UTF8String, true, ^NSObject *(id obj) {
    NSDictionary *dict = [obj objectForKey:@"kv"];
    NSString *key = FIRCLSFileHexDecodeString(((NSString *)dict[@"key"]).UTF8String);
    NSString *value = FIRCLSFileHexDecodeString(((NSString *)dict[@"value"]).UTF8String);
    return (key && value) ? @{key : value} : @{};
  });
  NSMutableDictionary *res = [NSMutableDictionary dictionary];
  for (NSDictionary *kv in sections) {
    [res addEntriesFromDictionary:kv];
  }
  return [res copy];
}

// An OOM is pretty simple.
// If the data in this report shows an OOM,
// then write an exception file to disk.
// That will be picked up by the normal reporting system.
//
// NOTE:
// I'd like to write a signal SIGKILL file, or some other
// kind of OOM file, but up to now I have not been able to have
// it show up in the Firebase crashlytics console. As a matter of fact,
// this one doesn't show up either. There must be something blocking
// it server side.
// see: https://github.com/firebase/firebase-ios-sdk/discussions/12897
- (void)_checkAndWriteOOMOfRequired {
  NSString *path = [self pathForContentFile:FIRCLSReportExceptionFile];
  if ([NSFileManager.defaultManager fileExistsAtPath:path]) {
    return;
  }

  // first check if we need to build one
  // we look for all internal breabcrumbs
  NSDictionary<NSString *, NSString *> *breadcrumbs = [self _loadInternalBreadcrumbs];
  FIRCLSAppMemory *memoryInfo = [[FIRCLSAppMemory alloc] initWithJSONObject:breadcrumbs];
  if (memoryInfo.isOutOfMemory) {
    FIRCLSInfoLog(@"Writing OOM record to %@", path);
    FIRCLSExceptionRecordOutOfMemoryTerminationAtPath(path.UTF8String);
  }
}

/**
 * Initializes a pre-existing report, i.e. one with metadata on the file system.
 */
- (instancetype)initWithPath:(NSString *)path {
  NSString *metadataPath = [path stringByAppendingPathComponent:FIRCLSReportMetadataFile];
  NSString *identifier = [[[[self.class readFIRCLSFileAtPath:metadataPath] objectAtIndex:0]
      objectForKey:@"identity"] objectForKey:@"session_id"];
  if (!identifier) {
    FIRCLSErrorLog(@"Unable to read identifier at path %@", path);
  }
  return [self initWithPath:path executionIdentifier:identifier];
}

#pragma mark - Path Helpers
- (NSString *)directoryName {
  return self.path.lastPathComponent;
}

- (NSString *)pathForContentFile:(NSString *)name {
  return [[self path] stringByAppendingPathComponent:name];
}

- (NSString *)metadataPath {
  return [[self path] stringByAppendingPathComponent:FIRCLSReportMetadataFile];
}

- (NSString *)binaryImagePath {
  return [self pathForContentFile:FIRCLSReportBinaryImageFile];
}

#pragma mark - Processing Methods
- (BOOL)hasAnyEvents {
  NSArray *reportFiles = @[
    FIRCLSReportExceptionFile, FIRCLSReportSignalFile, FIRCLSReportCustomExceptionAFile,
    FIRCLSReportCustomExceptionBFile, FIRCLSMetricKitFatalReportFile,
    FIRCLSMetricKitNonfatalReportFile,
#if CLS_MACH_EXCEPTION_SUPPORTED
    FIRCLSReportMachExceptionFile,
#endif
    FIRCLSReportErrorAFile, FIRCLSReportErrorBFile
  ];
  return [self checkExistenceOfAtLeastOneFileInArray:reportFiles];
}

// These are purposefully in order of precedence. If duplicate data exists
// in any crash file, the exception file's contents take precedence over the
// rest, for example
//
// Do not change the order of this.
//
+ (NSArray *)crashFileNames {
  static NSArray *files;
  static dispatch_once_t onceToken;
  dispatch_once(&onceToken, ^{
    files = @[
      FIRCLSReportExceptionFile,
#if CLS_MACH_EXCEPTION_SUPPORTED
      FIRCLSReportMachExceptionFile,
#endif
      FIRCLSReportSignalFile, FIRCLSMetricKitFatalReportFile, FIRCLSCustomFatalIndicatorFile
    ];
  });
  return files;
}

- (BOOL)isCrash {
  NSArray *crashFiles = [FIRCLSInternalReport crashFileNames];
  return [self checkExistenceOfAtLeastOneFileInArray:crashFiles];
}

- (BOOL)checkExistenceOfAtLeastOneFileInArray:(NSArray *)files {
  NSFileManager *manager = [NSFileManager defaultManager];

  for (NSString *fileName in files) {
    NSString *path = [self pathForContentFile:fileName];

    if ([manager fileExistsAtPath:path]) {
      return YES;
    }
  }

  return NO;
}

- (void)enumerateSymbolicatableFilesInContent:(void (^)(NSString *path))block {
  for (NSString *fileName in [FIRCLSInternalReport crashFileNames]) {
    NSString *path = [self pathForContentFile:fileName];

    block(path);
  }
}

#pragma mark - Metadata helpers
+ (NSArray *)readFIRCLSFileAtPath:(NSString *)path {
  NSArray *sections = FIRCLSFileReadSections([path fileSystemRepresentation], false, nil);

  if ([sections count] == 0) {
    return nil;
  }

  return sections;
}

- (NSArray *)metadataSections {
  if (!_metadataSections) {
    _metadataSections = [self.class readFIRCLSFileAtPath:self.metadataPath];
  }
  return _metadataSections;
}

- (NSString *)orgID {
  return
      [[[self.metadataSections objectAtIndex:0] objectForKey:@"identity"] objectForKey:@"org_id"];
}

- (NSDictionary *)customKeys {
  return nil;
}

- (NSString *)bundleVersion {
  return [[[self.metadataSections objectAtIndex:2] objectForKey:@"application"]
      objectForKey:@"build_version"];
}

- (NSString *)bundleShortVersionString {
  return [[[self.metadataSections objectAtIndex:2] objectForKey:@"application"]
      objectForKey:@"display_version"];
}

- (NSDate *)dateCreated {
  NSUInteger unixtime = [[[[self.metadataSections objectAtIndex:0] objectForKey:@"identity"]
      objectForKey:@"started_at"] unsignedIntegerValue];

  return [NSDate dateWithTimeIntervalSince1970:unixtime];
}

- (NSDate *)crashedOnDate {
  if (!self.isCrash) {
    return nil;
  }

#if CLS_MACH_EXCEPTION_SUPPORTED
  // try the mach exception first, because it is more common
  NSDate *date = [self timeFromCrashContentFile:FIRCLSReportMachExceptionFile
                                    sectionName:@"mach_exception"];
  if (date) {
    return date;
  }
#endif

  return [self timeFromCrashContentFile:FIRCLSReportSignalFile sectionName:@"signal"];
}

- (NSDate *)timeFromCrashContentFile:(NSString *)fileName sectionName:(NSString *)sectionName {
  // This works because both signal and mach exception files have the same structure to extract
  // the "time" component
  NSString *path = [self pathForContentFile:fileName];

  NSNumber *timeValue = [[[[self.class readFIRCLSFileAtPath:path] objectAtIndex:0]
      objectForKey:sectionName] objectForKey:@"time"];
  if (timeValue == nil) {
    return nil;
  }

  return [NSDate dateWithTimeIntervalSince1970:[timeValue unsignedIntegerValue]];
}

- (NSString *)OSVersion {
  return [[[self.metadataSections objectAtIndex:1] objectForKey:@"host"]
      objectForKey:@"os_display_version"];
}

- (NSString *)OSBuildVersion {
  return [[[self.metadataSections objectAtIndex:1] objectForKey:@"host"]
      objectForKey:@"os_build_version"];
}

@end
