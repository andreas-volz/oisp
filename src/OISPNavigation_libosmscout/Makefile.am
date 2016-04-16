
bin_PROGRAMS = \
	OISPNavigation_libosmscout

OISPNavigation_libosmscout_SOURCES = \
	main.cpp       \
	../common/util.h       \
	../common/optionparser.h       \
	../common/Logger.h \
	../common/FileNotFoundException.cpp       \
	../common/FileNotFoundException.h       \
	GPSConnection.cpp       \
	GPSConnection.h       \
	Navigation.cpp       \
	Navigation.h       \
	EcoreDispatcher.cpp       \
	EcoreDispatcher.h       \
	NavigationScreen.cpp       \
	NavigationScreen.h       \
	OICFNavigationProviderImpl.cpp       \
	OICFNavigationProviderImpl.h       \
	searchFile.cpp       \
	searchFile.h       \
	ScreenManager.cpp       \
	ScreenManager.h       \
	MapCanvas.cpp       \
	MapCanvas.h \
	MathUtil.h \
	Esmartxx_Cairo.cpp \
	Esmartxx_Cairo.h \
	esmart_cairo.c \
	Esmart_Cairo.h \
	Vector2.h       \
	Preferences.cpp       \
	Preferences.h  
	

OISPNavigation_libosmscout_LDADD =  \
	$(PROFILING_LIBS)  \
	$(DBUSMM_LIBS)  \
	$(DBUSMMECORE_LIBS)  \
	$(LIBOSMSCOUT_LIBS)  \
	$(GPS_LIBS)  \
	$(SIGC_LIBS)  \
	$(GLIBMM_LIBS)  \
	$(EFL_LIBS)  \
	$(OICFPROVIDER_LIBS) \
	$(LOG4CXX_LIBS)


OISPNavigation_libosmscout_CXXFLAGS =  \
	$(PROFILING_CFLAGS)  \
	$(DBUSMM_CFLAGS)  \
	$(DBUSMMECORE_CFLAGS)  \
	$(LIBOSMSCOUT_CFLAGS)  \
	$(GPS_CFLAGS)  \
	$(SIGC_CFLAGS)  \
	$(GLIBMM_CFLAGS)  \
	$(EFL_CFLAGS)  \
	$(PROFILING_CFLAGS)  \
	$(OICFPROVIDER_CFLAGS) \
	$(STOPCLOCK_CFLAGS) \
	$(LOG4CXX_CFLAGS)

OISPNavigation_libosmscout_CFLAGS = \
	$(EFL_CFLAGS) \
	$(LIBOSMSCOUT_CFLAGS)  

## File created by the gnome-build tools

