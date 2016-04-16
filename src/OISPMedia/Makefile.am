
bin_PROGRAMS = \
	OISPMedia

OISPMedia_SOURCES = \
	main.cpp   \
	DirectoryList.cpp   \
	DirectoryList.h   \
	OICFMediaProviderImpl.cpp   \
	OICFMediaProviderImpl.h   \
	../common/util.h   \
	../common/optionparser.h       \
	../common/Logger.h \
	../common/FileNotFoundException.cpp \
	../common/FileNotFoundException.h \
	Preferences.cpp \
	Preferences.h \
	Player.cpp   \
	Player.h \
	searchFile.cpp   \
	searchFile.h   \
	MathUtil.h

OISPMedia_LDADD = \
	$(PROFILING_LIBS) \
	$(GLIBMM_LIBS) \
	$(OICFPROVIDER_LIBS) \
	$(DBUSMM_LIBS) \
	$(DBUSMMECORE_LIBS) \
	$(EFL_LIBS) \
	$(OICFPROVIDER_LIBS) \
	$(GST_LIBS) \
	$(EFL_LIBS) \
	$(LOG4CXX_LIBS)

OISPMedia_CXXFLAGS = \
	$(PROFILING_CFLAGS) \
	$(GLIBMM_CFLAGS) \
	$(OICFPROVIDER_CFLAGS) \
	$(DBUSMM_CFLAGS) \
	$(DBUSMMECORE_CFLAGS) \
	$(EFL_CFLAGS) \
	$(OICFPROVIDER_CFLAGS) \
	$(GST_CFLAGS) \
	$(LOG4CXX_CFLAGS)

## File created by the gnome-build tools

