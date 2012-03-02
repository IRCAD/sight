/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_UT_ACQUISITIONTEST_HPP__
#define __FWDATA_UT_ACQUISITIONTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwData
{
namespace ut
{

class AcquisitionTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( AcquisitionTest );
    CPPUNIT_TEST( methode1 );
    CPPUNIT_TEST( methode2 );
    CPPUNIT_TEST( Image) ;
    CPPUNIT_TEST( StructAnat ) ;
    CPPUNIT_TEST( Reconstruction ) ;
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void methode1();
    void methode2();
    void Image() ;
    void StructAnat();
    void Reconstruction();
};

} //namespace ut
} //namespace fwData
#endif __FWDATA_UT_ACQUISITIONTEST_HPP__
