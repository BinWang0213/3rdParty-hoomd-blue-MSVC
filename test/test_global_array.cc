// this include is necessary to get MPI included before anything else to support intel MPI
#include "ExecutionConfiguration.h"

#include <iostream>

#include <memory>

#include "GlobalArray.h"
#include "GPUVector.h"

#ifdef ENABLE_CUDA
#include "test_gpu_array.cuh"
#endif

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

using namespace std;



//! test case for testing the basic operation of GlobalArray
TEST_CASE("GlobalArray_basic_tests")
{
	std::shared_ptr<ExecutionConfiguration> exec_conf(new ExecutionConfiguration(ExecutionConfiguration::CPU));
	GlobalArray<int> gpu_array(100, exec_conf);

	// basic check: ensure that the number of elements is set correctly
	REQUIRE((int)gpu_array.getNumElements() == 100);

	// basic check 2: acquire the data on the host and fill out a pattern
	{
		ArrayHandle<int> h_handle(gpu_array, access_location::host, access_mode::readwrite);
		REQUIRE(h_handle.data != NULL);
		for (int i = 0; i < (int)gpu_array.getNumElements(); i++)
			h_handle.data[i] = i;
	}

	// basic check 3: verify the data set in check 2
	{
		ArrayHandle<int> h_handle(gpu_array, access_location::host, access_mode::read);
		REQUIRE(h_handle.data != NULL);
		for (int i = 0; i < (int)gpu_array.getNumElements(); i++)
			REQUIRE(h_handle.data[i] == i);
	}

	// basic check 3.5: test the construction of a 2-D GlobalArray
	GlobalArray<int> gpu_array_2d(63, 120, exec_conf);
	REQUIRE((int)gpu_array_2d.getPitch() == 64);
	REQUIRE((int)gpu_array_2d.getHeight() == 120);
	REQUIRE((int)gpu_array_2d.getNumElements() == 7680);

	// basic check 4: verify the copy constructor
	GlobalArray<int> array_b(gpu_array);
	REQUIRE((int)array_b.getNumElements() == 100);
	REQUIRE((int)array_b.getPitch() == 100);
	REQUIRE((int)array_b.getHeight() == 1);

	{
		ArrayHandle<int> h_handle(array_b, access_location::host, access_mode::read);
		REQUIRE(h_handle.data != NULL);
		for (int i = 0; i < (int)array_b.getNumElements(); i++)
			REQUIRE(h_handle.data[i] == i);
	}

	// basic check 5: verify the = operator
	GlobalArray<int> array_c(1, exec_conf);
	array_c = gpu_array;

	REQUIRE((int)array_c.getNumElements() == 100);
	REQUIRE((int)array_c.getPitch() == 100);
	REQUIRE((int)array_c.getHeight() == 1);

	{
		ArrayHandle<int> h_handle(array_c, access_location::host, access_mode::read);
		REQUIRE(h_handle.data != NULL);
		for (int i = 0; i < (int)array_c.getNumElements(); i++)
			REQUIRE(h_handle.data[i] == i);
	}

}

