// Copyright (c) 2009-2019 The Regents of the University of Michigan
// This file is part of the HOOMD-blue project, released under the BSD 3-Clause License.


// this include is necessary to get MPI included before anything else to support intel MPI
#include "ExecutionConfiguration.h"

#include <iostream>

#include "Index1D.h"

using namespace std;



#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>


//! test case for 1x1 Index2D
TEST_CASE("Index2D_1")
{
	Index2D a(1);
	REQUIRE(a.getNumElements()== (unsigned int)1);
	REQUIRE(a(0, 0)== (unsigned int)0);
}

//! test case for 2x2 Index2D
TEST_CASE("Index2D_2")
{
	Index2D a(2);
	REQUIRE(a.getNumElements()== (unsigned int)4);
	REQUIRE(a(0, 0)== (unsigned int)0);
	REQUIRE(a(1, 0)== (unsigned int)1);
	REQUIRE(a(0, 1)== (unsigned int)2);
	REQUIRE(a(1, 1)== (unsigned int)3);
}

//! test case for 3x3 Index2D
TEST_CASE("Index2D_3")
{
	Index2D a(3);
	REQUIRE(a.getNumElements()== (unsigned int)9);
	REQUIRE(a(0, 0)== (unsigned int)0);
	REQUIRE(a(1, 0)== (unsigned int)1);
	REQUIRE(a(2, 0)== (unsigned int)2);
	REQUIRE(a(0, 1)== (unsigned int)3);
	REQUIRE(a(1, 1)== (unsigned int)4);
	REQUIRE(a(2, 1)== (unsigned int)5);
	REQUIRE(a(0, 2)== (unsigned int)6);
	REQUIRE(a(1, 2)== (unsigned int)7);
	REQUIRE(a(2, 2)== (unsigned int)8);
}

//! test case for 2x2x2 Index3D
TEST_CASE("Index3D_2")
{
	Index3D a(2);
	REQUIRE(a.getNumElements()== (unsigned int)8);
	REQUIRE(a(0, 0, 0)== (unsigned int)0);
	REQUIRE(a(1, 0, 0)== (unsigned int)1);
	REQUIRE(a(0, 1, 0)== (unsigned int)2);
	REQUIRE(a(1, 1, 0)== (unsigned int)3);
	REQUIRE(a(0, 0, 1)== (unsigned int)4);
	REQUIRE(a(1, 0, 1)== (unsigned int)5);
	REQUIRE(a(0, 1, 1)== (unsigned int)6);
	REQUIRE(a(1, 1, 1)== (unsigned int)7);
}

//! test case for 4x3x2 Index3D
TEST_CASE("Index3D_432")
{
	Index3D a(4, 3, 2);
	REQUIRE(a.getNumElements()== (unsigned int)24);
	REQUIRE(a(0, 0, 0)== (unsigned int)0);
	REQUIRE(a(1, 0, 0)== (unsigned int)1);
	REQUIRE(a(2, 0, 0)== (unsigned int)2);
	REQUIRE(a(3, 0, 0)== (unsigned int)3);
	REQUIRE(a(0, 1, 0)== (unsigned int)4);
	REQUIRE(a(1, 1, 0)== (unsigned int)5);
	REQUIRE(a(2, 1, 0)== (unsigned int)6);
	REQUIRE(a(3, 1, 0)== (unsigned int)7);
	REQUIRE(a(0, 2, 0)== (unsigned int)8);
	REQUIRE(a(1, 2, 0)== (unsigned int)9);
	REQUIRE(a(2, 2, 0)== (unsigned int)10);
	REQUIRE(a(3, 2, 0)== (unsigned int)11);

	REQUIRE(a(0, 0, 1)== (unsigned int)12);
	REQUIRE(a(1, 0, 1)== (unsigned int)13);
	REQUIRE(a(2, 0, 1)== (unsigned int)14);
	REQUIRE(a(3, 0, 1)== (unsigned int)15);
	REQUIRE(a(0, 1, 1)== (unsigned int)16);
	REQUIRE(a(1, 1, 1)== (unsigned int)17);
	REQUIRE(a(2, 1, 1)== (unsigned int)18);
	REQUIRE(a(3, 1, 1)== (unsigned int)19);
	REQUIRE(a(0, 2, 1)== (unsigned int)20);
	REQUIRE(a(1, 2, 1)== (unsigned int)21);
	REQUIRE(a(2, 2, 1)== (unsigned int)22);
	REQUIRE(a(3, 2, 1)== (unsigned int)23);
}

//! test case for 20x20 Index2D
TEST_CASE("Index2D_20")
{
	Index2D a(20);
	REQUIRE(a.getNumElements()== (unsigned int)20 * 20);

	for (unsigned int i = 0; i < 20; i++)
		for (unsigned int j = 0; j < 20; j++)
		{
			REQUIRE(a(i, j) == j * 20 + i);
		}
}

//! test case for 1x1 Index2DUpperTriangular
TEST_CASE("Index2DUpperTriangular_1")
{
	Index2DUpperTriangular a(1);
	REQUIRE(a.getNumElements()== (unsigned int)1);
	REQUIRE(a(0, 0)== (unsigned int)0);
}

//! test case for 2x2 Index2DUpperTriangular
TEST_CASE("Index2DUpperTriangular_2")
{
	Index2DUpperTriangular a(2);
	REQUIRE(a.getNumElements()== (unsigned int)3);
	REQUIRE(a(0, 0)== (unsigned int)0);
	REQUIRE(a(1, 0)== (unsigned int)1);
	REQUIRE(a(1, 1)== (unsigned int)2);
}

//! test case for 3x3 Index2DUpperTriangular
TEST_CASE("Index2DUpperTriangular_3")
{
	Index2DUpperTriangular a(3);
	REQUIRE(a.getNumElements()== (unsigned int)6);
	REQUIRE(a(0, 0)== (unsigned int)0);
	REQUIRE(a(1, 0)== (unsigned int)1);
	REQUIRE(a(2, 0)== (unsigned int)2);
	REQUIRE(a(1, 1)== (unsigned int)3);
	REQUIRE(a(2, 1)== (unsigned int)4);
	REQUIRE(a(2, 2)== (unsigned int)5);
}

//! test case for 20x20 Index2DUpperTriangular
TEST_CASE("Index2DUpperTriangular_20")
{
	Index2DUpperTriangular a(20);
	REQUIRE(a.getNumElements()== (unsigned int)20 * 21 / 2);

	unsigned int cur_idx = 0;
	for (unsigned int i = 0; i < 20; i++)
		for (unsigned int j = i; j < 20; j++)
		{
			REQUIRE(a(i, j) == cur_idx);
			REQUIRE(a(j, i) == cur_idx);
			cur_idx++;
		}
}
