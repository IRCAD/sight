/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_TEST_TU_OBJECTTEST_HPP_
#define _FWXML_TEST_TU_OBJECTTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

#include <fwData/Object.hpp>

class ObjectTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ObjectTest );
    CPPUNIT_TEST( testColor );
    CPPUNIT_TEST( testMaterial );
    CPPUNIT_TEST( testArray );
    CPPUNIT_TEST( testMesh );
    CPPUNIT_TEST( testPatientDB );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    static ::fwData::Object::sptr serialize(std::string fileName, ::fwData::Object::sptr obj1);

    void testMaterial();
    void testColor();
    void testArray();
    void testMesh();
    void testPatientDB();
};

#endif // _FWXML_TEST_TU_OBJECTTEST_HPP_
