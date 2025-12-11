# Makefile
# Justin Paik (jpaik03)
# 11 December, 2025
# Divide & Conquer Convex Hull Algorithm
# 
# Includes build rules for dnc_ch using CGAL.
#

CXX      = g++
CXXFLAGS = -g3 -Wall -Wextra -Werror -Wfatal-errors -std=c++17

LEDA_PATH = /usr/cots/leda-6.1

# Includes directories, use -isystem suppresses warnings from LEDA headers
INCLUDES = -I$(LEDA_PATH)/incl

# Library directories
LDFLAGS  = -L$(LEDA_PATH)

# Required dependencies
# -lleda: The LEDA library
# -lX11:  Required for the graphics window
# -lm:    Standard math library
LIBS     = -lleda -lX11 -lm

dnc_ch: project.o dnc.o
	$(CXX) $(CXXFLAGS) -o dnc_ch project.o dnc.o en47_vis.o $(LDFLAGS) $(LIBS)
	
project.o: project.cpp en47_vis_txt.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c project.cpp

dnc.o: dnc.cpp dnc.h en47_vis_txt.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c dnc.cpp

clean: 
	rm -f dnc_ch project.o
