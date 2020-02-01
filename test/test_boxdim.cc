// Copyright (c) 2009-2019 The Regents of the University of Michigan
// This file is part of the HOOMD-blue project, released under the BSD 3-Clause License.


// this include is necessary to get MPI included before anything else to support intel MPI
#include "ExecutionConfiguration.h"

#include <iostream>

#include "BoxDim.h"



using namespace std;



#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

//! Perform some basic tests on the boxdim structure
TEST_CASE("BoxDim_basic_test")
{
	Scalar tol = Scalar(1e-6);

	// test default constructor
	BoxDim a;
	REQUIRE(a.getLo().x == Approx(0.0));
	REQUIRE(a.getLo().y == Approx(0.0));
	REQUIRE(a.getLo().z == Approx(0.0));
	REQUIRE(a.getHi().x == Approx(0.0));
	REQUIRE(a.getHi().y == Approx(0.0));
	REQUIRE(a.getHi().z == Approx(0.0));

	BoxDim b(10.0);
	REQUIRE(b.getLo().x == Approx(-5.0));
	REQUIRE(b.getLo().y == Approx(-5.0));
	REQUIRE(b.getLo().z == Approx(-5.0));
	REQUIRE(b.getHi().x == Approx(5.0));
	REQUIRE(b.getHi().y == Approx(5.0));
	REQUIRE(b.getHi().z == Approx(5.0));
	REQUIRE(b.getL().x == Approx(10.0));
	REQUIRE(b.getL().y == Approx(10.0));
	REQUIRE(b.getL().z == Approx(10.0));
	REQUIRE(b.getPeriodic().x == 1);
	REQUIRE(b.getPeriodic().y == 1);
	REQUIRE(b.getPeriodic().z == 1);

	BoxDim c(10.0, 30.0, 50.0);
	REQUIRE(c.getLo().x == Approx(-5.0));
	REQUIRE(c.getLo().y == Approx(-15.0));
	REQUIRE(c.getLo().z == Approx(-25.0));
	REQUIRE(c.getHi().x == Approx(5.0));
	REQUIRE(c.getHi().y == Approx(15.0));
	REQUIRE(c.getHi().z == Approx(25.0));
	REQUIRE(c.getL().x == Approx(10.0));
	REQUIRE(c.getL().y == Approx(30.0));
	REQUIRE(c.getL().z == Approx(50.0));
	REQUIRE(c.getPeriodic().x == 1);
	REQUIRE(c.getPeriodic().y == 1);
	REQUIRE(c.getPeriodic().z == 1);

	// test for assignment and copy constructor
	c.setPeriodic(make_uchar3(1, 0, 1));
	BoxDim d(c);
	REQUIRE(d.getLo().x == Approx(-5.0));
	REQUIRE(d.getLo().y == Approx(-15.0));
	REQUIRE(d.getLo().z == Approx(-25.0));
	REQUIRE(d.getHi().x == Approx(5.0));
	REQUIRE(d.getHi().y == Approx(15.0));
	REQUIRE(d.getHi().z == Approx(25.0));
	REQUIRE(d.getL().x == Approx(10.0));
	REQUIRE(d.getL().y == Approx(30.0));
	REQUIRE(d.getL().z == Approx(50.0));
	REQUIRE(d.getPeriodic().x == 1);
	REQUIRE(d.getPeriodic().y == 0);
	REQUIRE(d.getPeriodic().z == 1);

	BoxDim e;
	e = c;
	REQUIRE(e.getLo().x == Approx(-5.0));
	REQUIRE(e.getLo().y == Approx(-15.0));
	REQUIRE(e.getLo().z == Approx(-25.0));
	REQUIRE(e.getHi().x == Approx(5.0));
	REQUIRE(e.getHi().y == Approx(15.0));
	REQUIRE(e.getHi().z == Approx(25.0));
	REQUIRE(e.getL().x == Approx(10.0));
	REQUIRE(e.getL().y == Approx(30.0));
	REQUIRE(e.getL().z == Approx(50.0));
	REQUIRE(d.getPeriodic().x == 1);
	REQUIRE(d.getPeriodic().y == 0);
	REQUIRE(d.getPeriodic().z == 1);

	b = b;
	REQUIRE(b.getLo().x == Approx(-5.0));
	REQUIRE(b.getLo().y == Approx(-5.0));
	REQUIRE(b.getLo().z == Approx(-5.0));
	REQUIRE(b.getHi().x == Approx(5.0));
	REQUIRE(b.getHi().y == Approx(5.0));
	REQUIRE(b.getHi().z == Approx(5.0));
	REQUIRE(b.getL().x == Approx(10.0));
	REQUIRE(b.getL().y == Approx(10.0));
	REQUIRE(b.getL().z == Approx(10.0));
	REQUIRE(b.getPeriodic().x == 1);
	REQUIRE(b.getPeriodic().y == 1);
	REQUIRE(b.getPeriodic().z == 1);
}

