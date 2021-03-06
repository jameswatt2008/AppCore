//
// APCDataArchiverAndUploader.h
// AppCore
//
// Copyright (c) 2015 Apple, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1.  Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
//
// 2.  Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation and/or
// other materials provided with the distribution.
//
// 3.  Neither the name of the copyright holder(s) nor the names of any contributors
// may be used to endorse or promote products derived from this software without
// specific prior written permission. No license is granted to the trademarks of
// the copyright holders even if such marks are included in this software.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#import <Foundation/Foundation.h>

/**
 Uploads .json and other files to Sage, .zipping and encrypting
 them first.
 
 To use this:  call one of its +upload methods.  It will queue
 your objects to be archived and uploaded, return to you immediately,
 and then begin processing those objects.
 
 Currently, this class only supports uploading a single
 dictionary.  It should be straightforward to add other features
 we know we need:

 - upload multiple dictionaries

 - upload precomputed files (tapping data, air-quality data, etc.)

 - encrypt the contents of a .zip file, as well as the
   entire .zip file (air-quality data)


 ----------
 Goals
 ----------

 This class is intended to upload all types of data to Sage.
 It blends features from several other classes in this project:
 the APCDataArchiver in AppCore, the APHAirQualityDataModel in
 Asthma, and some other features from other uploaders around the
 AppCore.  Hopefully, it can become a superclass of those other
 classes, or at least provide generic, streamlined tools those
 other classes can use.

 Let's add "upload..." methods to this class as needed.

 Eventually, this class will support any third-party server,
 not just Sage; and those upload methods will probably end up
 in DataSubstrate.


 ----------
 Behind the Scenes
 ----------

 (1) When you call +upload, we create an ArchiverAndUploader
 object.  It starts asynchronously processing the objects you
 passed it.  If all goes well, it shoves those items into a
 .zip file, encrypts the file, and ships that file to Sage by
 calling Sage's "Bridge SDK."  When Bridge gets back to us
 with a status report, the Archiver deletes up any temporary
 files and directories it made, and frees itself from RAM.
 
 (2) You can call +upload as often as you like.  Each call
 creates a separate ArchiverAndUploader.  All of these objects
 use the same, global operationQueue to do their work.
 Currently, that queue is set to allow only one operation
 at a time, which I think will aid debugging and CPU load.
 We can experiment with increasing the number of simultaneous
 operations it allows.
 
 (3) This .zip-and-ship process has about 8 steps.  Each step
 returns a Boolean saying whether it worked, and an NSError
 representing any problems.  All errors are custom-crafted
 for this class; it wraps any errors from any other classes
 into an ArchiverAndUploader error.  Each of those steps
 behaves the same way internally:  it has one or more tasks
 to do, each of which returns a Boolean and an error.
 In every case, we stop processing as fast as possible after
 getting an error, and all errors and successes end up falling
 into the same "clean up" method.


 ----------
 How'd we get here?
 ----------

 Here's how this class relates to other classes scattered
 around our application suite, and the reason I felt it would
 be useful to create a new class:
 
 (1)  The AppCore DataArchiver was designed to create .zip
      files of ResearchKit output.  It's been robustified and
      enhanced in various ways, but stayed true to that need.
      It did *not* address the need to *upload* data to Sage;
      it only .zips that data.
 
 (2)  This means that everyone that needs to upload anything
      does it in their own way.  They call the Archiver to
      .zip their stuff, and then ship it to Sage by copying and
      pasting one of a couple of different method calls.
 
 (3)  The Asthma DataModel, a much more recent piece, combined
      pieces of all of that.  It used some evolving generalized
      pieces in the AppCore Archiver, and incorporated a
      streamlined version of uploader logic from other parts
      of the app.  It's also bound up (completely appropriately)
      with the needs of the Asthma app.
 
 (4)  Same goes for the PassiveDataCollectors.
 
 This new class started its life as an uploader for one very
 specific type of data:  the output of the MedicationTracker.
 However, since we can now see the above evolution, I think
 it makes sense to have a general-purpose archiver-and-
 uploader -- continuing the trend started by the Asthma Model
 class, and generalizing it further.
 
 So while I'll try hard NOT to overengineer this, I'll
 also try to keep it general enough that we can find 
 naturally-occurring subclass and superclass relationships
 between this new piece and the other pieces above.
 
 Many thanks to the programmers who started and evolved all
 of the above classes!
 */
@interface APCDataArchiverAndUploader : NSObject


/** .zips and uploads the specified dictionary to Sage. */
+ (void) uploadOneDictionary: (NSDictionary *) dictionary;



// ---------------------------------------------------------
#pragma mark - Proposed Ideas (not yet implemented)
// ---------------------------------------------------------

/*
 Other ideas, based on other needs around the app.  Not yet implemented.
 (All this will eventually go into DataSubstrate.)
 */

/** This represents what -[BaseTaskViewController processTaskResult] does now. */
+ (void) uploadResearchKitTaskResult: (id /* ORKTaskResult* */) taskResult;

/** For air-quality data:  encrypt the individual files inside the .zip file, as well as encrypting the whole package. */
+ (void)         uploadOneDictionary: (NSDictionary *) dictionary
     encryptingContentsBeforeZipping: (BOOL) shouldEncryptContentsFirst;

/** Er...  maybe this would be better?  Maybe it calls the above? */
+ (void) uploadAirQualityData: (NSDictionary *) airQualityStuff;

/** Catchall for uploading piles of random stuff?  Tapping test, 6-minute-walk test, etc.? */
+ (void) uploadDictionaries: (NSArray *) dictionaries
          withGroupFilename: (NSString *) filename
    encryptingContentsFirst: (BOOL) shouldEncryptContentsFirst;

@end














