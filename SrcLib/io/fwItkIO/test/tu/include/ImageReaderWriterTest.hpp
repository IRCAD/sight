/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWITKIO_TEST_TU_IMAGEREADERWRITERTEST_HPP_
#define _FWITKIO_TEST_TU_IMAGEREADERWRITERTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>
#include <fwServices/macros.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwData/Image.hpp>

class ImageReaderWriterTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ImageReaderWriterTest );
    CPPUNIT_TEST( methode1 );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();
    // fonctions de tests
    void methode1();

private:
    ::fwData::Image::sptr createImage();

};

#endif // _FWITKIO_TEST_TU_IMAGEREADERWRITERTEST_HPP_
