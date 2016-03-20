
bin_PROGRAMS = \
	OISPMedia

OISPMedia_SOURCES = \
	main.cpp   \
	DirectoryList.cpp   \
	DirectoryList.h   \
	util.h   \
	OICFMediaProviderImpl.cpp   \
	OICFMediaProviderImpl.h   \
	Player.cpp   \
	Player.h \
	MathUtil.h

OISPMedia_LDADD = \
	$(PROFILING_LIBS) \
	$(GLIBMM_LIBS) \
	$(OICFPROVIDER_LIBS) \
	$(DBUSMM_LIBS) \
	$(DBUSMMECORE_LIBS) \
	$(EFL_LIBS) \
	$(BOOST_FILESYSTEM_LIB) \
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
	$(BOOST_CPPFLAGS) \
	$(EFL_CFLAGS) \
	$(OICFPROVIDER_CFLAGS) \
	$(GST_CFLAGS) \
	$(LOG4CXX_CFLAGS)

## File created by the gnome-build tools

