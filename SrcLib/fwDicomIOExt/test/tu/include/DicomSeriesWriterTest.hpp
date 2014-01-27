/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_TEST_TU_DICOMSERIESWRITERTEST_HPP__
#define __FWDICOMIOEXT_TEST_TU_DICOMSERIESWRITERTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>
#include <fwServices/macros.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

namespace fwDicomIOExt
{
namespace ut
{

class DicomSeriesWriterTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( DicomSeriesWriterTest );
    CPPUNIT_TEST( writeReadTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void writeReadTest();

};

} // namespace ut
} // namespace fwDicomIOExt

#endif // __FWDICOMIOEXT_TEST_TU_DICOMSERIESWRITERTEST_HPP__
