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
    CPPUNIT_TEST( testStructureTraits );
    CPPUNIT_TEST( testROITraits );
    CPPUNIT_TEST( testReconstructionTraits );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    static ::fwData::Object::sptr serializeOldVersion(std::string fileName, ::fwData::Object::sptr obj1);

    void testStructureTraits();
    void testROITraits();
    void testReconstructionTraits();
};

#endif // _FWXML_TEST_TU_OBJECTTEST_HPP_
