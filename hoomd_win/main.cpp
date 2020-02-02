// Copyright (c) 2009-2019 The Regents of the University of Michigan
// This file is part of the HOOMD-blue project, released under the BSD 3-Clause License.


// this include is necessary to get MPI included before anything else to support intel MPI
#include "ExecutionConfiguration.h"

#include <iostream>

#include "ParticleData.h"
#include "Initializers.h"
#include "SnapshotSystemData.h"

using namespace std;



#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>


//! Test operation of the particle data class
TEST_CASE("ParticleData_test")
{
	BoxDim box(10.0, 30.0, 50.0);
	std::shared_ptr<ExecutionConfiguration> exec_conf(new ExecutionConfiguration(ExecutionConfiguration::GPU));//Can switch to CPU as well

	ParticleData a(1, box, 1, exec_conf);

	Scalar tol = Scalar(1e-6);

	// make sure the box is working
	const BoxDim& c = a.getBox();
	REQUIRE(c.getLo().x == Approx(-5.0));
	REQUIRE(c.getLo().y == Approx(-15.0));
	REQUIRE(c.getLo().z == Approx(-25.0));
	REQUIRE(c.getHi().x == Approx(5.0));
	REQUIRE(c.getHi().y == Approx(15.0));
	REQUIRE(c.getHi().z == Approx(25.0));

	a.setGlobalBoxL(make_scalar3(5.0, 5.0, 5.0));
	const BoxDim& d = a.getBox();
	REQUIRE(d.getLo().x == Approx(-2.5));
	REQUIRE(d.getLo().y == Approx(-2.5));
	REQUIRE(d.getLo().z == Approx(-2.5));
	REQUIRE(d.getHi().x == Approx(2.5));
	REQUIRE(d.getHi().y == Approx(2.5));
	REQUIRE(d.getHi().z == Approx(2.5));

	// make sure that getN is working
	REQUIRE(a.getN() == 1);

	// Test the ability to acquire data
	{
		ArrayHandle<Scalar4> h_pos(a.getPositions(), access_location::host, access_mode::readwrite);
		ArrayHandle<Scalar4> h_vel(a.getVelocities(), access_location::host, access_mode::readwrite);
		ArrayHandle<Scalar3> h_accel(a.getAccelerations(), access_location::host, access_mode::readwrite);
		ArrayHandle<int3> h_image(a.getImages(), access_location::host, access_mode::readwrite);
		ArrayHandle<Scalar> h_charge(a.getCharges(), access_location::host, access_mode::readwrite);
		ArrayHandle<Scalar> h_diameter(a.getDiameters(), access_location::host, access_mode::readwrite);
		ArrayHandle<unsigned int> h_tag(a.getTags(), access_location::host, access_mode::readwrite);
		ArrayHandle<unsigned int> h_rtag(a.getRTags(), access_location::host, access_mode::readwrite);
		ArrayHandle<unsigned int> h_body(a.getBodies(), access_location::host, access_mode::readwrite);

		// begin by verifying that the defaults the class advertises are set
		REQUIRE(a.getPositions().getNumElements() == 1);
		REQUIRE(a.getVelocities().getNumElements() == 1);
		REQUIRE(a.getAccelerations().getNumElements() == 1);
		REQUIRE(a.getImages().getNumElements() == 1);
		REQUIRE(a.getCharges().getNumElements() == 1);
		REQUIRE(a.getDiameters().getNumElements() == 1);
		REQUIRE(a.getTags().getNumElements() == 1);
		REQUIRE(a.getRTags().size() == 1);
		REQUIRE(a.getBodies().getNumElements() == 1);


		REQUIRE(h_pos.data[0].x == Approx(0.0));
		REQUIRE(h_pos.data[0].y == Approx(0.0));
		REQUIRE(h_pos.data[0].z == Approx(0.0));
		REQUIRE(h_vel.data[0].x == Approx(0.0));
		REQUIRE(h_vel.data[0].y == Approx(0.0));
		REQUIRE(h_vel.data[0].z == Approx(0.0));
		REQUIRE(h_accel.data[0].x == Approx(0.0));
		REQUIRE(h_accel.data[0].y == Approx(0.0));
		REQUIRE(h_accel.data[0].z == Approx(0.0));

		REQUIRE(h_charge.data[0] == Approx(0.0));
		REQUIRE(h_vel.data[0].w == Approx(1.0));//mass
		REQUIRE(h_diameter.data[0] == Approx(1.0));

		REQUIRE(h_image.data[0].x == 0);
		REQUIRE(h_image.data[0].y == 0);
		REQUIRE(h_image.data[0].z == 0);

		REQUIRE(__scalar_as_int(h_pos.data[0].w) == 0); //type
		REQUIRE(h_rtag.data[0] == 0);
		REQUIRE(h_tag.data[0] == 0);
		REQUIRE(h_body.data[0] == NO_BODY);

		// set some new values for testing
		h_pos.data[0].x = 1.0;
		h_pos.data[0].y = 2.0;
		h_pos.data[0].z = -2.0;
		h_vel.data[0].x = 11.0;
		h_vel.data[0].y = 12.0;
		h_vel.data[0].z = 13.0;
		h_accel.data[0].x = 21.0;
		h_accel.data[0].y = 22.0;
		h_accel.data[0].z = 23.0;
		h_charge.data[0] = 24.0;
		h_vel.data[0].w = 25.0; // mass
		h_diameter.data[0] = 26.0;
		h_image.data[0].x = 27;
		h_image.data[0].y = 28;
		h_image.data[0].z = 29;
		h_pos.data[0].w = __int_as_scalar(1); //type
		h_body.data[0] = 0;

	}

	// make sure when the data is re-acquired, the values read properly
	{
		ArrayHandle<Scalar4> h_pos(a.getPositions(), access_location::host, access_mode::read);
		ArrayHandle<Scalar4> h_vel(a.getVelocities(), access_location::host, access_mode::read);
		ArrayHandle<Scalar3> h_accel(a.getAccelerations(), access_location::host, access_mode::read);
		ArrayHandle<int3> h_image(a.getImages(), access_location::host, access_mode::read);
		ArrayHandle<Scalar> h_charge(a.getCharges(), access_location::host, access_mode::read);
		ArrayHandle<Scalar> h_diameter(a.getDiameters(), access_location::host, access_mode::read);
		ArrayHandle<unsigned int> h_tag(a.getTags(), access_location::host, access_mode::read);
		ArrayHandle<unsigned int> h_rtag(a.getRTags(), access_location::host, access_mode::read);
		ArrayHandle<unsigned int> h_body(a.getBodies(), access_location::host, access_mode::read);

		REQUIRE(a.getPositions().getNumElements() == 1);
		REQUIRE(a.getVelocities().getNumElements() == 1);
		REQUIRE(a.getAccelerations().getNumElements() == 1);
		REQUIRE(a.getImages().getNumElements() == 1);
		REQUIRE(a.getCharges().getNumElements() == 1);
		REQUIRE(a.getDiameters().getNumElements() == 1);
		REQUIRE(a.getTags().getNumElements() == 1);
		REQUIRE(a.getRTags().getNumElements() == 1);
		REQUIRE(a.getBodies().getNumElements() == 1);

		REQUIRE(h_pos.data[0].x == Approx(1.0));
		REQUIRE(h_pos.data[0].y == Approx(2.0));
		REQUIRE(h_pos.data[0].z == Approx(-2.0));
		REQUIRE(h_vel.data[0].x == Approx(11.0));
		REQUIRE(h_vel.data[0].y == Approx(12.0));
		REQUIRE(h_vel.data[0].z == Approx(13.0));
		REQUIRE(h_accel.data[0].x == Approx(21.0));
		REQUIRE(h_accel.data[0].y == Approx(22.0));
		REQUIRE(h_accel.data[0].z == Approx(23.0));

		REQUIRE(h_charge.data[0] == Approx(24.0));
		REQUIRE(h_vel.data[0].w == Approx(25.0));//mass
		REQUIRE(h_diameter.data[0] == Approx(26.0));

		REQUIRE(h_image.data[0].x == 27);
		REQUIRE(h_image.data[0].y == 28);
		REQUIRE(h_image.data[0].z == 29);

		REQUIRE(__scalar_as_int(h_pos.data[0].w) == 1); //type
		REQUIRE(h_rtag.data[0] == 0);
		REQUIRE(h_tag.data[0] == 0);
		REQUIRE(h_body.data[0] == 0);

	}
	// finally, lets check a larger ParticleData for correctness of the initialization
	const unsigned int N = 1000;
	ParticleData b(N, box, 1, exec_conf);
	{
		ArrayHandle<Scalar4> h_pos(b.getPositions(), access_location::host, access_mode::read);
		ArrayHandle<Scalar4> h_vel(b.getVelocities(), access_location::host, access_mode::read);
		ArrayHandle<Scalar3> h_accel(b.getAccelerations(), access_location::host, access_mode::read);
		ArrayHandle<int3> h_image(b.getImages(), access_location::host, access_mode::read);
		ArrayHandle<Scalar> h_charge(b.getCharges(), access_location::host, access_mode::read);
		ArrayHandle<Scalar> h_diameter(b.getDiameters(), access_location::host, access_mode::read);
		ArrayHandle<unsigned int> h_tag(b.getTags(), access_location::host, access_mode::read);
		ArrayHandle<unsigned int> h_rtag(b.getRTags(), access_location::host, access_mode::read);
		ArrayHandle<unsigned int> h_body(b.getBodies(), access_location::host, access_mode::read);

		// begin by verifying that the defaults the class advertises are set
		REQUIRE(b.getPositions().getNumElements() == N);
		REQUIRE(b.getVelocities().getNumElements() == N);
		REQUIRE(b.getAccelerations().getNumElements() == N);
		REQUIRE(b.getImages().getNumElements() == N);
		REQUIRE(b.getCharges().getNumElements() == N);
		REQUIRE(b.getDiameters().getNumElements() == N);
		REQUIRE(b.getTags().getNumElements() == N);
		REQUIRE(b.getRTags().size() == N);
		REQUIRE(b.getBodies().getNumElements() == N);


		for (unsigned int i = 0; i < N; i++)
		{
			REQUIRE(h_pos.data[i].x == Approx(0.0));
			REQUIRE(h_pos.data[i].y == Approx(0.0));
			REQUIRE(h_pos.data[i].z == Approx(0.0));
			REQUIRE(h_vel.data[i].x == Approx(0.0));
			REQUIRE(h_vel.data[i].y == Approx(0.0));
			REQUIRE(h_vel.data[i].z == Approx(0.0));
			REQUIRE(h_accel.data[i].x == Approx(0.0));
			REQUIRE(h_accel.data[i].y == Approx(0.0));
			REQUIRE(h_accel.data[i].z == Approx(0.0));

			REQUIRE(h_charge.data[i] == Approx(0.0));
			REQUIRE(h_vel.data[i].w == Approx(1.0));//mass
			REQUIRE(h_diameter.data[i] == Approx(1.0));

			REQUIRE(h_image.data[i].x == 0);
			REQUIRE(h_image.data[i].y == 0);
			REQUIRE(h_image.data[i].z == 0);

			REQUIRE(__scalar_as_int(h_pos.data[i].w) == 0); //type
			REQUIRE(h_rtag.data[i] == i);
			REQUIRE(h_tag.data[i] == i);
			REQUIRE(h_body.data[i] == NO_BODY);
		}

	}

	// check that new types can be added
	ParticleData pdata_type_test(123, box, 1, exec_conf);

	REQUIRE(pdata_type_test.getNTypes() == 1);
	REQUIRE(pdata_type_test.addType("test") == 1);
	REQUIRE(pdata_type_test.getNTypes() == 2);
	REQUIRE(pdata_type_test.getNameByType(1) == "test");
	REQUIRE(pdata_type_test.getTypeByName("test") == 1);
}


