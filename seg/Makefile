CXX=g++
CXXFLAGS=-std=c++11 -Wall -Wextra -g
LDFLAGS=

.PHONY: all clean

all: xorshift mwc

# --- xorshift ---
XORSHIFT_SRCS=xorshift.cpp bigint.cpp
XORSHIFT_OBJS=$(XORSHIFT_SRCS:.cpp=.o)

xorshift: $(XORSHIFT_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(XORSHIFT_OBJS) $(LDFLAGS)

# --- mwc ---
MWC_SRCS=mwc.cpp bigint.cpp
MWC_OBJS=$(MWC_SRCS:.cpp=.o)

mwc: $(MWC_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(MWC_OBJS) $(LDFLAGS)

# --- common rules ---
%.o: %.cpp bigint.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

bigint.o: bigint.cpp bigint.h

xorshift.o: xorshift.cpp bigint.h

mwc.o: mwc.cpp bigint.h

clean:
	rm -f xorshift mwc $(XORSHIFT_OBJS) $(MWC_OBJS)

