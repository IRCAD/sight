/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <service/macros.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <gdcmDataSet.h>
#include <gdcmTag.h>

namespace sight::io::dicom::ut
{

class dicom_anonymizer_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(dicom_anonymizer_test);
CPPUNIT_TEST(anonymize_image_series_test);
CPPUNIT_TEST(anonymize_dicom_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    /// Test anonymization of ImageSeries
    static void anonymize_image_series_test();

    /// Test anonymisation of DICOM folder
    void anonymize_dicom_test();

private:

    void test_dicom_folder(const std::filesystem::path& _src_path);
    void test_anonymized_file(const std::filesystem::path& _filename);

    std::set<std::string> m_uid_container;
};

} // namespace sight::io::dicom::ut
