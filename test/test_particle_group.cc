// Copyright (c) 2009-2019 The Regents of the University of Michigan
// This file is part of the HOOMD-blue project, released under the BSD 3-Clause License.


// this include is necessary to get MPI included before anything else to support intel MPI
#include "ExecutionConfiguration.h"

#include <iostream>

#include "ParticleData.h"
#include "Initializers.h"
#include "ParticleGroup.h"

using namespace std;



#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>


//! initializes the particle data used by the tests
std::shared_ptr<SystemDefinition> create_sysdef()
{
	// initialize a box with 10 particles of 4 groups
	BoxDim box(10.0);
	std::shared_ptr<SystemDefinition> sysdef(new SystemDefinition(10, box, 4));
	std::shared_ptr<ParticleData> pdata = sysdef->getParticleData();

	// set the types
	// currently, the position is only set on the first 3 particles, intended for use in the total and center of mass
	// tests. Later, other particles will be added to test the new particle data selectors
	{
		ArrayHandle<Scalar4> h_pos(pdata->getPositions(), access_location::host, access_mode::readwrite);
		ArrayHandle<Scalar4> h_vel(pdata->getVelocities(), access_location::host, access_mode::readwrite);
		ArrayHandle<int3> h_image(pdata->getImages(), access_location::host, access_mode::readwrite);
		ArrayHandle<unsigned int> h_body(pdata->getBodies(), access_location::host, access_mode::readwrite);

		h_pos.data[0].w = __int_as_scalar(0);
		h_pos.data[0].x = Scalar(0.0); h_pos.data[0].y = Scalar(0.0); h_pos.data[0].z = Scalar(0.0);
		h_image.data[0].x = 0; h_image.data[0].y = 0; h_image.data[0].z = 0;
		h_vel.data[0].w = Scalar(1.0); //mass
		h_body.data[0] = 0;

		h_pos.data[1].w = __int_as_scalar(2);
		h_pos.data[1].x = Scalar(1.0); h_pos.data[1].y = Scalar(2.0); h_pos.data[1].z = Scalar(3.0);
		h_image.data[1].x = 1; h_image.data[1].y = -1; h_image.data[1].z = 2;
		h_vel.data[1].w = Scalar(2.0);
		h_body.data[1] = 0;

		h_pos.data[2].w = __int_as_scalar(0);
		h_pos.data[2].x = Scalar(-1.0); h_pos.data[2].y = Scalar(-2.0); h_pos.data[2].z = Scalar(-3.0);
		h_image.data[2].x = 0; h_image.data[2].y = 0; h_image.data[2].z = 0;
		h_vel.data[2].w = Scalar(5.0);
		h_body.data[2] = 1;

		h_pos.data[3].w = __int_as_scalar(1);
		h_pos.data[3].x = Scalar(-4.0); h_pos.data[3].y = Scalar(-4.0); h_pos.data[3].z = Scalar(-4.0);
		h_body.data[3] = 1;

		h_pos.data[4].w = __int_as_scalar(3);
		h_pos.data[4].x = Scalar(-3.5); h_pos.data[4].y = Scalar(-4.5); h_pos.data[4].z = Scalar(-5.0);

		h_pos.data[5].w = __int_as_scalar(0);
		h_pos.data[5].x = Scalar(-5.0); h_pos.data[5].y = Scalar(-4.5); h_pos.data[5].z = Scalar(-3.5);

		h_pos.data[6].w = __int_as_scalar(1);
		h_pos.data[6].x = Scalar(4.0); h_pos.data[6].y = Scalar(4.0); h_pos.data[6].z = Scalar(4.0);

		h_pos.data[7].w = __int_as_scalar(2);
		h_pos.data[7].x = Scalar(3.5); h_pos.data[7].y = Scalar(4.5); h_pos.data[7].z = Scalar(-5.0);

		h_pos.data[8].w = __int_as_scalar(0);
		h_pos.data[8].x = Scalar(5.0); h_pos.data[8].y = Scalar(4.5); h_pos.data[8].z = Scalar(3.5);

		h_pos.data[9].w = __int_as_scalar(3);
		h_pos.data[9].x = Scalar(5.0); h_pos.data[9].y = Scalar(5.0); h_pos.data[9].z = Scalar(5.0);
	}

	return sysdef;
}

