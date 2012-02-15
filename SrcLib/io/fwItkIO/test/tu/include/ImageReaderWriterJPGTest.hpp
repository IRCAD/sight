/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWITKIO_TEST_TU_IMAGEREADERWRITERJPGTEST_HPP_
#define _FWITKIO_TEST_TU_IMAGEREADERWRITERJPGTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>
#include <fwServices/macros.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwData/Image.hpp>

class ImageReaderWriterJPGTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ImageReaderWriterJPGTest );
    CPPUNIT_TEST( testImageWriter );
    CPPUNIT_TEST( testImageWriter2 );
    CPPUNIT_TEST( testPatientDBReader );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    // fonctions de tests
    void testImageWriter();
    void testImageWriter2();
    void testPatientDBReader();

};


#endif // _FWITKIO_TEST_TU_IMAGEREADERWRITERJPGTEST_HPP_
