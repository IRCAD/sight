/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_UT_TRANSFORMATIONMATRIX3DTEST_HPP__
#define __FWDATA_UT_TRANSFORMATIONMATRIX3DTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwData
{
namespace ut
{

class TransformationMatrix3DTest : public CPPUNIT_NS::TestFixture
{
private:
    CPPUNIT_TEST_SUITE( TransformationMatrix3DTest );
    CPPUNIT_TEST( getterSetterByCoef );
    CPPUNIT_TEST( getterSetterByArray );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void getterSetterByCoef();
    void getterSetterByArray();
};

} //namespace ut
} //namespace fwData

#endif //__FWDATA_UT_TRANSFORMATIONMATRIX3DTEST_HPP__