TEST_CASE("BoxDim_functionality_test1")
{
	BoxDim b(5.0);
	REQUIRE(b.getL().x == Approx(5.0));
	REQUIRE(b.getL().y == Approx(5.0));
	REQUIRE(b.getL().z == Approx(5.0));

	b.setL(make_scalar3(10.0, 10.0, 10.0));
	REQUIRE(b.getL().x == Approx(10.0));
	REQUIRE(b.getL().y == Approx(10.0));
	REQUIRE(b.getL().z == Approx(10.0));

	Scalar3 v = b.makeFraction(make_scalar3(5.0, 5.0, 5.0));
	REQUIRE(v.x == Approx(1.0));
	REQUIRE(v.y == Approx(1.0));
	REQUIRE(v.z == Approx(1.0));

	v = b.makeFraction(make_scalar3(1.0, -2.0, 3.0));
	REQUIRE(v.x == Approx(0.6));
	REQUIRE(v.y == Approx(0.3));
	REQUIRE(v.z == Approx(0.8));

	v = b.minImage(make_scalar3(1.0, -2.0, 3.0));
	REQUIRE(v.x == Approx(1.0));
	REQUIRE(v.y == Approx(-2.0));
	REQUIRE(v.z == Approx(3.0));

	// test minimum image
	v = b.minImage(make_scalar3(6.0, -7.0, 8.0));
	REQUIRE(v.x == Approx(-4.0));
	REQUIRE(v.y == Approx(3.0));
	REQUIRE(v.z == Approx(-2.0));

	b.setPeriodic(make_uchar3(0, 0, 0));
	v = b.minImage(make_scalar3(6.0, -7.0, 8.0));
	REQUIRE(v.x == Approx(6.0));
	REQUIRE(v.y == Approx(-7.0));
	REQUIRE(v.z == Approx(8.0));

	b.setPeriodic(make_uchar3(1, 0, 0));
	v = b.minImage(make_scalar3(6.0, -7.0, 8.0));
	REQUIRE(v.x == Approx(-4.0));
	REQUIRE(v.y == Approx(-7.0));
	REQUIRE(v.z == Approx(8.0));

	b.setPeriodic(make_uchar3(0, 1, 0));
	v = b.minImage(make_scalar3(6.0, -7.0, 8.0));
	REQUIRE(v.x == Approx(6.0));
	REQUIRE(v.y == Approx(3.0));
	REQUIRE(v.z == Approx(8.0));

	b.setPeriodic(make_uchar3(0, 0, 1));
	v = b.minImage(make_scalar3(6.0, -7.0, 8.0));
	REQUIRE(v.x == Approx(6.0));
	REQUIRE(v.y == Approx(-7.0));
	REQUIRE(v.z == Approx(-2.0));

	// test wrap
	b.setPeriodic(make_uchar3(1, 1, 1));
	int3 image = make_int3(10, 20, 30);
	v = make_scalar3(1.0, -2.0, 3.0);
	b.wrap(v, image);
	REQUIRE(v.x == Approx(1.0));
	REQUIRE(v.y == Approx(-2.0));
	REQUIRE(v.z == Approx(3.0));
	REQUIRE(image.x == 10);
	REQUIRE(image.y == 20);
	REQUIRE(image.z == 30);

	image = make_int3(10, 20, 30);
	v = make_scalar3(6.0, -7.0, 8.0);
	b.wrap(v, image);
	REQUIRE(v.x == Approx(-4.0));
	REQUIRE(v.y == Approx(3.0));
	REQUIRE(v.z == Approx(-2.0));
	REQUIRE(image.x == 11);
	REQUIRE(image.y == 19);
	REQUIRE(image.z == 31);

	b.setPeriodic(make_uchar3(1, 0, 0));
	image = make_int3(10, 20, 30);
	v = make_scalar3(6.0, -7.0, 8.0);
	b.wrap(v, image);
	REQUIRE(v.x == Approx(-4.0));
	REQUIRE(v.y == Approx(-7.0));
	REQUIRE(v.z == Approx(8.0));
	REQUIRE(image.x == 11);
	REQUIRE(image.y == 20);
	REQUIRE(image.z == 30);

	b.setPeriodic(make_uchar3(0, 1, 0));
	image = make_int3(10, 20, 30);
	v = make_scalar3(6.0, -7.0, 8.0);
	b.wrap(v, image);
	REQUIRE(v.x == Approx(6.0));
	REQUIRE(v.y == Approx(3.0));
	REQUIRE(v.z == Approx(8.0));
	REQUIRE(image.x == 10);
	REQUIRE(image.y == 19);
	REQUIRE(image.z == 30);

	b.setPeriodic(make_uchar3(0, 0, 1));
	image = make_int3(10, 20, 30);
	v = make_scalar3(6.0, -7.0, 8.0);
	b.wrap(v, image);
	REQUIRE(v.x == Approx(6.0));
	REQUIRE(v.y == Approx(-7.0));
	REQUIRE(v.z == Approx(-2.0));
	REQUIRE(image.x == 10);
	REQUIRE(image.y == 20);
	REQUIRE(image.z == 31);
}

