// Copyright (c) 2009-2019 The Regents of the University of Michigan
// This file is part of the HOOMD-blue project, released under the BSD 3-Clause License.


// this include is necessary to get MPI included before anything else to support intel MPI
#include "ExecutionConfiguration.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include "hoomd/Messenger.h"

using namespace std;


/*! \file test_messenger.cc
	\brief Unit test for Messenger
	\ingroup unit_tests
*/


#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>


TEST_CASE("Messenger_basic")
{
	Messenger msg;

	// check that the default streams are set as documented
	REQUIRE(&(msg.error())== &cerr);
	REQUIRE(&(msg.warning())== &cerr);
	REQUIRE(msg.getNoticeLevel() == (unsigned int)2);
	REQUIRE(&(msg.notice(0))== &cout);
	REQUIRE(&(msg.notice(1))== &cout);
	REQUIRE(&(msg.notice(2))== &cout);
	REQUIRE(&(msg.notice(3))== &(msg.getNullStream()));
	cout << endl;

	msg.setNoticeLevel(10);
	REQUIRE(msg.getNoticeLevel() == (unsigned int)10);
}


TEST_CASE("Messenger_print")
{
	Messenger msg;

	// try a few test messages (must be determined by hand runs)
	msg.error() << "This is an error message test " << 1 << " " << 786.345 << endl;
	msg.warning() << "This is a warning message test" << endl;
	msg.notice(9) << "This notice message should not be printed" << endl;
	msg.setNoticeLevel(10);
	msg.notice(9) << "And this one should" << endl;
	msg.notice(1) << "Notice level 1" << endl;
}

TEST_CASE("Messenger_null")
{
	Messenger msg;

	// try setting the null streams
	msg.error() << "This message should be printed" << endl;
	msg.setErrorStream(msg.getNullStream());
	msg.error() << "But this one should not" << endl;

	msg.warning() << "This message should be printed" << endl;
	msg.setWarningStream(msg.getNullStream());
	msg.warning() << "But this one should not" << endl;

	msg.notice(1) << "This message should be printed" << endl;
	msg.setNoticeStream(msg.getNullStream());
	msg.notice(1) << "But this one should not" << endl;
}

TEST_CASE("Messenger_prefix")
{
	Messenger msg;

	// check that set/get work on prefixes
	msg.setErrorPrefix("err");
	REQUIRE(msg.getErrorPrefix()== string("err"));
	msg.setWarningPrefix("warn");
	REQUIRE(msg.getWarningPrefix()== string("warn"));
	msg.setNoticePrefix("note");
	REQUIRE(msg.getNoticePrefix()== string("note"));

	ostringstream strm;
	// check that the prefixes are used properly
	msg.setErrorStream(strm);
	msg.error() << "message";
	REQUIRE(strm.str()== string("err: message"));

	strm.str("");
	msg.setWarningStream(strm);
	msg.warning() << "message";
	REQUIRE(strm.str()== string("warn: message"));

	strm.str("");
	msg.setNoticeStream(strm);
	msg.notice(1) << "message";
	REQUIRE(strm.str()== string("message"));

	strm.str("");
	msg.setNoticeStream(strm);
	msg.notice(5) << "message";
	REQUIRE(strm.str()== string(""));
	msg.setNoticeLevel(5);
	msg.notice(5) << "message";
	REQUIRE(strm.str()== string("note(5): message"));

	// try copying a messenger and make sure that it still works
	strm.str("");
	Messenger msg2;
	msg2 = msg;
	msg2.error() << "1" << endl;
	msg.warning() << "2" << endl; // intentional use of msg to see if it merges output as it should
	msg2.notice(1) << "3" << endl;
	msg2.notice(5) << "4" << endl;
	msg2.notice(6) << "5" << endl;
	REQUIRE(strm.str()== string("err: 1\nwarn: 2\n3\nnote(5): 4\n"));
}

TEST_CASE("Messenger_file")
{
	// scope the messengers so that the file is closed and written
	{
		Messenger msg;

		msg.setErrorPrefix("err");
		msg.setWarningPrefix("warn");
		msg.setNoticePrefix("note");

		// test opening a file
		msg.openFile("test_messenger_output");

		// also test that we can write to the file from 2 messengers
		Messenger msg2(msg);
		msg2.setNoticeLevel(5);

		// write the file (also tests the *Str functions)
		msg.errorStr("Error 1\n");
		msg2.warningStr("Warning 2\n");
		msg.noticeStr(1, "Notice 3\n");
		msg.noticeStr(5, "This shouldn't be printed\n");
		msg2.noticeStr(5, "Notice 4\n");
	}

	// make sure the file was created
	//REQUIRE(filesystem::exists("test_messenger_output"));

	// read in the file and make sure correct data was written
	ifstream f("test_messenger_output");
	string line;

	getline(f, line);
	REQUIRE(line == "err: Error 1");
	REQUIRE(!f.bad());

	getline(f, line);
	REQUIRE(line == "warn: Warning 2");
	REQUIRE(!f.bad());

	getline(f, line);
	REQUIRE(line == "Notice 3");
	REQUIRE(!f.bad());

	getline(f, line);
	REQUIRE(line == "note(5): Notice 4");
	REQUIRE(!f.bad());
	f.close();

	unlink("test_messenger_output");
}
