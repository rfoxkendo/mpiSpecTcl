lib_LTLIBRARIES=libMPIUtilities.la
libMPIUtilities_la_CPPFLAGS=@LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@
libMPIUtilities_lla_LDFLAGS=@LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@ @LIBEXCEPTION_LDFLAGS@

libMPIUtilities_la_SOURCES= TclPump.cpp TclPump.h MPITclCommand.cpp MPITclCommandAll.cpp \
    MPITclPackagedCommand.cpp MPITclPackagedCommandAll.cpp

include_HEADERS = MPITclCommand.h MPITclCommandAll.h MPITclPackagedCommand.h \
    MPITclPackagedCommandAll.h

noinst_PROGRAMS= testApp

testApp_SOURCES= testApp.cpp TclPump.cpp MPITclCommand.cpp MPITclCommandAll.cpp
testApp_CPPFLAGS=$(libMPIUtilities_la_CPPFLAGS)
testApp_LDFLAGS= $(libMPIUtilities_lla_LDFLAGS)