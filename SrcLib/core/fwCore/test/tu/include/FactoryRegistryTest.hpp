/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCORE_UT_FATORYREGISTRYTEST_HPP__
#define __FWCORE_UT_FATORYREGISTRYTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwCore
{
namespace ut
{

class FactoryRegistryTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( FactoryRegistryTest );
    CPPUNIT_TEST( pointerTest );
    CPPUNIT_TEST( valueTest );
    CPPUNIT_TEST( argTest );
    CPPUNIT_TEST( threadSafetyTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void pointerTest();
    void valueTest();
    void argTest();
    void threadSafetyTest();

};

} //namespace ut
} //namespace fwCore
#endif //__FWCORE_UT_FATORYREGISTRYTEST_HPP__
