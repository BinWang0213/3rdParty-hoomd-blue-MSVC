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
	rotmat3<Scalar> A;
	REQUIRE(A.row0.x == Approx(1.0));
	REQUIRE(std::abs(A.row0.y) < tol_small);
	REQUIRE(std::abs(A.row0.z) < tol_small);
	REQUIRE(std::abs(A.row1.x) < tol_small);
	REQUIRE(A.row1.y == Approx(1.0));
	REQUIRE(std::abs(A.row1.z) < tol_small);
	REQUIRE(std::abs(A.row2.x) < tol_small);
	REQUIRE(std::abs(A.row2.y) < tol_small);
	REQUIRE(A.row2.z == Approx(1.0));

	rotmat3<Scalar> B(vec3<Scalar>(1, 2, 3), vec3<Scalar>(4, 5, 6), vec3<Scalar>(7, 8, 9));
	REQUIRE(B.row0.x == Approx(1.0));
	REQUIRE(B.row0.y == Approx(2.0));
	REQUIRE(B.row0.z == Approx(3.0));
	REQUIRE(B.row1.x == Approx(4.0));
	REQUIRE(B.row1.y == Approx(5.0));
	REQUIRE(B.row1.z == Approx(6.0));
	REQUIRE(B.row2.x == Approx(7.0));
	REQUIRE(B.row2.y == Approx(8.0));
	REQUIRE(B.row2.z == Approx(9.0));

	Scalar pi = M_PI;
	Scalar alpha = pi / 2.0; // angle of rotation
	quat<Scalar> q1(cos(alpha / 2.0), (Scalar)sin(alpha / 2.0) * vec3<Scalar>(0, 0, 1)); // rotation quaternions
	rotmat3<Scalar> C(q1);
	REQUIRE(std::abs(C.row0.x) < tol_small);
	REQUIRE(C.row0.y == Approx(-1.0));
	REQUIRE(std::abs(C.row0.z) < tol_small);
	REQUIRE(C.row1.x == Approx(1.0));
	REQUIRE(std::abs(C.row1.y) < tol_small);
	REQUIRE(std::abs(C.row1.z) < tol_small);
	REQUIRE(std::abs(C.row2.x) < tol_small);
	REQUIRE(std::abs(C.row2.y) < tol_small);
	REQUIRE(C.row2.z == Approx(1.0));

	rotmat3<Scalar> D = rotmat3<Scalar>::fromAxisAngle(vec3<Scalar>(0, 0, 1), alpha);
	REQUIRE(std::abs(D.row0.x) < tol_small);
	REQUIRE(D.row0.y == Approx(-1.0));
	REQUIRE(std::abs(D.row0.z) < tol_small);
	REQUIRE(D.row1.x == Approx(1.0));
	REQUIRE(std::abs(D.row1.y) < tol_small);
	REQUIRE(std::abs(D.row1.z) < tol_small);
	REQUIRE(std::abs(D.row2.x) < tol_small);
	REQUIRE(std::abs(D.row2.y) < tol_small);
	REQUIRE(D.row2.z == Approx(1.0));
}

TEST_CASE("rotation_1")
{
	// test rotating a vec3
	Scalar pi = M_PI;
	Scalar alpha = pi / 2.0; // angle of rotation
	vec3<Scalar> v(1, 1, 1); // test vector
	quat<Scalar> q1(cos(alpha / 2.0), (Scalar)sin(alpha / 2.0) * vec3<Scalar>(0, 0, 1)); // rotation quaternions
	vec3<Scalar> a;

	rotmat3<Scalar> A(q1);

	a = A * v;
	REQUIRE(a.x == Approx(-1.0));
	REQUIRE(a.y == Approx(1.0));
	REQUIRE(a.z == Approx(1.0));
}

TEST_CASE("rotation_2")
{
	// test rotating a vec3
	Scalar pi = M_PI;
	Scalar alpha = pi / 2.0; // angle of rotation
	vec3<Scalar> v(1, 1, 1); // test vector
	quat<Scalar> q1(cos(alpha / 2.0), (Scalar)sin(alpha / 2.0) * vec3<Scalar>(0, 1, 0)); // rotation quaternions
	vec3<Scalar> a;

	rotmat3<Scalar> A(q1);

	a = A * v;
	REQUIRE(a.x == Approx(1.0));
	REQUIRE(a.y == Approx(1.0));
	REQUIRE(a.z == Approx(-1.0));
}

TEST_CASE("rotation_3")
{
	// test rotating a vec3
	Scalar pi = M_PI;
	Scalar alpha = pi / 2.0; // angle of rotation
	vec3<Scalar> v(1, 1, 1); // test vector
	quat<Scalar> q1(cos(alpha / 2.0), (Scalar)sin(alpha / 2.0) * vec3<Scalar>(1, 0, 0)); // rotation quaternions
	vec3<Scalar> a;

	rotmat3<Scalar> A(q1);

	a = A * v;
	REQUIRE(a.x == Approx(1.0));
	REQUIRE(a.y == Approx(-1.0));
	REQUIRE(a.z == Approx(1.0));
}
