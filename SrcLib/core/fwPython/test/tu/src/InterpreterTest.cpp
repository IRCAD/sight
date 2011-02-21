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
#include <boost/assign/std/vector.hpp>
#include <boost/foreach.hpp>

#include <fwData/String.hpp>

#include "fwPython/python.hpp"
#include "fwPython/bindings/base.hpp"
#include "fwPython/Interpreter.hpp"
#include "InterpreterTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( InterpreterTest );

InterpreterTest::InterpreterTest()
{
    ::fwPython::setHome("./PythonHome");
    ::fwPython::initialize();
    ::fwPython::bindings::initialize();
}

void InterpreterTest::setUp()
{
    // Set up context before running a test.
}

void InterpreterTest::tearDown()
{
    ::fwPython::finalize();
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void InterpreterTest::helloWorld()
{
    fwPython::Interpreter interpreter;

    int succes;

    succes = interpreter.execute(
            "a = 'hello world'\n"
            );

    CPPUNIT_ASSERT_EQUAL( succes , 0 );
}

//-----------------------------------------------------------------------------

void InterpreterTest::fail()
{
    fwPython::Interpreter interpreter;

    int succes;

    succes = interpreter.execute(
            "assert 0\n"
            );

    CPPUNIT_ASSERT_EQUAL( succes , -1 );
}


//-----------------------------------------------------------------------------

void InterpreterTest::import()
{
    fwPython::Interpreter interpreter;

    int succes;

    succes = interpreter.execute(
            "import os\n"
            "assert os\n"
            );

    CPPUNIT_ASSERT_EQUAL( succes , 0 );
}



//-----------------------------------------------------------------------------

void InterpreterTest::importFwDataBindings()
{
    fwPython::Interpreter interpreter;

    int succes;
    succes = interpreter.execute(
            "import fwData\n"
            );

    CPPUNIT_ASSERT_EQUAL( succes , 0 );
}

//-----------------------------------------------------------------------------

void InterpreterTest::testFwDataBindings()
{
    fwPython::Interpreter interpreter;
    int succes;


    //::fwData::Object::sptr dataObj   =  ::fwData::String::New("fwData::String as Data::Object");
    //::fwTools::Object::sptr toolsObj =  ::fwData::String::New("fwData::String as fwTools::Object");

    //::fwData::String::sptr str =  ::fwData::String::New("a string in fwData");

    using namespace boost::assign;
    std::vector<std::string> classes;
    classes += "String", "Float", "Integer", "Boolean";


    std::string base = "::fwData::";
    BOOST_FOREACH(std::string name, classes)
    {
        std::string klass = base + name;
        interpreter.addObject(name, ::fwTools::Factory::New(klass) );

        std::string code = std::string();
        code += "assert " + name + "\n";
        code += "assert repr(" + name + ").startswith('<fwData." + name + " object at ')\n";
        code += "assert " + name + ".getClassname() == '" + klass + "'\n";

        succes = interpreter.execute( code );
        CPPUNIT_ASSERT_EQUAL( succes , 0 );
    }

}

//-----------------------------------------------------------------------------

void InterpreterTest::changeValue()
{
    fwPython::Interpreter interpreter;

    int succes;

    ::fwData::String::sptr str =  ::fwData::String::New("a string in fwData");
    interpreter.addObject("myStr", str );

    succes = interpreter.execute(
            "assert myStr.value == 'a string in fwData'\n"
            "myStr.value = 'changed value in python'\n"
            );

    CPPUNIT_ASSERT_EQUAL( succes , 0 );
    CPPUNIT_ASSERT_EQUAL( str->value() , std::string("changed value in python") );

}
