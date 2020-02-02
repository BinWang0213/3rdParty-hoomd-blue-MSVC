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
	quat<Scalar> a;
	REQUIRE(a.s == Approx(1.0));
	REQUIRE(std::abs(a.v.x) < tol_small);
	REQUIRE(std::abs(a.v.y) < tol_small);
	REQUIRE(std::abs(a.v.z) < tol_small);


	quat<Scalar> b(123, vec3<Scalar>(86, -103, 12));
	REQUIRE(b.s == Approx(123));
	REQUIRE(b.v.x == Approx(86));
	REQUIRE(b.v.y == Approx(-103));
	REQUIRE(b.v.z == Approx(12));


	// note this mapping is for hoomd compatibility
	Scalar4 s4 = make_scalar4(-10, 25, 92, 12);
	quat<Scalar> c(s4);
	REQUIRE(c.s == Approx(s4.x));
	REQUIRE(c.v.x == Approx(s4.y));
	REQUIRE(c.v.y == Approx(s4.z));
	REQUIRE(c.v.z == Approx(s4.w));

	Scalar pi = M_PI;
	Scalar alpha = pi / 2.0; // angle of rotation
	quat<Scalar> d = quat<Scalar>::fromAxisAngle(vec3<Scalar>(0, 0, 1), alpha);
	quat<Scalar> q1(cos(alpha / 2.0), (Scalar)sin(alpha / 2.0) * vec3<Scalar>(0, 0, 1)); // rotation quaternions
	REQUIRE(d.s == Approx(q1.s));
	REQUIRE(d.v.x == Approx(q1.v.x));
	REQUIRE(d.v.y == Approx(q1.v.y));
	REQUIRE(d.v.z == Approx(q1.v.z));

	quat<float> e(123, vec3<float>(86, -103, 12));
	REQUIRE(quat<float>(e).s == Approx(123));
	REQUIRE(quat<float>(e).v.x == Approx(86));
	REQUIRE(quat<float>(e).v.y == Approx(-103));
	REQUIRE(quat<float>(e).v.z == Approx(12));
	REQUIRE(quat<double>(e).s == Approx(123));
	REQUIRE(quat<double>(e).v.x == Approx(86));
	REQUIRE(quat<double>(e).v.y == Approx(-103));
	REQUIRE(quat<double>(e).v.z == Approx(12));

	quat<double> f(123, vec3<double>(86, -103, 12));
	REQUIRE(quat<float>(f).s == Approx(123));
	REQUIRE(quat<float>(f).v.x == Approx(86));
	REQUIRE(quat<float>(f).v.y == Approx(-103));
	REQUIRE(quat<float>(f).v.z == Approx(12));
	REQUIRE(quat<double>(f).s == Approx(123));
	REQUIRE(quat<double>(f).v.x == Approx(86));
	REQUIRE(quat<double>(f).v.y == Approx(-103));
	REQUIRE(quat<double>(f).v.z == Approx(12));

	// Test assignment
	quat<float> g;
	quat<double> h;
	g = quat<float>(123, vec3<float>(86, -103, 12));
	REQUIRE(g.s == Approx(123));
	REQUIRE(g.v.x == Approx(86));
	REQUIRE(g.v.y == Approx(-103));
	REQUIRE(g.v.z == Approx(12));
	g = quat<double>(123, vec3<double>(86, -103, 12));
	REQUIRE(g.s == Approx(123));
	REQUIRE(g.v.x == Approx(86));
	REQUIRE(g.v.y == Approx(-103));
	REQUIRE(g.v.z == Approx(12));
	h = quat<float>(123, vec3<float>(86, -103, 12));
	REQUIRE(h.s == Approx(123));
	REQUIRE(h.v.x == Approx(86));
	REQUIRE(h.v.y == Approx(-103));
	REQUIRE(h.v.z == Approx(12));
	h = quat<double>(123, vec3<double>(86, -103, 12));
	REQUIRE(h.s == Approx(123));
	REQUIRE(h.v.x == Approx(86));
	REQUIRE(h.v.y == Approx(-103));
	REQUIRE(h.v.z == Approx(12));
}