#ifdef ENABLE_CUDA
//! test case for testing device to/from host transfers
TEST_CASE("GlobalArray_transfer_tests")
{
	std::shared_ptr<ExecutionConfiguration> exec_conf(new ExecutionConfiguration(ExecutionConfiguration::GPU));
	REQUIRE(exec_conf->isCUDAEnabled());

	GlobalArray<int> gpu_array(100, exec_conf);

	// initialize the data on the device
	{
		ArrayHandle<int> d_handle(gpu_array, access_location::device, access_mode::readwrite);
		REQUIRE(d_handle.data != NULL);

		gpu_fill_test_pattern(d_handle.data, gpu_array.getNumElements());
		cudaError_t err_sync = cudaGetLastError();
		exec_conf->handleCUDAError(err_sync, __FILE__, __LINE__);
	}

	// copy it to the host and verify
	{
		ArrayHandle<int> h_handle(gpu_array, access_location::host, access_mode::readwrite);
		REQUIRE(h_handle.data != NULL);
		for (int i = 0; i < (int)gpu_array.getNumElements(); i++)
		{
			REQUIRE(h_handle.data[i] == i*i);
			// overwrite the data as we verify
			h_handle.data[i] = 100 + i;
		}
	}

	// test host-> device copies
	{
		ArrayHandle<int> d_handle(gpu_array, access_location::device, access_mode::readwrite);
		REQUIRE(d_handle.data != NULL);

		gpu_add_one(d_handle.data, gpu_array.getNumElements());
		cudaError_t err_sync = cudaGetLastError();
		exec_conf->handleCUDAError(err_sync, __FILE__, __LINE__);
	}

	// via the read access mode
	{
		ArrayHandle<int> h_handle(gpu_array, access_location::host, access_mode::read);
		REQUIRE(h_handle.data != NULL);
		for (int i = 0; i < (int)gpu_array.getNumElements(); i++)
		{
			REQUIRE(h_handle.data[i] == 100 + i + 1);
		}
	}

	{
		ArrayHandle<int> d_handle(gpu_array, access_location::device, access_mode::readwrite);
		REQUIRE(d_handle.data != NULL);

		gpu_add_one(d_handle.data, gpu_array.getNumElements());
		cudaError_t err_sync = cudaGetLastError();
		exec_conf->handleCUDAError(err_sync, __FILE__, __LINE__);
	}

	// and via the readwrite access mode
	{
		ArrayHandle<int> h_handle(gpu_array, access_location::host, access_mode::readwrite);
		REQUIRE(h_handle.data != NULL);
		for (int i = 0; i < (int)gpu_array.getNumElements(); i++)
		{
			REQUIRE(h_handle.data[i] == 100 + i + 1 + 1);
		}
	}
}

//! Tests operations on NULL GlobalArrays
TEST_CASE("GlobalArray_null_tests")
{
	// Construct a NULL GlobalArray
	GlobalArray<int> a;

	REQUIRE(a.isNull());
	REQUIRE(a.getNumElements() == (unsigned)0);

	// check copy construction of a NULL GlobalArray
	GlobalArray<int> b(a);

	REQUIRE(b.isNull());
	REQUIRE(b.getNumElements() == (unsigned)0);

	// check assignment of a NULL GlobalArray
	std::shared_ptr<ExecutionConfiguration> exec_conf(new ExecutionConfiguration(ExecutionConfiguration::GPU));
	GlobalArray<int> c(1000, exec_conf);
	c = a;

	REQUIRE(c.isNull());
	REQUIRE(c.getNumElements() == (unsigned)0);

	// check swapping of a NULL GlobalArray
	GlobalArray<int> d(1000, exec_conf);

	d.swap(a);
	REQUIRE(d.isNull());
	REQUIRE(d.getNumElements() == (unsigned)0);

	REQUIRE(!a.isNull());
	REQUIRE(a.getNumElements() == (unsigned)1000);
}

