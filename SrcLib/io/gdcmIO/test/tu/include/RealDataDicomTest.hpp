/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_TEST_TU_REALDATADICOMTEST_HPP_
#define _GDCMIO_TEST_TU_REALDATADICOMTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>
#include <fwServices/macros.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwData/Image.hpp>

namespace gdcmIO
{
namespace ut
{

class RealDataDicomTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( RealDataDicomTest );
    CPPUNIT_TEST( testWriteImage );
    CPPUNIT_TEST( testReadPatientDB );
    CPPUNIT_TEST( testReadPatientDBACHGenou );
//    CPPUNIT_TEST( testWritePatient );
//    CPPUNIT_TEST( testWritePatientDB );
//    CPPUNIT_TEST( testReadWritePatientDB );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();
    // fonctions de tests
    void testWriteImage();
    void testReadPatientDB();
    void testReadPatientDBACHGenou();

//    void testWritePatient();
//    void testWritePatientDB();
//    void testReadWritePatientDB();

private :

    void writeImage( ::fwData::Image::sptr image );

    std::stringstream m_gdcmTraceStream;
};


} // namespace ut
} // namespace gdcmIO

#endif // _GDCMIO_TEST_TU_REALDATADICOMTEST_HPP_
