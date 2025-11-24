# Makefile
# Justin Paik (jpaik03)
# 11 December, 2025
# Divide & Conquer Convex Hull Algorithm
# 
# Includes build rules for dnc_ch using CGAL.
#

CXX      = clang++
CXXFLAGS = -g3 -Wall -Wextra -Werror -Wfatal-errors -std=c++17

# DEPENDENCY PATHS (/usr/sup)
CGAL_PATH  = /usr/sup/cgal-5.3.1
GMP_PATH   = /usr/sup/gmp-6.1.0
MPFR_PATH  = /usr/sup/mpfr-3.0.0
BOOST_PATH = /usr/sup/boost-1.77.0

# -isystem to suppress warnings from external library headers
INCLUDES = -isystem $(CGAL_PATH)/include \
           -isystem $(GMP_PATH)/include \
           -isystem $(MPFR_PATH)/include \
           -isystem $(BOOST_PATH)/include

# Library directories
LDFLAGS  = -L$(GMP_PATH)/lib -L$(GMP_PATH)/lib64 \
           -L$(MPFR_PATH)/lib -L$(MPFR_PATH)/lib64 \
           -L$(BOOST_PATH)/lib -L$(BOOST_PATH)/lib64

# Required dependencies
LIBS     = -lgmp -lmpfr

dnc_ch: project.o
	$(CXX) $(CXXFLAGS) -o dnc_ch project.o $(LDFLAGS) $(LIBS)
	
project.o: project.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c project.cpp

clean: 
	rm -f dnc_ch *.o
