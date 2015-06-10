CXXFLAGS=-std=c++11 -Wall -Wextra -pedantic -Wno-unused-argument
include mk/$(shell uname -s).mk

SRCDIR=.
SRCS=\
	raytracing.cpp \
	mesh.cpp \
	main.cpp
OBJDIR=.
OBJS=$(foreach S,$(SRCS:.cpp=.o),$(OBJDIR)/$(S))
BINARY=50_rays_of_tracing


all: $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $(BINARY)

clean:
	rm -f $(OBJS)
	rm -f $(BINARY)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