//! Tests resize methods
TEST_CASE("GlobalArray_resize_tests")
{
	std::shared_ptr<ExecutionConfiguration> exec_conf(new ExecutionConfiguration(ExecutionConfiguration::GPU));

	// create a 1D GlobalArray
	GlobalArray<unsigned int> a(5, exec_conf);

	{
		// Fill it with some values
		ArrayHandle<unsigned int> h_handle(a, access_location::host, access_mode::overwrite);

		h_handle.data[0] = 1;
		h_handle.data[1] = 2;
		h_handle.data[2] = 3;
		h_handle.data[3] = 4;
		h_handle.data[4] = 5;
	}

	// resize the array
	a.resize(10);

	// check that it has the right size
	REQUIRE(a.getNumElements()== (unsigned int)10);
	{
		// test that it still contains the data
		ArrayHandle<unsigned int> h_handle(a, access_location::host, access_mode::read);

		REQUIRE(h_handle.data[0]== (unsigned int)1);
		REQUIRE(h_handle.data[1]== (unsigned int)2);
		REQUIRE(h_handle.data[2]== (unsigned int)3);
		REQUIRE(h_handle.data[3]== (unsigned int)4);
		REQUIRE(h_handle.data[4]== (unsigned int)5);

		// test that the other elements are set to zero
		REQUIRE(h_handle.data[5]== (unsigned int)0);
		REQUIRE(h_handle.data[6]== (unsigned int)0);
		REQUIRE(h_handle.data[7]== (unsigned int)0);
		REQUIRE(h_handle.data[8]== (unsigned int)0);
		REQUIRE(h_handle.data[9]== (unsigned int)0);
	}

	// check that it also works for a GlobalArray that is initially empty
	GlobalArray<unsigned int> b;
	b.resize(7);

	REQUIRE(b.getNumElements()== (unsigned int)7);

	// allocate a 2D GlobalArray
	unsigned int width = 3;
	unsigned int height = 2;
	GlobalArray<unsigned int> c(width, height, exec_conf);
	unsigned int pitch = c.getPitch();
	{
		// write some data to it
		ArrayHandle<unsigned int> h_handle(c, access_location::host, access_mode::overwrite);

		h_handle.data[0] = 123;
		h_handle.data[1] = 456;
		h_handle.data[2] = 789;

		h_handle.data[0 + pitch] = 1234;
		h_handle.data[1 + pitch] = 3456;
		h_handle.data[2 + pitch] = 5678;
	}

	// resize it
	width = 17;
	height = 4;
	c.resize(width, height);
	pitch = c.getPitch();

	// check that it has the right size
	REQUIRE(c.getNumElements() == pitch*height);

	{
		// test that we can still recover the data
		ArrayHandle<unsigned int> h_handle(c, access_location::host, access_mode::read);

		REQUIRE(h_handle.data[0]== (unsigned int)123);
		REQUIRE(h_handle.data[1]== (unsigned int)456);
		REQUIRE(h_handle.data[2]== (unsigned int)789);

		REQUIRE(h_handle.data[0 + pitch]== (unsigned int)1234);
		REQUIRE(h_handle.data[1 + pitch]== (unsigned int)3456);
		REQUIRE(h_handle.data[2 + pitch]== (unsigned int)5678);
	}
}

//! Tests GPUVector
TEST_CASE("GPUVector_basic_tests")
{
	std::shared_ptr<ExecutionConfiguration> exec_conf(new ExecutionConfiguration(ExecutionConfiguration::GPU));

	// First create an empty GPUVector
	GPUVector<unsigned int> vec(exec_conf);

	// The size should be zero
	REQUIRE(vec.size()== (unsigned int)0);

	// Add some elements
	vec.push_back(1);
	vec.push_back(2);
	vec.push_back(3);

	REQUIRE(vec.size()== (unsigned int)3);

	// Test bracket operator
	REQUIRE((unsigned int)vec[0]== (unsigned int)1);
	REQUIRE((unsigned int)vec[1]== (unsigned int)2);
	REQUIRE((unsigned int)vec[2]== (unsigned int)3);

	// Test assignment
	vec[1] = 4;

	REQUIRE((unsigned int)vec[0]== (unsigned int)1);
	REQUIRE((unsigned int)vec[1]== (unsigned int)4);
	REQUIRE((unsigned int)vec[2]== (unsigned int)3);

	// remove an element
	vec.pop_back();

	REQUIRE(vec.size()== (unsigned int)2);

	// clear the array
	vec.clear();

	REQUIRE(vec.size()== (unsigned int)0);

	// resize it
	vec.resize(10);
	REQUIRE(vec.size()== (unsigned int)10);

	// verify we can still write to it
	vec[0] = 234;
	vec[1] = 123;
	vec[2] = 654;
	vec[3] = 789;
	vec[4] = 321;
	vec[5] = 432;
	vec[6] = 543;
	vec[7] = 678;
	vec[8] = 987;
	vec[9] = 890;

	// read back
	REQUIRE((unsigned int)vec[0]== (unsigned int)234);
	REQUIRE((unsigned int)vec[1]== (unsigned int)123);
	REQUIRE((unsigned int)vec[2]== (unsigned int)654);
	REQUIRE((unsigned int)vec[3]== (unsigned int)789);
	REQUIRE((unsigned int)vec[4]== (unsigned int)321);
	REQUIRE((unsigned int)vec[5]== (unsigned int)432);
	REQUIRE((unsigned int)vec[6]== (unsigned int)543);
	REQUIRE((unsigned int)vec[7]== (unsigned int)678);
	REQUIRE((unsigned int)vec[8]== (unsigned int)987);
	REQUIRE((unsigned int)vec[9]== (unsigned int)890);
}
#endif