//! Checks that ParticleGroup can successfully initialize
TEST_CASE("ParticleGroup_basic_test")
{
	std::shared_ptr<SystemDefinition> sysdef = create_sysdef();
	std::shared_ptr<ParticleData> pdata = sysdef->getParticleData();

	// create an empty group
	ParticleGroup a;
	// copy construct it
	ParticleGroup b(a);
	// copy it
	ParticleGroup c;
	c = a;
}

//! Test copy and equals operators
TEST_CASE("ParticleGroup_copy_test")
{
	std::shared_ptr<SystemDefinition> sysdef = create_sysdef();
	std::shared_ptr<ParticleData> pdata = sysdef->getParticleData();

	// create another particle group of all particles
	std::shared_ptr<ParticleSelector> selector_all(new ParticleSelectorTag(sysdef, 0, pdata->getN() - 1));
	ParticleGroup tags_all(sysdef, selector_all);
	// verify it
	REQUIRE(tags_all.getNumMembers() == (unsigned int) pdata->getN());
	REQUIRE(tags_all.getIndexArray().getNumElements()== (unsigned int) pdata->getN());

	for (unsigned int i = 0; i < pdata->getN(); i++)
	{
		REQUIRE(tags_all.getMemberTag(i) == (unsigned int)i);
		REQUIRE(tags_all.getMemberIndex(i) == (unsigned int)i);
		REQUIRE(tags_all.isMember(i));
	}

	// copy construct it
	ParticleGroup copy1(tags_all);
	// verify it
	REQUIRE(copy1.getNumMembers()== (unsigned int) (pdata->getN()));
	REQUIRE(copy1.getIndexArray().getNumElements()== (unsigned int) (pdata->getN()));
	for (unsigned int i = 0; i < pdata->getN(); i++)
	{
		REQUIRE(copy1.getMemberTag(i) == (unsigned int)i);
		REQUIRE(copy1.getMemberIndex(i) == (unsigned int)i);
		REQUIRE(copy1.isMember(i));
	}

	// copy it
	ParticleGroup copy2;
	copy2 = copy1;
	// verify it
	REQUIRE(copy2.getNumMembers()== (unsigned int) (pdata->getN()));
	REQUIRE(copy2.getIndexArray().getNumElements()== (unsigned int) (pdata->getN()));
	for (unsigned int i = 0; i < pdata->getN(); i++)
	{
		REQUIRE(copy2.getMemberTag(i) == (unsigned int)i);
		REQUIRE(copy2.getMemberIndex(i) == (unsigned int)i);
		REQUIRE(copy2.isMember(i));
	}
}

