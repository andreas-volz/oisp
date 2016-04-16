
bin_PROGRAMS = \
	OISPControl_simu

OISPControl_simu_SOURCES = \
	main.cpp \
	../common/FileNotFoundException.cpp \
	../common/FileNotFoundException.h \
	searchFile.cpp \
	searchFile.h \
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
        $(OICFPROVIDER_LIBS) \
        $(LOG4CXX_LIBS)

OISPControl_simu_CXXFLAGS = \
        $(PROFILING_CFLAGS) \
        $(DBUSMM_CFLAGS) \
        $(DBUSMMECORE_CFLAGS) \
        $(SIGC_CFLAGS) \
        $(EFL_CFLAGS) \
        $(OICFPROVIDER_CFLAGS) \
        $(LOG4CXX_CFLAGS)


## File created by the gnome-build tools

