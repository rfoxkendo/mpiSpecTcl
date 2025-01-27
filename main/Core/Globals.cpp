/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  Globals.cpp
 *  @brief: Contains SpecTcl global variables.
 */
static const char* Copyright = "(C) Copyright Michigan State University 2008, All rights reserved";

// Globals.cpp:
//    This file defines the globals which are used by the NSCL histogramming
//    program.  All items in this file are available to clients in the
//    header file:  Globals.h
//
// Author:
//    Ron FOx
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    
// Change Log:
//     July 12, 1999 Ron FOx
//        Added gpVersion and set it to be 0.2
//   $Log$
//   Revision 5.3  2007/02/17 02:34:59  ron-fox
//   BZ292 - Synch the VERSION in config.h with gpVersion
//
//   Revision 5.2  2005/06/03 15:19:26  ron-fox
//   Part of breaking off /merging branch to start 3.1 development
//
//   Revision 5.1.2.2  2005/03/04 19:05:41  ron-fox
//   Set the version string to 3.0pre1
//
//   Revision 5.1.2.1  2004/12/15 17:24:05  ron-fox
//   - Port to gcc/g++ 3.x
//   - Recast swrite/sread in terms of tcl[io]stream rather than
//     the kludgy thing I had done of decoding the channel fd.
//     This is both necessary due to g++ 3.x's runtime and
//     nicer too!.
//
//   Revision 5.1  2004/11/29 16:56:10  ron-fox
//   Begin port to 3.x compilers calling this 3.0
//
//   Revision 4.5  2003/08/27 13:37:52  ron-fox
//   Create the event sink pipeline in the TclGrammerApp so that we have explicit
//   control over when it gets created and don't get any compiler dependencies on
//   when globals are constructed.
//


static const char* pCopyright=
  "Globals.cpp (c) Copyright 1999 NSCL, All rights reserved.";

#include <config.h>
#include <EventUnpacker.h>
#include <BufferDecoder.h>
#include <EventSinkPipeline.h>
#include <EventSink.h>
#include <TCLInterpreter.h>
#include <File.h>
#include <RunControl.h>
#include <Analyzer.h>
#include <histotypes.h>
#include <NSCLBufferDecoder.h>
#include <DisplayInterface.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif
#ifdef WITH_MPI
#include <mpi.h>
#endif


//  The following points to the current event unpacker.
//  This must be an object which is derived from the CEventUnpacker class.
//  The user, when tailoring this system, must create an event unpacker
//  named gUnpacker.
//    Our pointer by default points to that unpacker:
extern CEventUnpacker&  grUnpacker;
CEventUnpacker*         gpUnpacker = 0; //&grUnpacker;

//  The buffer decoder is responsible for knowing the global format of
//  event buffers.  This includes knowing about buffer headers, and event
//  types.  It is up to the formatter to, given a buffer, call the
//  Analyzer's appropriate callback virtual functions.
//  The default buffer decoder understands the NSCL buffer format.
//  If another buffer format is used, then the user must hook a formatter
//  for it into the gpBufferDecoder global.

// CNSCLBufferDecoder gNSCLDecoder;
CBufferDecoder*    gpBufferDecoder = 0; //gNSCLDecoder;

//  The event sink takes dumps of event lists and processes them.  The default
//  action is to process them through a CAnalyzer object.  The CAnalyzer object
//  is created dynamically and stored in this pointer in order to 
//  allow the size of the display memory to be gotten from the TCL .rc file.
//  If it is desired to modify the sink (e.g. to hook a different sink in,
//  the application initialization must be copied and initialized.  This 
//  initialization is in the file:  AppInit.cpp
CEventSinkPipeline* gpEventSinkPipeline = (CEventSinkPipeline*)kpNULL;
CEventSink* gpEventSink = (CEventSink*)kpNULL;

//   The entire system is 'command driven' using a CTCLInterpreterObject
//   to interface it to TCL.  It is useful to have such an object available
//   to allow user code to interface with it.  This pointer is also dynamically
//   set at an appropriate time in AppInit.cpp:
CTCLInterpreter* gpInterpreter = (CTCLInterpreter*)kpNULL;

//  The event source is set dynamically by appropriate TCL commands.
//  It is available for external manipulation here.
CFile* gpEventSource     = (CFile*)kpNULL;

//  The run control object is dynamically created and a pointer to it is
//  maintained here.  Setting this allows the analysis framework to run in 
//  non Tcl/TK environments.
CRunControl* gpRunControl = (CRunControl*)kpNULL;

//  The analyzer is responsible for taking a buffer and producing lists of
//  events which are shoved into the sink.
//  The analyzer is produced in the AppInit.cpp module.
CAnalyzer* gpAnalyzer   = (CAnalyzer*)kpNULL;

// SpecTcl also retains a version text string.
const char* gpVersion = VERSION;

//  The interface to the display. Provides simple mechanisms for adding/removing/updating
//  spectra, gates, fits on the display
CDisplayInterface* gpDisplayInterface = (CDisplayInterface*)kpNULL;

// Shared display memory pointer.

volatile void* gpDisplayMemory = kpNULL;    // Gets filled in by TclGrammerApp.

// This variable is true if SpecTcl was built for MPI and is run under MPI

bool          gMPIParallel(false);

// This variable holds the communicator used to send ring items from 
// the root process to the worker processes.
//
#ifdef WITH_MPI
MPI_Comm  gRingItemComm;
#endif

// This variable holds the communicator used to broadcast
// gates received from Xamine.  It's needed to
// keep broadcasts for commands distinct from gates.
//
#ifdef WITH_MPI
MPI_Comm                  gXamineGateComm;
#endif