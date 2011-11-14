/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWDATA_TU_ARRAYTEST_H
#define FWDATA_TU_ARRAYTEST_H

#include <cppunit/extensions/HelperMacros.h>

class ArrayTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE( ArrayTest );
    CPPUNIT_TEST( allocation );
    CPPUNIT_TEST( resize );
    CPPUNIT_TEST_SUITE_END();
protected:

public:
    // interface
    void setUp();
    void tearDown();
    // fonctions de tests
    void allocation();
    void resize();
};
#endif // FWDATA_TU_ARRAYTEST_H
