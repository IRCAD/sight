/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOGDCM_UT_IOGDCMTEST_HPP__
#define __IOGDCM_UT_IOGDCMTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <fwRuntime/EConfigurationElement.hpp>

namespace ioGdcm
{
namespace ut
{

/**
 * @brief Test GDCM reader/writer for SeriesDB.
 */
class IoGdcmTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( IoGdcmTest );
    CPPUNIT_TEST( testSeriesDBReader );
    CPPUNIT_TEST( testSeriesWriter );
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();


    void testSeriesDBReader();
    void testSeriesWriter();

private :

    void executeService( ::fwData::Object::sptr obj, std::string srvType, std::string srvImpl, ::fwRuntime::EConfigurationElement::sptr cfg );

    std::stringstream m_gdcmTraceStream;
};

} // namespace ut
} // namespace ioGdcm

#endif //__IOGDCM_UT_IOGDCMTEST_HPP__