//! Checks that ParticleGroup can successfully handle particle resorts
TEST_CASE("ParticleGroup_sort_test")
{
	std::shared_ptr<SystemDefinition> sysdef = create_sysdef();
	std::shared_ptr<ParticleData> pdata = sysdef->getParticleData();

	std::shared_ptr<ParticleSelector> selector04(new ParticleSelectorTag(sysdef, 0, 4));
	ParticleGroup tags04(sysdef, selector04);
	// verify the initial set
	REQUIRE(tags04.getNumMembers() == (unsigned int)5);
	REQUIRE(tags04.getIndexArray().getNumElements() == (unsigned int)5);
	for (unsigned int i = 0; i < 5; i++)
	{
		REQUIRE(tags04.getMemberTag(i) == (unsigned int)i);
		REQUIRE(tags04.getMemberIndex(i) == (unsigned int)i);
	}

	for (unsigned int i = 0; i < pdata->getN(); i++)
	{
		if (i <= 4)
			REQUIRE(tags04.isMember(i));
		else
			REQUIRE(!tags04.isMember(i));
	}

	// resort the particles
	{
		ArrayHandle<unsigned int> h_tag(pdata->getTags(), access_location::host, access_mode::readwrite);
		ArrayHandle<unsigned int> h_rtag(pdata->getRTags(), access_location::host, access_mode::readwrite);

		// set the types
		h_tag.data[0] = 9;
		h_tag.data[1] = 8;
		h_tag.data[2] = 7;
		h_tag.data[3] = 6;
		h_tag.data[4] = 5;
		h_tag.data[5] = 4;
		h_tag.data[6] = 3;
		h_tag.data[7] = 2;
		h_tag.data[8] = 1;
		h_tag.data[9] = 0;

		h_rtag.data[0] = 9;
		h_rtag.data[1] = 8;
		h_rtag.data[2] = 7;
		h_rtag.data[3] = 6;
		h_rtag.data[4] = 5;
		h_rtag.data[5] = 4;
		h_rtag.data[6] = 3;
		h_rtag.data[7] = 2;
		h_rtag.data[8] = 1;
		h_rtag.data[9] = 0;
	}

	pdata->notifyParticleSort();

	// verify that the group has updated
	REQUIRE(tags04.getNumMembers() == (unsigned int)5);
	REQUIRE(tags04.getIndexArray().getNumElements() == (unsigned int)5);
	for (unsigned int i = 0; i < 5; i++)
	{
		REQUIRE(tags04.getMemberTag(i) == (unsigned int)i);
		// indices are in sorted order (tags 0-4 are particles 9-5)
		REQUIRE(tags04.getMemberIndex(i) == (unsigned int)i + 5);
	}
	{
		ArrayHandle<unsigned int> h_tag(pdata->getTags(), access_location::host, access_mode::readwrite);
		for (unsigned int i = 0; i < pdata->getN(); i++)
		{
			if (h_tag.data[i] <= 4)
				REQUIRE(tags04.isMember(i));
			else
				REQUIRE(!tags04.isMember(i));
		}
	}
}

//! Checks that ParticleGroup can initialize by particle type
TEST_CASE("ParticleGroup_type_test")
{
	std::shared_ptr<SystemDefinition> sysdef = create_sysdef();
	std::shared_ptr<ParticleData> pdata = sysdef->getParticleData();

	// create a group of type 0 and check it
	std::shared_ptr<ParticleSelector> selector0(new ParticleSelectorType(sysdef, 0, 0));
	ParticleGroup type0(sysdef, selector0);
	REQUIRE(type0.getNumMembers() == (unsigned int)4);
	REQUIRE(type0.getIndexArray().getNumElements() == (unsigned int)4);

	REQUIRE(type0.getMemberTag(0) == (unsigned int)0);
	REQUIRE(type0.getMemberTag(1) == (unsigned int)2);
	REQUIRE(type0.getMemberTag(2) == (unsigned int)5);
	REQUIRE(type0.getMemberTag(3) == (unsigned int)8);

	// create a group of type 1 and check it
	std::shared_ptr<ParticleSelector> selector1(new ParticleSelectorType(sysdef, 1, 1));
	ParticleGroup type1(sysdef, selector1);
	REQUIRE(type1.getNumMembers() == (unsigned int)2);
	REQUIRE(type1.getIndexArray().getNumElements() == (unsigned int)2);
	REQUIRE(type1.getMemberTag(0) == (unsigned int)3);
	REQUIRE(type1.getMemberTag(1) == (unsigned int)6);

	// create a group of type 2 and check it
	std::shared_ptr<ParticleSelector> selector2(new ParticleSelectorType(sysdef, 2, 2));
	ParticleGroup type2(sysdef, selector2);
	REQUIRE(type2.getNumMembers() == (unsigned int)2);
	REQUIRE(type2.getIndexArray().getNumElements() == (unsigned int)2);
	REQUIRE(type2.getMemberTag(0) == (unsigned int)1);
	REQUIRE(type2.getMemberTag(1) == (unsigned int)7);

	// create a group of type 3 and check it
	std::shared_ptr<ParticleSelector> selector3(new ParticleSelectorType(sysdef, 3, 3));
	ParticleGroup type3(sysdef, selector3);
	REQUIRE(type3.getNumMembers() == (unsigned int)2);
	REQUIRE(type3.getIndexArray().getNumElements() == (unsigned int)2);
	REQUIRE(type3.getMemberTag(0) == (unsigned int)4);
	REQUIRE(type3.getMemberTag(1) == (unsigned int)9);

	// create a group of all types and check it
	std::shared_ptr<ParticleSelector> selector_all(new ParticleSelectorType(sysdef, 0, 3));
	ParticleGroup alltypes(sysdef, selector_all);
	REQUIRE(alltypes.getNumMembers() == (unsigned int)10);
	REQUIRE(alltypes.getIndexArray().getNumElements() == (unsigned int)10);
	for (unsigned int i = 0; i < 10; i++)
		REQUIRE(alltypes.getMemberTag(i) == (unsigned int)i);
}

