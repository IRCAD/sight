/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef DATA_INTERPRETERTEST_H
#define DATA_INTERPRETERTEST_H

#include <cppunit/extensions/HelperMacros.h>


class InterpreterTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE( InterpreterTest );
    CPPUNIT_TEST( helloWorld );
    CPPUNIT_TEST( fail );
    CPPUNIT_TEST( import );
    CPPUNIT_TEST( importFwDataBindings );
    CPPUNIT_TEST( testFwDataBindings );
    CPPUNIT_TEST( changeValue );
    CPPUNIT_TEST_SUITE_END();
protected:

public:
    InterpreterTest();
    // interface
    void setUp();
    void tearDown();
    // fonctions de tests
    void helloWorld();
    void fail();
    void import();
    void importFwDataBindings();
    void testFwDataBindings();
    void changeValue();
};


#endif
