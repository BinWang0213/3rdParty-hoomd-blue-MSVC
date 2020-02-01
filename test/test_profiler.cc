// Copyright (c) 2009-2019 The Regents of the University of Michigan
// This file is part of the HOOMD-blue project, released under the BSD 3-Clause License.


// this include is necessary to get MPI included before anything else to support intel MPI
#include "ExecutionConfiguration.h"

#include <iostream>

#include <math.h>
#include "ClockSource.h"
#include "Profiler.h"

#include "VectorMath.h"


using namespace std;



#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

//! perform some simple checks on the profiler code
TEST_CASE("Profiler_test")
{
	// ProfileDataElem tests
	// constructor test
	ProfileDataElem p;
	REQUIRE(p.getChildElapsedTime() == 0);
	REQUIRE(p.getTotalFlopCount() == 0);
	REQUIRE(p.getTotalMemByteCount() == 0);

	// build up a tree and test its getTotal members
	p.m_elapsed_time = 1;
	p.m_flop_count = 2;
	p.m_mem_byte_count = 3;
	REQUIRE(p.getChildElapsedTime() == 0);
	REQUIRE(p.getTotalFlopCount() == 2);
	REQUIRE(p.getTotalMemByteCount() == 3);

	p.m_children["A"].m_elapsed_time = 4;
	p.m_children["A"].m_flop_count = 5;
	p.m_children["A"].m_mem_byte_count = 6;
	REQUIRE(p.getChildElapsedTime() == 4);
	REQUIRE(p.getTotalFlopCount() == 7);
	REQUIRE(p.getTotalMemByteCount() == 9);

	p.m_children["B"].m_elapsed_time = 7;
	p.m_children["B"].m_flop_count = 8;
	p.m_children["B"].m_mem_byte_count = 9;
	REQUIRE(p.getChildElapsedTime() == 4 + 7);
	REQUIRE(p.getTotalFlopCount() == 7 + 8);
	REQUIRE(p.getTotalMemByteCount() == 9 + 9);

	p.m_children["A"].m_children["C"].m_elapsed_time = 10;
	p.m_children["A"].m_children["C"].m_flop_count = 11;
	p.m_children["A"].m_children["C"].m_mem_byte_count = 12;
	REQUIRE(p.getChildElapsedTime() == 4 + 7);
	REQUIRE(p.getTotalFlopCount() == 7 + 8 + 11);
	REQUIRE(p.getTotalMemByteCount() == 9 + 9 + 12);

	Profiler prof("Main");
	prof.push("Loading");
	Sleep(500);
	prof.pop();
	prof.push("Neighbor");
	Sleep(1000);
	prof.pop(int64_t(1e6), int64_t(1e6));

	prof.push("Pair");
	prof.push("Load");
	Sleep(1000);
	prof.pop(int64_t(1e9), int64_t(1e9));
	prof.push("Work");
	Sleep(1000);
	prof.pop(int64_t(10e9), int64_t(100));
	prof.push("Unload");
	Sleep(1000);
	prof.pop(int64_t(100), int64_t(1e9));
	prof.pop();

	std::cout << prof;

	// This code attempts to reproduce the problem found in ticket #50
	Profiler prof2("test");
	prof2.push("test1");
	//Changing this slep value much lower than 100 results in the bug.
	Sleep(000);
	prof2.pop(100, 100);
	std::cout << prof2;

}