//! Checks that ParticleGroup can initialize to the empty set
TEST_CASE("ParticleGroup_empty_test")
{
	std::shared_ptr<SystemDefinition> sysdef = create_sysdef();
	std::shared_ptr<ParticleData> pdata = sysdef->getParticleData();

	// create a group of type 100 and check it
	std::shared_ptr<ParticleSelector> selector100(new ParticleSelectorType(sysdef, 100, 100));
	ParticleGroup empty(sysdef, selector100);
	REQUIRE(empty.getNumMembers() == (unsigned int)0);
	REQUIRE(empty.getIndexArray().getNumElements() == (unsigned int)0);
}

//! Checks that ParticleGroup can initialize by particle body
TEST_CASE("ParticleGroup_body_test")
{
	std::shared_ptr<SystemDefinition> sysdef = create_sysdef();
	std::shared_ptr<ParticleData> pdata = sysdef->getParticleData();

	// create a group of rigid bodies and check it
	std::shared_ptr<ParticleSelector> selector_body_true(new ParticleSelectorRigid(sysdef, true));
	ParticleGroup type_true(sysdef, selector_body_true);
	REQUIRE(type_true.getNumMembers() == (unsigned int)4);
	REQUIRE(type_true.getMemberTag(0) == (unsigned int)0);
	REQUIRE(type_true.getMemberTag(1) == (unsigned int)1);
	REQUIRE(type_true.getMemberTag(2) == (unsigned int)2);
	REQUIRE(type_true.getMemberTag(3) == (unsigned int)3);

	// create a group of non rigid particles and check it
	std::shared_ptr<ParticleSelector> selector_body_false(new ParticleSelectorRigid(sysdef, false));
	ParticleGroup type_false(sysdef, selector_body_false);
	REQUIRE(type_false.getNumMembers() == (unsigned int)6);
	REQUIRE(type_false.getMemberTag(0) == (unsigned int)4);
	REQUIRE(type_false.getMemberTag(1) == (unsigned int)5);
	REQUIRE(type_false.getMemberTag(2) == (unsigned int)6);
	REQUIRE(type_false.getMemberTag(3) == (unsigned int)7);
	REQUIRE(type_false.getMemberTag(4) == (unsigned int)8);
	REQUIRE(type_false.getMemberTag(5) == (unsigned int)9);
}

