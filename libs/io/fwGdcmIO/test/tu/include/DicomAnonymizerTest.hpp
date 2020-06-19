/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include <fwServices/macros.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <gdcmDataSet.h>
#include <gdcmTag.h>

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
    void testDICOMFolder(const std::filesystem::path& srcPath);
    void testAnonymizedFile(const std::filesystem::path& filename);

    std::set< std::string > m_uidContainer;

};

} // namespace ut
} // namespace fwGdcmIO
