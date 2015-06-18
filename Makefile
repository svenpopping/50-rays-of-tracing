INCDIR=include
SRCDIR=src
OBJDIR=obj
SRCS=\
	raytracing.cpp \
	mesh.cpp \
	helper.cpp \
	main.cpp
OBJS=$(foreach S,$(SRCS:.cpp=.o),$(OBJDIR)/$(S))
BINARY=./50_rays_of_tracing

XVFBFLAGS=-screen 0 800x800x24
CPPFLAGS=-I. -I$(INCDIR)
CXXFLAGS=-std=c++11 -Wall -Wextra -pedantic -Wno-unused-argument -Wno-unused-variable
include mk/$(shell uname -s).mk


.PHONY: all onlytrace clean cleanresults
all: $(BINARY)

onlytrace: CPPFLAGS+=-DFIFTYRAYS_ONLYTRACE
onlytrace: DISPLAY=:1
onlytrace: all
	Xvfb $(DISPLAY) $(XVFBFLAGS) & \
	sleep 2; \
	DISPLAY=$(DISPLAY) $(BINARY); \
	kill $$!

clean:
	rm -f $(OBJS)
	rm -f $(BINARY)

cleanresults:
	rm -f result*.{ppm,bmp}

$(BINARY): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(BINARY)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


# Dependencies.
$(SRCDIR)/raytracing.cpp: $(INCDIR)/raytracing.h
$(SRCDIR)/main.cpp: $(INCDIR)/raytracing.h $(INCDIR)/mesh.h $(INCDIR)/traqueboule.h $(INCDIR)/imageWriter.h
$(SRCDIR)/mesh.cpp: $(INCDIR)/mesh.h
$(INCDIR)/raytracing.h: config.h
