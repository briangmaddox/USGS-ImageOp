
prefix		=	/home/bmaddox
exec_prefix	=	${prefix}
host_os		=	linux
srcdir		=	.
top_srcdir	=	.
gen_depend	=	no
enable_debug	=	no
enable_purify	=	no

# where to find our own headers
IMAGEOPHEADERS = -I$(prefix)

foundone := false
# DG/UX (Data General UNIX)
ifeq ($(findstring dgux,$(host_os)),dgux)
INCPATHS = $(IMAGEOPHEADERS) -I$(prefix)/include
LIBPATHS = -L$(prefix)/lib
LIBS = $(LIBPATHS) -lImageOp -lFrodo -lImage -lgeotiff -ltiff
foundone := true
endif

# Linux
ifeq ($(host_os),linux)
INCPATHS = $(IMAGEOPHEADERS) -I/usr/X11R6/include -I$(prefix)/include
LIBPATHS = -L/usr/X11R6/lib -L$(prefix)/lib
LIBS = $(LIBPATHS) -lImageOp -lFrodo -lImage -lgeotiff -ltiff
foundone := true
endif

ifeq ($(foundone),false)
INCPATHS = $(IMAGEOPHEADERS) -I$(prefix)/include
LIBPATHS = -L$(prefix)/lib
LIBS = $(LIBPATHS) -lImageOp -lFrodo -lImage -lgeotiff -ltiff
ORALIBS = 
endif


# Where to install ImageOp's include files.
INCDEST = $(prefix)/include/ImageOp

# Where to install ImageOp's library.
LIBDEST = $(prefix)/lib

ifeq ($(enable_debug),yes)
DEBUG = -g
else
DEBUG = -O2
endif

ifeq ($(enable_purify),yes)
PURIFY = purify
else
PURIFY =
endif

CC=$(PURIFY) gcc
CXX=$(PURIFY) g++
CXXFLAGS= $(DEBUG) $(INCPATHS)

INSTALLFILE = $(srcdir)/config/install-sh -c
INSTALLDIR  = $(srcdir)/config/install-sh -d

SRCS=    ImageOperator.cc		\
         UnaryImageOperator.cc		\
         BinaryImageOperator.cc		\
         CopyImage.cc			\
         FilterImage.cc			\
         ToPixelFilter.cc		\
         SubtractNeighborFilter.cc	\
         LaplacianFilter.cc		\
         BlurFilter.cc

OBJS=$(SRCS:.cc=.o)

all: libImageOp.a installlib installinc
	@echo "Done."

libImageOp.a: $(OBJS)
	ar ru libImageOp.a $(OBJS)

testCopy: testCopy.o
	$(CXX) $(CXXFLAGS) -o testCopy testCopy.o $(LIBS)

tiffCopy: tiffCopy.o
	$(CXX) $(CXXFLAGS) -o tiffCopy tiffCopy.o $(LIBS)

testFilter: testFilter.o
	$(CXX) $(CXXFLAGS) -o testFilter testFilter.o $(LIBS)

testSub: testSub.o
	$(CXX) $(CXXFLAGS) -o testSub testSub.o $(LIBS)

# Preserve the time stamp on the include files.
installinc: 
	@echo "Installing includes"
	@test -d $(INCDEST) || $(INSTALLDIR) $(INCDEST)
	@tar cf /tmp/ImageOp.tar *.h ; \
	cd $(INCDEST); tar xf /tmp/ImageOp.tar ; \
	rm -rf /tmp/ImageOp.tar

installlib:
	@echo "Installing library"
	@test -d $(LIBDEST) || $(INSTALLDIR) $(LIBDEST)
	@cp libImageOp.a $(LIBDEST)

clean::
	rm -f $(OBJS) *.a core 

realclean: 
	rm -rf *.d config.cache config.status config.log


ifeq ($(gen_depend),yes)
# Automatic generation of dependencies
%.d: %.cc
	@echo "Regenerating dependencies for $<";$(SHELL) -ec '$(CC) \
	$(INCPATHS) -MM $(CPPFLAGS) $< | sed '\''s/$*.o/& $@/g'\'' > $@'


include $(SRCS:.cc=.d)

endif


