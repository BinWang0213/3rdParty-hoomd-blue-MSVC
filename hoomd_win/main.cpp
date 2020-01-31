// this include is necessary to get MPI included before anything else to support intel MPI
#include "ExecutionConfiguration.h"

#include <iostream>

#include <memory>

#include "GlobalArray.h"
#include "GPUVector.h"


#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

using namespace std;



//! test case for testing the basic operation of GPUArray
TEST_CASE("GPUArray_basic_tests")
{
}
