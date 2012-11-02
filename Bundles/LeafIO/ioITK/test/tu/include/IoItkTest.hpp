/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef _IOITK_TEST_TU_IOITKTEST_HPP_
#define _IOITK_TEST_TU_IOITKTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

#include <fwRuntime/EConfigurationElement.hpp>

#include <fwData/Patient.hpp>

namespace ioITK
{
namespace ut
{

/**
 * @brief Test many methods to create mesh.
 */
class IoItkTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( IoItkTest );
    CPPUNIT_TEST( testImageWriterJPG );
    CPPUNIT_TEST( testPatientDBReaderJPG );
    CPPUNIT_TEST( testSaveLoadInr );
    CPPUNIT_TEST( testLoadInr );
    CPPUNIT_TEST( testPatientDBReaderDicom );
    CPPUNIT_TEST( testDicomImageWriter );
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

} //namespace ut
} //namespace ioITK


#endif //_IOITK_TEST_TU_IOITKTEST_HPP_
