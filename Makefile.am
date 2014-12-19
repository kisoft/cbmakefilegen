INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include

libdir = $(pkgdatadir)/plugins
pkgdata_DATA = cbMakefileGen.zip

lib_LTLIBRARIES = libcbMakefileGen.la

libcbMakefileGen_la_LDFLAGS = -module -version-info 0:2:0 -shared -no-undefined -avoid-version

libcbMakefileGen_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la

libcbMakefileGen_la_SOURCES = \
 cbmakefilecfg.cpp \
 cbMakefileGen.cpp \
 Makefile.cpp \
 Rule.cpp \
 Variable.cpp

noinst_HEADERS = \
 cbmakefilecfg.h \
 cbMakefileGen.h \
 Makefile.hpp \
 Rule.hpp \
 Variable.hpp

EXTRA_DIST = cbmakegen.cbp cbmakegen-unix.cbp manifest.xml wxsmith/cbmakefilecfg.wxs

CLEANFILES = $(pkgdata_DATA)

cbMakefileGen.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/cbMakefileGen.zip manifest.xml > /dev/null
