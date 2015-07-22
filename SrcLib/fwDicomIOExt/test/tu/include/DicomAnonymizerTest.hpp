/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_UT_DICOMANONYMIZERTEST_HPP__
#define __FWDICOMIOEXT_UT_DICOMANONYMIZERTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>
#include <fwServices/macros.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

namespace fwDicomIOExt
{
namespace ut
{

class DicomAnonymizerTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( DicomAnonymizerTest );
CPPUNIT_TEST( anonymizeDicomFolderTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void anonymizeDicomFolderTest();

};

} // namespace ut
} // namespace fwDicomIOExt

#endif // __FWDICOMIOEXT_UT_DICOMANONYMIZERTEST_HPP__