TEST_CASE("BoxDim_triclinic_test")
{
	BoxDim b(5.0);

	Scalar tol = Scalar(1e-4);

	Scalar xy = 1.0;
	Scalar xz = .4;
	Scalar yz = .9;

	b.setTiltFactors(xy, xz, yz);
	REQUIRE(b.getTiltFactorXY() == Approx(xy));
	REQUIRE(b.getTiltFactorXZ() == Approx(xz));
	REQUIRE(b.getTiltFactorYZ() == Approx(yz));

	Scalar3 f = make_scalar3(.5, .6, .7);
	Scalar3 L = b.getL();

	Scalar3 pos;
	pos.x = b.getLo().x + f.x * L.x + xy * L.y * (f.y - Scalar(0.5)) + xz * L.z * (f.z - Scalar(0.5));
	pos.y = b.getLo().y + f.y * L.y + yz * L.z * (f.z - Scalar(0.5));
	pos.z = b.getLo().z + f.z * L.z;

	// convert pos to fraction
	Scalar3 f2 = b.makeFraction(pos);

	REQUIRE(f2.x == Approx(f.x));
	REQUIRE(f2.y == Approx(f.y));
	REQUIRE(f2.z == Approx(f.z));

	// convert fraction to pos
	Scalar3 pos2 = b.makeCoordinates(f);

	REQUIRE(pos2.x == Approx(pos.x));
	REQUIRE(pos2.y == Approx(pos.y));
	REQUIRE(pos2.z == Approx(pos.z));

	/*
	 * test ghost layer padding
	 */
	 // fractional ghost layer
	Scalar3 npd = b.getNearestPlaneDistance();
	Scalar3 ghost_frac = make_scalar3(0.5, 0.3, 0.0);
	Scalar3 ghost_width = ghost_frac * npd;

	// make fraction with a ghost layer
	Scalar3 f3 = b.makeFraction(pos, ghost_width);

	// compare to new fraction, calculated from old fraction accounting for ghost layer
	Scalar3 f3_cmp = (f2 + ghost_frac) / (make_scalar3(1.0, 1.0, 1.0) + 2.0*ghost_frac);
	REQUIRE(f3.x == Approx(f3_cmp.x));
	REQUIRE(f3.y == Approx(f3_cmp.y));
	REQUIRE(f3.z == Approx(f3_cmp.z));

	// test minimum image

	// along x coordinate
	Scalar3 dx = make_scalar3(3.0, 1.0, 2.0);
	Scalar3 dx2 = b.minImage(dx);

	REQUIRE(dx2.x == Approx(-2));
	REQUIRE(dx2.y == Approx(1.0));
	REQUIRE(dx2.z == Approx(2.0));

	dx = make_scalar3(-3.0, 1.0, 2.0);
	dx2 = b.minImage(dx);

	REQUIRE(dx2.x == Approx(2.0));
	REQUIRE(dx2.y == Approx(1.0));
	REQUIRE(dx2.z == Approx(2.0));

	// along y coordinate
	dx = make_scalar3(2.0, 2.6, 1.5);
	dx2 = b.minImage(dx);

	REQUIRE(dx2.x == Approx(2.0));
	REQUIRE(dx2.y == Approx(-2.4));
	REQUIRE(dx2.z == Approx(1.5));

	dx = make_scalar3(2.0, -2.6, 1.5);
	dx2 = b.minImage(dx);

	REQUIRE(dx2.x == Approx(2.0));
	REQUIRE(dx2.y == Approx(2.4));
	REQUIRE(dx2.z == Approx(1.5));

	dx = make_scalar3(3.0, 2.6, 1.5);
	dx2 = b.minImage(dx);

	REQUIRE(dx2.x == Approx(-2.0));
	REQUIRE(dx2.y == Approx(-2.4));
	REQUIRE(dx2.z == Approx(1.5));

	dx = make_scalar3(3.0, -2.6, 1.5);
	dx2 = b.minImage(dx);

	REQUIRE(dx2.x == Approx(-2.0));
	REQUIRE(dx2.y == Approx(2.4));
	REQUIRE(dx2.z == Approx(1.5));

	// along z coordinate
	dx = make_scalar3(2.1, 1.5, 3.0);
	dx2 = b.minImage(dx);

	REQUIRE(dx2.x == Approx(0.1));
	REQUIRE(dx2.y == Approx(2.0));
	REQUIRE(dx2.z == Approx(-2.0));

	dx = make_scalar3(2.1, 1.5, -3.0);
	dx2 = b.minImage(dx);

	REQUIRE(dx2.x == Approx(-0.9));
	REQUIRE(dx2.y == Approx(1.0));
	REQUIRE(dx2.z == Approx(2.0));

	// test particle wrap

	// along z direction
	pos = make_scalar3(1.0, 2.0, 2.6);
	int3 img = make_int3(0, 0, 0);

	b.wrap(pos, img);
	REQUIRE(pos.x == Approx(-1.0));
	REQUIRE(pos.y == Approx(-2.5));
	REQUIRE(pos.z == Approx(-2.4));
	REQUIRE(img.x == 0);
	REQUIRE(img.y == 0);
	REQUIRE(img.z == 1);

	pos = make_scalar3(-1.0, -2.0, -2.6);
	img = make_int3(0, 0, 0);
	b.wrap(pos, img);

	REQUIRE(pos.x == Approx(1.0));
	REQUIRE(pos.y == Approx(2.5));
	REQUIRE(pos.z == Approx(2.4));
	REQUIRE(img.x == 0);
	REQUIRE(img.y == 0);
	REQUIRE(img.z == -1);

	// along y direction
	pos = make_scalar3(1.0, 4.0, 1.5);
	img = make_int3(0, 0, 0);

	b.wrap(pos, img);
	REQUIRE(pos.x == Approx(-4.0));
	REQUIRE(pos.y == Approx(-1.0));
	REQUIRE(pos.z == Approx(1.5));

	REQUIRE(img.x == 0);
	REQUIRE(img.y == 1);
	REQUIRE(img.z == 0);

	pos = make_scalar3(-1.0, -4.0, -1.5);
	img = make_int3(0, 0, 0);

	b.wrap(pos, img);
	REQUIRE(pos.x == Approx(4.0));
	REQUIRE(pos.y == Approx(1.0));
	REQUIRE(pos.z == Approx(-1.5));

	REQUIRE(img.x == 0);
	REQUIRE(img.y == -1);
	REQUIRE(img.z == 0);

	// along x direction
	pos = make_scalar3(4.2, 1.5, 1.0);
	img = make_int3(0, 0, 0);

	b.wrap(pos, img);
	REQUIRE(pos.x == Approx(-0.8));
	REQUIRE(pos.y == Approx(1.5));
	REQUIRE(pos.z == Approx(1.0));

	REQUIRE(img.x == 1);
	REQUIRE(img.y == 0);
	REQUIRE(img.z == 0);

	pos = make_scalar3(-5.0, -1.5, 1.0);
	img = make_int3(0, 0, 0);

	b.wrap(pos, img);
	REQUIRE(pos.x == Approx(0.0));
	REQUIRE(pos.y == Approx(-1.5));
	REQUIRE(pos.z == Approx(1.0));

	REQUIRE(img.x == -1);
	REQUIRE(img.y == 0);
	REQUIRE(img.z == 0);
}