//! Tests the RandomParticleInitializer class
TEST_CASE("Random_test")
{
	// create a fairly dense system with a minimum distance of 0.8
	std::shared_ptr<ExecutionConfiguration> exec_conf(new ExecutionConfiguration(ExecutionConfiguration::GPU));
	Scalar min_dist = Scalar(0.8);
	RandomInitializer rand_init(500, Scalar(0.4), min_dist, "ABC");
	std::shared_ptr< SnapshotSystemData<Scalar> > snap = rand_init.getSnapshot();
	ParticleData pdata(snap->particle_data, snap->global_box, exec_conf);

	REQUIRE(pdata.getNameByType(0)== "ABC");
	REQUIRE(pdata.getTypeByName("ABC")== (unsigned int)0);

	{
		ArrayHandle<Scalar4> h_pos(pdata.getPositions(), access_location::host, access_mode::read);

		// check that the distances between particles are OK
		BoxDim box = pdata.getBox();
		Scalar L = box.getL().x;
		for (unsigned int i = 0; i < pdata.getN(); i++)
		{
			REQUIRE(h_pos.data[i].x <= box.getHi().x); REQUIRE(h_pos.data[i].x >= box.getLo().x);
			REQUIRE(h_pos.data[i].y <= box.getHi().x); REQUIRE(h_pos.data[i].y >= box.getLo().x);
			REQUIRE(h_pos.data[i].z <= box.getHi().x); REQUIRE(h_pos.data[i].z >= box.getLo().x);

			for (unsigned int j = 0; j < pdata.getN(); j++)
			{
				if (i == j)
					continue;

				Scalar dx = h_pos.data[j].x - h_pos.data[i].x;
				Scalar dy = h_pos.data[j].y - h_pos.data[i].y;
				Scalar dz = h_pos.data[j].z - h_pos.data[i].z;

				if (dx < -L / Scalar(2.0))
					dx += L;
				if (dx > L / Scalar(2.0))
					dx -= L;

				if (dy < -L / Scalar(2.0))
					dy += L;
				if (dy > L / Scalar(2.0))
					dy -= L;

				if (dz < -L / Scalar(2.0))
					dz += L;
				if (dz > L / Scalar(2.0))
					dz -= L;

				Scalar dr2 = dx * dx + dy * dy + dz * dz;
				REQUIRE(dr2 >= min_dist * min_dist);
			}
		}

	}
}