/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSTRUCTURALPATCH_UT_CREATOR_FWMEDDATA_MODELSERIES1TEST_HPP__
#define __FWSTRUCTURALPATCH_UT_CREATOR_FWMEDDATA_MODELSERIES1TEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwStructuralPatch
{
namespace ut
{
namespace creator
{
namespace fwMedData
{

class ModelSeries1Test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ModelSeries1Test );
CPPUNIT_TEST( createTest );
CPPUNIT_TEST( copyTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void createTest();
    void copyTest();

};

} //namespace fwMedData
} //namespace creator
} //namespace ut
} //namespace fwStructuralPatch

#endif //__FWSTRUCTURALPATCH_UT_CREATOR_FWMEDDATA_MODELSERIES1TEST_HPP__

