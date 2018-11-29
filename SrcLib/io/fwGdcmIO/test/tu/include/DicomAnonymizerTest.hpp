/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWGDCMIO_UT_DICOMANONYMIZERTEST_HPP__
#define __FWGDCMIO_UT_DICOMANONYMIZERTEST_HPP__

#include <fwServices/macros.hpp>

#include <gdcmTag.h>
#include <gdcmDataSet.h>

#include <cppunit/extensions/HelperMacros.h>

namespace fwGdcmIO
{
namespace ut
{

class DicomAnonymizerTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( DicomAnonymizerTest );
CPPUNIT_TEST( anonymizeImageSeriesTest );
CPPUNIT_TEST( anonymizeDICOMTest );
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    /// Test anonymization of ImageSeries
    void anonymizeImageSeriesTest();

    /// Test anonymisation of DICOM folder
    void anonymizeDICOMTest();

private:
    void testDICOMFolder(const ::boost::filesystem::path& srcPath);
    void testAnonymizedFile(const ::boost::filesystem::path& filename);

    std::set< std::string > m_uidContainer;

};

} // namespace ut
} // namespace fwGdcmIO

#endif // __FWGDCMIO_UT_DICOMANONYMIZERTEST_HPP__
