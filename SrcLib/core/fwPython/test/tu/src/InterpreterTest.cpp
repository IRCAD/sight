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

#include <fwData/String.hpp>

#include "fwPython/Interpreter.hpp"
#include "fwPython/bindings.hpp"
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

void InterpreterTest::syntaxError()
{
    fwPython::Interpreter interpreter;

    int succes = interpreter.execute("b=[1,2]\n"
                                     "a = [1,2,3;]");
    CPPUNIT_ASSERT_EQUAL( succes , -1 );
}



void InterpreterTest::printClassName()
{
    fwPython::Interpreter interpreter;
    int succes;
    
    ::fwPython::initBindings();
    succes = interpreter.execute("import fwData");

    ::fwData::String::sptr str =  ::fwData::String::New(" a string in fwData");
    interpreter.addObject("myObject", str );

    succes = interpreter.execute("print \"print myObject ==>\" , myObject");
    succes = interpreter.execute("print \"myObject.getClassname() => \" , myObject.getClassname()");

    CPPUNIT_ASSERT_EQUAL( succes , 0 );
}
