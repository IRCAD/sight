/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <exception>
#include <vector>
#include <ostream>
#include <map>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "fwPython/Interpreter.hpp"
#include "InterpreterTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( InterpreterTest );

void InterpreterTest::setUp()
{
    // Set up context before running a test.

}
void InterpreterTest::tearDown()
{
    // Clean up after the test run.
}

void InterpreterTest::printHelloWorld()
{
    fwPython::Interpreter interpreter;

    int succes = interpreter.execute("print \"hello world\"");
    CPPUNIT_ASSERT_EQUAL( succes , 0 );
}



