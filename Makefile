CXXFLAGS=-std=c++11 -Wall -Wextra -pedantic -Wno-unused-argument -Wno-unused-variable
include mk/$(shell uname -s).mk

SRCDIR=.
OBJDIR=.
SRCS=\
	raytracing.cpp \
	mesh.cpp \
	main.cpp
OBJS=$(foreach S,$(SRCS:.cpp=.o),$(OBJDIR)/$(S))
BINARY=$(OBJDIR)/50_rays_of_tracing


.PHONY: all clean
all: $(BINARY)

clean:
	rm -f $(OBJS)
	rm -f $(BINARY)

$(BINARY): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $(BINARY)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


# Dependencies.
raytracing.cpp: raytracing.h
main.cpp: raytracing.h mesh.h traqueboule.h imageWriter.h
mesh.cpp: mesh.h