//! Checks that ParticleGroup can initialize by particle tag
TEST_CASE("ParticleGroup_tag_test")
{
	std::shared_ptr<SystemDefinition> sysdef = create_sysdef();
	std::shared_ptr<ParticleData> pdata = sysdef->getParticleData();

	// create a group of tags 0-4 and check it
	std::shared_ptr<ParticleSelector> selector04(new ParticleSelectorTag(sysdef, 0, 4));
	ParticleGroup tags05(sysdef, selector04);
	REQUIRE(tags05.getNumMembers() == (unsigned int)5);
	REQUIRE(tags05.getIndexArray().getNumElements() == (unsigned int)5);
	REQUIRE(tags05.getMemberTag(0) == (unsigned int)0);
	REQUIRE(tags05.getMemberTag(1) == (unsigned int)1);
	REQUIRE(tags05.getMemberTag(2) == (unsigned int)2);
	REQUIRE(tags05.getMemberTag(3) == (unsigned int)3);
	REQUIRE(tags05.getMemberTag(4) == (unsigned int)4);

	// create a group of tags 5-9 and check it
	std::shared_ptr<ParticleSelector> selector59(new ParticleSelectorTag(sysdef, 5, 9));
	ParticleGroup tags59(sysdef, selector59);
	REQUIRE(tags59.getNumMembers() == (unsigned int)5);
	REQUIRE(tags59.getIndexArray().getNumElements() == (unsigned int)5);
	REQUIRE(tags59.getMemberTag(0) == (unsigned int)5);
	REQUIRE(tags59.getMemberTag(1) == (unsigned int)6);
	REQUIRE(tags59.getMemberTag(2) == (unsigned int)7);
	REQUIRE(tags59.getMemberTag(3) == (unsigned int)8);
	REQUIRE(tags59.getMemberTag(4) == (unsigned int)9);
}

//! Checks that ParticleGroup can initialize by cuboid
TEST_CASE("ParticleGroup_cuboid_test")
{
	std::shared_ptr<SystemDefinition> sysdef = create_sysdef();
	std::shared_ptr<ParticleData> pdata = sysdef->getParticleData();

	// create a group containing only particle 0
	std::shared_ptr<ParticleSelector> selector0(new ParticleSelectorCuboid(sysdef,
		make_scalar3(-0.5, -0.5, -0.5),
		make_scalar3(0.5, 0.5, 0.5)));
	ParticleGroup tags0(sysdef, selector0);
	REQUIRE(tags0.getNumMembers() == (unsigned int)1);
	REQUIRE(tags0.getIndexArray().getNumElements() == (unsigned int)1);
	REQUIRE(tags0.getMemberTag(0) == (unsigned int)0);

	// create a group containing particles 0 and 1
	std::shared_ptr<ParticleSelector> selector1(new ParticleSelectorCuboid(sysdef,
		make_scalar3(-0.5, -0.5, -0.5),
		make_scalar3(1.5, 2.5, 3.5)));
	ParticleGroup tags1(sysdef, selector1);
	REQUIRE(tags1.getNumMembers() == (unsigned int)2);
	REQUIRE(tags1.getIndexArray().getNumElements() == (unsigned int)2);
	REQUIRE(tags1.getMemberTag(0) == (unsigned int)0);
	REQUIRE(tags1.getMemberTag(1) == (unsigned int)1);

	// create a group containing particles 0, 1 and 2
	std::shared_ptr<ParticleSelector> selector2(new ParticleSelectorCuboid(sysdef,
		make_scalar3(-1.5, -2.5, -3.5),
		make_scalar3(1.5, 2.5, 3.5)));
	ParticleGroup tags2(sysdef, selector2);
	REQUIRE(tags2.getNumMembers() == (unsigned int)3);
	REQUIRE(tags2.getIndexArray().getNumElements() == (unsigned int)3);
	REQUIRE(tags2.getMemberTag(0) == (unsigned int)0);
	REQUIRE(tags2.getMemberTag(1) == (unsigned int)1);
	REQUIRE(tags2.getMemberTag(2) == (unsigned int)2);
}

