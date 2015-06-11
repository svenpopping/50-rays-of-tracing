INCDIR=include
SRCDIR=src
OBJDIR=obj
SRCS=\
	raytracing.cpp \
	mesh.cpp \
	main.cpp
OBJS=$(foreach S,$(SRCS:.cpp=.o),$(OBJDIR)/$(S))
BINARY=./50_rays_of_tracing

XVFBFLAGS=-screen 0 800x600x24
CPPFLAGS=-I. -I$(INCDIR)
CXXFLAGS=-std=c++11 -Wall -Wextra -pedantic -Wno-unused-argument -Wno-unused-variable
include mk/$(shell uname -s).mk


.PHONY: all onlytrace clean
all: $(BINARY)

onlytrace: CXXFLAGS+=-DFIFTYRAYS_ONLYTRACE
onlytrace: DISPLAY=:1
onlytrace: all
	Xvfb $(DISPLAY) $(XVFBFLAGS) & \
	DISPLAY=$(DISPLAY) $(BINARY); \
	kill $$!

clean:
	rm -f $(OBJS)
	rm -f $(BINARY)

$(BINARY): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $(BINARY)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


# Dependencies.
$(SRCDIR)/raytracing.cpp: $(INCDIR)/raytracing.h
$(SRCDIR)/main.cpp: $(INCDIR)/raytracing.h $(INCDIR)/mesh.h $(INCDIR)/traqueboule.h $(INCDIR)/imageWriter.h
$(SRCDIR)/mesh.cpp: $(INCDIR)/mesh.h
$(INCDIR)/raytracing.h: ./config.h
