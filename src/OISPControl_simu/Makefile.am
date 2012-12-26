
bin_PROGRAMS = \
	OISPControl_simu

OISPControl_simu_SOURCES = \
	main.cpp \
	searchFile.cpp \
	searchFile.h \
	FileNotFoundException.cpp \
	FileNotFoundException.h \
	OICFControlListenerProviderImpl.cpp \
	OICFControlListenerProviderImpl.h \
	Simulation.cpp \
	Simulation.h 

OISPControl_simu_LDADD = \
        $(PROFILING_LIBS) \
        $(DBUSMM_LIBS) \
        $(DBUSMMECORE_LIBS) \
        $(SIGC_LIBS) \
        $(EFL_LIBS) \
        $(OICFPROVIDER_LIBS)

OISPControl_simu_CXXFLAGS = \
        $(PROFILING_CFLAGS) \
        $(DBUSMM_CFLAGS) \
        $(DBUSMMECORE_CFLAGS) \
        $(SIGC_CFLAGS) \
        $(EFL_CFLAGS) \
        $(OICFPROVIDER_CFLAGS)


## File created by the gnome-build tools

