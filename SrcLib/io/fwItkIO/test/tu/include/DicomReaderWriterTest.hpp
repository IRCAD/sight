/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWITKIO_TEST_TU_DICOMREADERWRITERTEST_HPP_
#define _FWITKIO_TEST_TU_DICOMREADERWRITERTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>
#include <fwServices/macros.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwData/Image.hpp>

class DicomReaderWriterTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( DicomReaderWriterTest );
    CPPUNIT_TEST( testWriteImage );
    CPPUNIT_TEST( testWritePatient );
    CPPUNIT_TEST( testWritePatientDB );
    CPPUNIT_TEST( testReadPatientDB );
    CPPUNIT_TEST( testReadWritePatientDB );
    CPPUNIT_TEST( testReadPatientDBACHGenou );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();
    // fonctions de tests
    void testWriteImage();
    void testWritePatient();
    void testWritePatientDB();
    void testReadPatientDB();
    void testReadWritePatientDB();
    void testReadPatientDBACHGenou();

private :

    void writeImage( ::fwData::Image::sptr image );
};

#endif // _FWITKIO_TEST_TU_DICOMREADERWRITERTEST_HPP_
