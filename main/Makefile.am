# Remove Tape directory:

SUBDIRS = Replace \
					Scripts \
					Utility \
					factories \
					mpi \
					XamineParsers \
					Display \
					Core \
					ddas	\
					RootInterface \
					SpecTcl \
					TreeParam \
					contrib \
					TestFiles \
					Gui \
					splash \
					filtsplit \
					calibratedparams \
					treegui \
					ccusb \
					vmusb \
					tkcon \
					compress \
					docs \
					scalerWrapper \
					restclient \
					mirrorclient \
					Python db \
					xaminerunner \
					rootxamine \
					const	\
					firstof \
					map	\
					radwareio \
					rootFilterFormat 

# Removed packaged from the build - mpiSpecTcl - the offline version maybe
# gets obsolete and I think that's what it was for.

ACLOCAL_AMFLAGS=-I m4

# Gri TestFiles need to be figured out still..

AUX_DIST = $(ac_aux_dir)/config.guess \
	$(ac_aux_dir)/config.sub \
	$(ac_aux_dir)/install-sh \
	$(ac_aux_dir)/ltmain.sh \
	$(ac_aux_dir)/missing

EXTRA_DIST = fixdos ReleaseNotesNSCL4.0.txt SpecTclRC.tcl \
	bootstrap $(MYGRIDIR) LICENSE  \
	build.cygwin Directories.include CompilationRules.include \
	libtcl \
	CHANGELOG m4 Porting.txt rest unifiedformat PyQtGUI

dist-hook:
	mkdir -p $(distdir)/config
	echo `pwd`
	for file in $(AUX_DIST) $(AUX_DIST_EXTRA); do \
		base=$$(basename $$file) ; \
	  cp $$file $(distdir)/$$base ; \
	done
	rm -rf $(distdir)/unifiedformat/build

install-exec-local:
	install  -d   -m 0775 $(prefix) $(prefix)/bin $(prefix)/include
	install  -m 0664 config.h $(prefix)/include
	install  -d   -m 0775 $(prefix)/lib 
	install  -d   -m 0775 $(prefix)/etc
	install -d   -m 0775 $(prefix)/Script 
	install -d   -m 0775 $(prefix)/Skel
	install  -d   -m 0775 $(prefix)/contrib $(prefix)/doc $(prefix)/share
	install -m 0664 config.h $(prefix)/include
	(cd @top_srcdir@/rest; ./installer @prefix@)
	rm -f $(prefix)/Bin
	rm -f $(prefix)/Lib
	rm -f $(prefix)/Include
	rm -f $(prefix)/Etc
	ln -s $(prefix)/bin $(prefix)/Bin
	ln -s $(prefix)/lib $(prefix)/Lib
	ln -s $(prefix)/include $(prefix)/Include
	ln -s $(prefix)/etc     $(prefix)/Etc
	echo  TCLINCLUDES=$(TCL_CPPFLAGS)          > Directories.temp
	echo  TCLLIBS    =@TK_LIBS@ @TCL_LIBS@ -lm -ldl  >> Directories.temp 
	echo  X11INCLUDES=$(X11INCLUDES)       >> Directories.temp
	echo  X11LIBS    =$(XLIBSW) $(MOTIF_LIBSW) -lXm -lXt -lX11 >> Directories.temp
	echo ROOTCFLAGS=@ROOT_CFLAGS@ >> Directories.temp
	echo ROOTLDFLAGS=@ROOT_LDFLAGS@ -Wl,-rpath=@ROOT_LIBRARY_DIR@ >> Directories.temp
	cat   @srcdir@/Directories.include              >> Directories.temp
	sed  -e 's/-lieee//g'  @builddir@/Directories.temp >Directories.edited
	$(INSTALL_DATA) @builddir@/Directories.edited $(sysconfdir)/Directories.include
	$(INSTALL_DATA) @srcdir@/LICENSE    $(prefix)
	echo CXX=$(CXX)                       > CompilationRules.temp
	echo CC=$(CC)                        >> CompilationRules.temp
	echo CXXLD=$(CXX)                  >> CompilationRules.temp
	echo CCLD=$(CC)                    >> CompilationRules.temp
	echo LIBTCLPLUS_CLFLAGS=@LIBTCLPLUS_CFLAGS@ >> CompilationRules.temp
	echo LIBTCLPLUS_LDFLAGS=@LIBTCLPLUS_LDFLAGS@ >> CompilationRules.temp
	cat  @srcdir@/CompilationRules.include        >> CompilationRules.temp
	$(INSTALL_DATA) @builddir@/CompilationRules.temp $(sysconfdir)/CompilationRules.include
	echo "Version: " SpecTcl-$(VERSION) build on `hostname` `date` by `whoami` >VERSION
	echo The CERN Root installation used  was: `@ROOTCONFIG@ --prefix` >> VERSION
	echo configured as shown below: >> VERSION
	head @top_builddir@/config.log >>VERSION
	echo libtcl `(cd @top_srcdir@/libtcl; @GIT@ status -uno) ` >> VERSION
	echo unifiedformat `(cd @top_srcdir@/libtcl; @GIT@ status -uno) ` >> VERSION
	echo CutiePie `(cd @top_srcdir@/PyQtGUI; @GIT@ status -uno) ` >> VERSION 
	$(INSTALL_DATA) @builddir@/VERSION $(prefix)/VERSION
	$(INSTALL_DATA) @srcdir@/CHANGELOG $(prefix)
	$(MANDB) $(prefix)/docs/man
	$(INSTALL_DATA) @srcdir@/Porting.txt  @prefix@

#check-TESTS:
#	- for d in $(SUBDIRS); do (cd $$d; make check-TESTS); done
