lib_LTLIBRARIES=librootfilterformat.la
librootfilterformat_la_LIBADD=@ROOT_LDFLAGS@ @LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@

librootfilterformat_la_CPPFLAGS=$(SPECTCLCCSW) @ROOT_CFLAGS@ @LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@ \
	-I@top_srcdir@/Core -I@top_srcdir@/Utility -I@top_srcdir@/factories

librootfilterformat_la_SOURCES	=	CRootFilterOutputStage.cpp	\
					CRootNtupleAccumulator.cpp	\
					CRootFilterOutputStageCreator.cpp \
					rootPackage.cpp

noinst_HEADERS=	CRootFilterOutputStage.h	\
		CRootFilterOutputStageCreator.h	\
		CRootNtupleAccumulator.h


#  The library will get installed in $prefix/lib, nothing we can do
#  about that, but we want it installed in $prefix/TclLibs:

install-exec-local: 
	$(mkinstalldirs) $(prefix)/TclLibs
	for f in .libs/lib*; do $(INSTALL_PROGRAM) $$f $(prefix)/TclLibs; done


docs: rootFilterFormat.pdf

# This build sometimes fails so let'snot do it  by default....
# And not install it either... until we work that out
# (maybe buliding html is better?)

rootFilterFormat.pdf: rootFilterFormat.xml
	@PDFDOCBOOK@      @srcdir@/rootFilterFormat.xml


EXTRA_DIST=rootFilterFormat.xml selectFilterFormat.tcl \
	selectiondialog.gif selectiondialog.eps


ACLOCAL_AMFLAGS=-I m4