//! Checks that the ParticleGroup boolean operation work correctly
TEST_CASE("ParticleGroup_boolean_tests")
{
	std::shared_ptr<SystemDefinition> sysdef = create_sysdef();
	std::shared_ptr<ParticleData> pdata = sysdef->getParticleData();

	// create a group of tags 0-4
	std::shared_ptr<ParticleSelector> selector04(new ParticleSelectorTag(sysdef, 0, 4));
	std::shared_ptr<ParticleGroup> tags04(new ParticleGroup(sysdef, selector04));

	// create a group of type 0
	std::shared_ptr<ParticleSelector> selector0(new ParticleSelectorType(sysdef, 0, 0));
	std::shared_ptr<ParticleGroup> type0(new ParticleGroup(sysdef, selector0));

	// make a union of the two groups and check it
	std::shared_ptr<ParticleGroup> union_group = ParticleGroup::groupUnion(type0, tags04);
	REQUIRE(union_group->getNumMembers() == (unsigned int)7);
	REQUIRE(union_group->getIndexArray().getNumElements() == (unsigned int)7);
	REQUIRE(union_group->getMemberTag(0) == (unsigned int)0);
	REQUIRE(union_group->getMemberTag(1) == (unsigned int)1);
	REQUIRE(union_group->getMemberTag(2) == (unsigned int)2);
	REQUIRE(union_group->getMemberTag(3) == (unsigned int)3);
	REQUIRE(union_group->getMemberTag(4) == (unsigned int)4);
	REQUIRE(union_group->getMemberTag(5) == (unsigned int)5);
	REQUIRE(union_group->getMemberTag(6) == (unsigned int)8);

	// make a intersection group and test it
	std::shared_ptr<ParticleGroup> intersection_group = ParticleGroup::groupIntersection(type0, tags04);
	REQUIRE(intersection_group->getNumMembers() == (unsigned int)2);
	REQUIRE(intersection_group->getIndexArray().getNumElements() == (unsigned int)2);
	REQUIRE(intersection_group->getMemberTag(0) == (unsigned int)0);
	REQUIRE(intersection_group->getMemberTag(1) == (unsigned int)2);
}

//! Checks that the ParticleGroup::getTotalMass works correctly
TEST_CASE("ParticleGroup_total_mass_tests")
{
	std::shared_ptr<SystemDefinition> sysdef = create_sysdef();
	std::shared_ptr<ParticleData> pdata = sysdef->getParticleData();

	ParticleGroup group1(sysdef, std::shared_ptr<ParticleSelector>(new ParticleSelectorTag(sysdef, 0, 0)));
	REQUIRE(group1.getTotalMass() == Approx(1.0));

	ParticleGroup group2(sysdef, std::shared_ptr<ParticleSelector>(new ParticleSelectorTag(sysdef, 0, 1)));
	REQUIRE(group2.getTotalMass() == Approx(3.0));

	ParticleGroup group3(sysdef, std::shared_ptr<ParticleSelector>(new ParticleSelectorTag(sysdef, 0, 2)));
	REQUIRE(group3.getTotalMass() == Approx(8.0));
}

//! Tolerance setting for near-zero comparisons
const Scalar tol_small = Scalar(1e-3);

//! Checks that the ParticleGroup::getCenterOfMass works correctly
TEST_CASE("ParticleGroup_center_of_mass_tests")
{
	std::shared_ptr<SystemDefinition> sysdef = create_sysdef();
	std::shared_ptr<ParticleData> pdata = sysdef->getParticleData();

	Scalar3 com;
	ParticleGroup group1(sysdef, std::shared_ptr<ParticleSelector>(new ParticleSelectorTag(sysdef, 0, 0)));
	com = group1.getCenterOfMass();
	REQUIRE(std::abs(com.x) < tol_small);
	REQUIRE(std::abs(com.y) < tol_small);
	REQUIRE(std::abs(com.z) < tol_small);

	ParticleGroup group2(sysdef, std::shared_ptr<ParticleSelector>(new ParticleSelectorTag(sysdef, 0, 1)));
	com = group2.getCenterOfMass();
	REQUIRE(com.x == Approx(7.3333333333));
	REQUIRE(com.y == Approx(-5.3333333333));
	REQUIRE(com.z == Approx(15.333333333));

	ParticleGroup group3(sysdef, std::shared_ptr<ParticleSelector>(new ParticleSelectorTag(sysdef, 0, 2)));
	com = group3.getCenterOfMass();
	REQUIRE(com.x == Approx(2.125));
	REQUIRE(com.y == Approx(-3.25));
	REQUIRE(com.z == Approx(3.875));
}
