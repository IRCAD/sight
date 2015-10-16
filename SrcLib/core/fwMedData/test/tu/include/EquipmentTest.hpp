/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_UT_EQUIPMENTTEST_HPP__
#define __FWMEDDATA_UT_EQUIPMENTTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <fwMedData/Equipment.hpp>

namespace fwMedData
{

namespace ut
{

class EquipmentTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( EquipmentTest );
CPPUNIT_TEST(institutionNameTest);
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void institutionNameTest();

private:
    Equipment::sptr m_equipment;
};

} //namespace ut
} //namespace fwMedData

#endif //__FWMEDDATA_UT_EQUIPMENTTEST_HPP__

