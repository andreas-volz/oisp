
bin_PROGRAMS = \
	OISPControl

OISPControl_SOURCES = \
	Joystick.cpp \
	Joystick.h \
	main.cpp \
	OICFControlListenerProviderImpl.cpp \
	OICFControlListenerProviderImpl.h 

OISPControl_LDADD = \
        $(PROFILING_LIBS) \
        $(DBUSMM_LIBS) \
        $(SIGC_LIBS) \
        $(GLIBMM_LIBS) \
        $(UDEV_LIBS) \
        $(OICFPROVIDER_LIBS) \
        $(LOG4CXX_LIBS)

OISPControl_CXXFLAGS = \
        $(PROFILING_CFLAGS) \
        $(DBUSMM_CFLAGS) \
        $(SIGC_CFLAGS) \
        $(GLIBMM_CFLAGS) \
        $(UDEV_CFLAGS) \
        $(OICFPROVIDER_CFLAGS) \
        $(LOG4CXX_CFLAGS)


## File created by the gnome-build tools