TEST_CASE("arithmetic")
{
	quat<Scalar> a(1, vec3<Scalar>(2, 3, 4));
	quat<Scalar> b(4, vec3<Scalar>(6, 8, 10));
	quat<Scalar> c;
	vec3<Scalar> v;
	Scalar s = 3;

	// test each operator separately
	c = a * s;
	REQUIRE(c.s == Approx(3));
	REQUIRE(c.v.x == Approx(6));
	REQUIRE(c.v.y == Approx(9));
	REQUIRE(c.v.z == Approx(12));

	c = s * a;
	REQUIRE(c.s == Approx(3));
	REQUIRE(c.v.x == Approx(6));
	REQUIRE(c.v.y == Approx(9));
	REQUIRE(c.v.z == Approx(12));

	c = a + b;
	REQUIRE(c.s == Approx(5));
	REQUIRE(c.v.x == Approx(8));
	REQUIRE(c.v.y == Approx(11));
	REQUIRE(c.v.z == Approx(14));

	c = a;
	c += b;
	REQUIRE(c.s == Approx(5));
	REQUIRE(c.v.x == Approx(8));
	REQUIRE(c.v.y == Approx(11));
	REQUIRE(c.v.z == Approx(14));

	c = a * b;
	REQUIRE(c.s == Approx(-72));
	REQUIRE(c.v.x == Approx(12));
	REQUIRE(c.v.y == Approx(24));
	REQUIRE(c.v.z == Approx(24));

	c = a * vec3<Scalar>(6, 8, 10);
	REQUIRE(c.s == Approx(-76));
	REQUIRE(c.v.x == Approx(4));
	REQUIRE(c.v.y == Approx(12));
	REQUIRE(c.v.z == Approx(8));

	c = vec3<Scalar>(6, 8, 10) * a;
	REQUIRE(c.s == Approx(-76));
	REQUIRE(c.v.x == Approx(8));
	REQUIRE(c.v.y == Approx(4));
	REQUIRE(c.v.z == Approx(12));

	s = norm2(a);
	REQUIRE(s == Approx(30));

	c = conj(a);
	REQUIRE(c.s == a.s);
	REQUIRE(c.v.x == -a.v.x);
	REQUIRE(c.v.y == -a.v.y);
	REQUIRE(c.v.z == -a.v.z);

	v = rotate(a, vec3<Scalar>(1, 1, 1));
	REQUIRE(v.x == Approx(6));
	REQUIRE(v.y == Approx(30));
	REQUIRE(v.z == Approx(42));
}

TEST_CASE("hoomd_compat")
{
	// test convenience function for conversion to hoomd quaternions
	quat<Scalar> q(1, vec3<Scalar>(2, 3, 4));
	Scalar4 hq;
	hq = quat_to_scalar4(q);
	REQUIRE(hq.x == Approx(1));
	REQUIRE(hq.y == Approx(2));
	REQUIRE(hq.z == Approx(3));
	REQUIRE(hq.w == Approx(4));
}

// test some quaternion identities for more sanity checking

TEST_CASE("conjugation_norm")
{
	quat<Scalar> p(1, vec3<Scalar>(2, 3, 4));
	quat<Scalar> a;
	Scalar s1, s2;

	// conjugation and the norm
	s1 = norm2(p);
	a = p * conj(p);
	s2 = sqrt(norm2(a));
	REQUIRE(s1 == Approx(s2));
}

TEST_CASE("multiplicative_norm")
{
	quat<Scalar> p(1, vec3<Scalar>(2, 3, 4));
	quat<Scalar> q(0.4, vec3<Scalar>(0.3, 0.2, 0.1));
	quat<Scalar> a;
	Scalar s1, s2;

	// multiplicative norm
	s1 = norm2(p*q);
	s2 = norm2(p) * norm2(q);
	REQUIRE(s1 == Approx(s2));
}

TEST_CASE("rotation")
{
	Scalar pi = M_PI;
	Scalar alpha = pi / 2.0; // angle of rotation
	vec3<Scalar> v(1, 1, 1); // test vector
	quat<Scalar> q1(cos(alpha / 2.0), (Scalar)sin(alpha / 2.0) * vec3<Scalar>(0, 0, 1)); // rotation quaternions
	quat<Scalar> q2(cos(alpha / 2.0), (Scalar)sin(alpha / 2.0) * vec3<Scalar>(1, 0, 0));
	quat<Scalar> q3;
	vec3<Scalar> a;

	a = (q1 * v * conj(q1)).v;
	REQUIRE(a.x == Approx(-1));
	REQUIRE(a.y == Approx(1));
	REQUIRE(a.z == Approx(1));


	a = rotate(q1, v);
	REQUIRE(a.x == Approx(-1));
	REQUIRE(a.y == Approx(1));
	REQUIRE(a.z == Approx(1));

	// test rotation composition
	a = (q2 * q1 * v * conj(q1) * conj(q2)).v;
	REQUIRE(a.x == Approx(-1));
	REQUIRE(a.y == Approx(-1));
	REQUIRE(a.z == Approx(1));

	q3 = q2 * q1;
	a = (q3 * v * conj(q3)).v;
	REQUIRE(a.x == Approx(-1));
	REQUIRE(a.y == Approx(-1));
	REQUIRE(a.z == Approx(1));

	a = rotate(q3, v);
	REQUIRE(a.x == Approx(-1));
	REQUIRE(a.y == Approx(-1));
	REQUIRE(a.z == Approx(1));

	// inverse rotation
	a = rotate(conj(q3), a);
	REQUIRE(a.x == Approx(v.x));
	REQUIRE(a.y == Approx(v.y));
	REQUIRE(a.z == Approx(v.z));
}

TEST_CASE("rotation_2")
{
	// test rotating a vec2
	Scalar pi = M_PI;
	Scalar alpha = pi / 2.0; // angle of rotation
	vec2<Scalar> v(1, 1); // test vector
	quat<Scalar> q1(cos(alpha / 2.0), (Scalar)sin(alpha / 2.0) * vec3<Scalar>(0, 0, 1)); // rotation quaternions
	vec2<Scalar> a;

	a = rotate(q1, v);
	REQUIRE(a.x == Approx(-1));
	REQUIRE(a.y == Approx(1));
}
