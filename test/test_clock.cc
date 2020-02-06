// Copyright (c) 2009-2019 The Regents of the University of Michigan
// This file is part of the HOOMD-blue project, released under the BSD 3-Clause License.


// this include is necessary to get MPI included before anything else to support intel MPI

#include <iostream>

#include <math.h>
#include "hoomd/ClockSource.h"

#include "hoomd/VectorMath.h"


using namespace std;



#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

//! perform some simple checks on the profiler code
TEST_CASE("Profiler_test")
{
	printf("Sleep test\n");
	Sleep(500);
	printf("Sleep test\n");
	//Changing this slep value much lower than 100 results in the bug.
	Sleep(1000);
}
