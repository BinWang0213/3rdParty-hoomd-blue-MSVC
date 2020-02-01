// Copyright (c) 2009-2019 The Regents of the University of Michigan
// This file is part of the HOOMD-blue project, released under the BSD 3-Clause License.


// this include is necessary to get MPI included before anything else to support intel MPI
#include "ExecutionConfiguration.h"

#include <iostream>
#include <math.h>
#include <memory>

#include <hoomd/extern/pybind/include/pybind11/pybind11.h>

#include "VectorMath.h"


using namespace std;


/*! \file test_messenger.cc
	\brief Unit test for Messenger
	\ingroup unit_tests
*/


#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

//! Tolerance setting for near-zero comparisons
const Scalar tol_small = Scalar(1e-3);

//! Tolerance setting for comparisons
const Scalar tol = Scalar(1e-2);

//! Loose tolerance to be used with randomly generated and unpredictable comparisons
Scalar loose_tol = Scalar(10);


TEST_CASE("construction")
{
	// test each constructor separately
	vec2<Scalar> a;
	REQUIRE(std::abs(a.x) < tol_small);
	REQUIRE(std::abs(a.y) < tol_small);

	vec2<Scalar> b(123, 86);
	REQUIRE(b.x == Approx(123));
	REQUIRE(b.y == Approx(86));
}

TEST_CASE("component_wise")
{
	vec2<Scalar> a(1, 2);
	vec2<Scalar> b(4, 6);
	vec2<Scalar> c;

	// test each component-wise operator separately
	c = a + b;
	REQUIRE(c.x == Approx(5));
	REQUIRE(c.y == Approx(8));

	c = a - b;
	REQUIRE(c.x == Approx(-3));
	REQUIRE(c.y == Approx(-4));

	c = a * b;
	REQUIRE(c.x == Approx(4));
	REQUIRE(c.y == Approx(12));

	c = a / b;
	REQUIRE(c.x == Approx(1.0 / 4.0));
	REQUIRE(c.y == Approx(2.0 / 6.0));

	c = -a;
	REQUIRE(c.x == Approx(-1));
	REQUIRE(c.y == Approx(-2));
}

TEST_CASE("assignment_component_wise")
{
	vec2<Scalar> a(1, 2);
	vec2<Scalar> b(4, 6);
	vec2<Scalar> c;

	// test each component-wise operator separately
	c = a += b;
	REQUIRE(c.x == Approx(5));
	REQUIRE(c.y == Approx(8));
	REQUIRE(a.x == Approx(5));
	REQUIRE(a.y == Approx(8));

	a = vec2<Scalar>(1, 2);
	c = a -= b;
	REQUIRE(c.x == Approx(-3));
	REQUIRE(c.y == Approx(-4));
	REQUIRE(a.x == Approx(-3));
	REQUIRE(a.y == Approx(-4));

	a = vec2<Scalar>(1, 2);
	c = a *= b;
	REQUIRE(c.x == Approx(4));
	REQUIRE(c.y == Approx(12));
	REQUIRE(a.x == Approx(4));
	REQUIRE(a.y == Approx(12));

	a = vec2<Scalar>(1, 2);
	c = a /= b;
	REQUIRE(c.x == Approx(1.0 / 4.0));
	REQUIRE(c.y == Approx(2.0 / 6.0));
	REQUIRE(a.x == Approx(1.0 / 4.0));
	REQUIRE(a.y == Approx(2.0 / 6.0));
}

TEST_CASE("scalar")
{
	vec2<Scalar> a(1, 2);
	Scalar b(4);
	vec2<Scalar> c;

	// test each component-wise operator separately
	c = a * b;
	REQUIRE(c.x == Approx(4));
	REQUIRE(c.y == Approx(8));

	c = b * a;
	REQUIRE(c.x == Approx(4));
	REQUIRE(c.y == Approx(8));

	c = a / b;
	REQUIRE(c.x == Approx(1.0 / 4.0));
	REQUIRE(c.y == Approx(2.0 / 4.0));
}

TEST_CASE("assignment_scalar")
{
	vec2<Scalar> a(1, 2);
	Scalar b(4);

	// test each component-wise operator separately
	a = vec2<Scalar>(1, 2);
	a *= b;
	REQUIRE(a.x == Approx(4));
	REQUIRE(a.y == Approx(8));

	a = vec2<Scalar>(1, 2);
	a /= b;
	REQUIRE(a.x == Approx(1.0 / 4.0));
	REQUIRE(a.y == Approx(2.0 / 4.0));
}

TEST_CASE("vector_ops")
{
	vec2<Scalar> a(1, 2);
	vec2<Scalar> b(6, 5);
	vec2<Scalar> c;
	Scalar d;

	// test each vector operation
	d = dot(a, b);
	REQUIRE(d == Approx(16));

	c = perp(a);
	REQUIRE(std::abs(dot(a, c)) < tol_small);
}

TEST_CASE("comparison")
{
	vec2<Scalar> a(1.1, 2.1);
	vec2<Scalar> b = a;
	vec2<Scalar> c(2.1, 1.1);

	// test equality
	REQUIRE(a == b);
	REQUIRE(!(a == c));

	// test inequality
	REQUIRE(!(a != b));
	REQUIRE(a != c);
}

TEST_CASE("projection")
{
	vec2<Scalar> a(3.4, 5.5);
	vec2<Scalar> b(0.1, 0);
	vec2<Scalar> c;

	// test projection
	c = project(a, b);
	REQUIRE(c.x == Approx(3.4));
	REQUIRE(std::abs(c.y) < tol_small);
}

TEST_CASE("perpdot_product")
{
	vec2<Scalar> a(3.4, 5.5);
	vec2<Scalar> b(0.1, -4.2);

	// test projection
	Scalar c = perpdot(a, b);
	REQUIRE(c == Approx(-14.83));
}

TEST_CASE("test_swap")
{
	vec2<Scalar> a(1.1, 2.2);
	vec2<Scalar> b(3.3, 4.4);
	vec2<Scalar> c(1.1, 2.2);
	vec2<Scalar> d(3.3, 4.4);

	// test swap
	a.swap(b);
	REQUIRE(a == d);
	REQUIRE(b == c);
}

TEST_CASE("test_assignment")
{
	// Test assignment
	vec2<float> g;
	vec2<double> h;
	g = vec2<float>(121, 12);
	REQUIRE(g.x == Approx(121));
	REQUIRE(g.y == Approx(12));
	g = vec2<double>(-122, 15);
	REQUIRE(g.x == Approx(-122));
	REQUIRE(g.y == Approx(15));
	h = vec2<float>(18, 12);
	REQUIRE(h.x == Approx(18));
	REQUIRE(h.y == Approx(12));
	h = vec2<double>(55, -64);
	REQUIRE(h.x == Approx(55));
	REQUIRE(h.y == Approx(-64));
}
