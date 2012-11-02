/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOGDCM_UT_IOGDCMTEST_HPP__
#define __IOGDCM_UT_IOGDCMTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <fwRuntime/EConfigurationElement.hpp>

#include <fwData/Patient.hpp>

namespace ioGdcm
{
namespace ut
{

/**
 * @brief Test many methods to create mesh.
 */
class IoGdcmTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( IoGdcmTest );
    CPPUNIT_TEST( testPatientDBReaderDicom );
    CPPUNIT_TEST( testDicomPatientWriter );
    CPPUNIT_TEST( testDicomPatientDBWriter );
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    void testImageWriterJPG();
    void testPatientDBReaderJPG();
    void testSaveLoadInr();
    void testLoadInr();
    void testPatientDBReaderDicom();

    void testDicomImageWriter();
    void testDicomPatientWriter();
    void testDicomPatientDBWriter();

private :

    void executeService( ::fwData::Object::sptr obj, std::string srvType, std::string srvImpl, ::fwRuntime::EConfigurationElement::sptr cfg );
};

} // namespace ut
} // namespace ioGdcm

#endif //__IOGDCM_UT_IOGDCMTEST_HPP__
