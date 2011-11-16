/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWDATA_TU_MESHTEST_H
#define FWDATA_TU_MESHTEST_H

#include <cppunit/extensions/HelperMacros.h>

class MeshTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE( MeshTest );
    CPPUNIT_TEST( allocation );
    CPPUNIT_TEST( insertion );
    CPPUNIT_TEST( colorsNormals );
    CPPUNIT_TEST( addingArray );
    CPPUNIT_TEST( copy );
    CPPUNIT_TEST_SUITE_END();
protected:

public:
    // interface
    void setUp();
    void tearDown();
    // fonctions de tests
    void allocation();
    void insertion();
    void colorsNormals();
    void addingArray();
    void copy();
};
#endif // FWDATA_TU_MESHTEST_H
