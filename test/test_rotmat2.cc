// Copyright (c) 2009-2019 The Regents of the University of Michigan
// This file is part of the HOOMD-blue project, released under the BSD 3-Clause License.


// this include is necessary to get MPI included before anything else to support intel MPI
#include "ExecutionConfiguration.h"

#include <iostream>
#include <math.h>

#include "VectorMath.h"

using namespace std;



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
	rotmat2<Scalar> A;
	REQUIRE(A.row0.x == Approx(1.0));
	REQUIRE(std::abs(A.row0.y) < tol_small);
	REQUIRE(std::abs(A.row1.x) < tol_small);
	REQUIRE(A.row1.y == Approx(1.0));

	rotmat2<Scalar> B(vec2<Scalar>(1, 2), vec2<Scalar>(3, 4));
	REQUIRE(B.row0.x == Approx(1.0));
	REQUIRE(B.row0.y == Approx(2.0));
	REQUIRE(B.row1.x == Approx(3.0));
	REQUIRE(B.row1.y == Approx(4.0));

	Scalar pi = M_PI;
	Scalar alpha = pi / 2.0; // angle of rotation
	quat<Scalar> q1(cos(alpha / 2.0), (Scalar)sin(alpha / 2.0) * vec3<Scalar>(0, 0, 1)); // rotation quaternions
	rotmat2<Scalar> C(q1);
	REQUIRE(std::abs(C.row0.x) < tol_small);
	REQUIRE(C.row0.y == Approx(-1.0));
	REQUIRE(C.row1.x == Approx(1.0));
	REQUIRE(std::abs(C.row1.y) < tol_small);


	rotmat2<Scalar> D = rotmat2<Scalar>::fromAngle(alpha);
	REQUIRE(std::abs(D.row0.x) < tol_small);
	REQUIRE(D.row0.y == Approx(-1.0));
	REQUIRE(D.row1.x == Approx(1.0));
	REQUIRE(std::abs(D.row1.y) < tol_small);
}

TEST_CASE("rotation_2")
{
	// test rotating a vec2
	Scalar pi = M_PI;
	Scalar alpha = pi / 2.0; // angle of rotation
	vec2<Scalar> v(1, 1); // test vector
	quat<Scalar> q1(cos(alpha / 2.0), (Scalar)sin(alpha / 2.0) * vec3<Scalar>(0, 0, 1)); // rotation quaternions
	vec2<Scalar> a;

	rotmat2<Scalar> A(q1);

	a = A * v;
	REQUIRE(a.x == Approx(-1.0));
	REQUIRE(a.y == Approx(1.0));
}


TEST_CASE("transpose_2")
{
	// test rotating a vec2
	Scalar pi = M_PI;
	Scalar alpha = pi / 2.0; // angle of rotation
	vec2<Scalar> v(0.1234, 1.76123); // test vector
	quat<Scalar> q1(cos(alpha / 2.0), (Scalar)sin(alpha / 2.0) * vec3<Scalar>(0, 0, 1)); // rotation quaternions
	vec2<Scalar> a, b;

	rotmat2<Scalar> A(q1), B;

	a = A * v;
	b = transpose(A)*a;
	REQUIRE(b.x == Approx(v.x));
	REQUIRE(b.y == Approx(v.y));
}
