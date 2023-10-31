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

#include "dicom_dir_test.hpp"

#include <core/log/logger.hpp>

#include <data/dicom_series.hpp>

#include <io/dicom/helper/dicom_dir.hpp>

#include <utest/filter.hpp>

#include <utest_data/data.hpp>

#include <boost/algorithm/string/replace.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::ut::dicom_dir_test);

namespace sight::io::dicom::ut
{

//------------------------------------------------------------------------------

void dicom_dir_test::setUp()
{
    // Set up context before running a test.
    if(utest::filter::ignore_slow_tests())
    {
        std::cout << std::endl << "Ignoring slow " << std::endl;
    }
    else
    {
        std::cout << std::endl << "Executing slow tests.." << std::endl;
    }
}

//------------------------------------------------------------------------------

void dicom_dir_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void dicom_dir_test::read_dicom_dir()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    const std::filesystem::path path = utest_data::dir()
                                       / "sight/Patient/Dicom/DicomDB/82-MR-SAGITTAL-KNEE-DICOMDIR";
    const std::string path_str = boost::algorithm::replace_all_copy(path.string(), "\\", "/");

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    std::vector<data::dicom_series::sptr> series_container;

    core::log::logger::sptr logger = std::make_shared<core::log::logger>();

    // Read DICOMDIR file
    io::dicom::helper::dicom_dir::retrieve_dicom_series(path / "DICOMDIR", series_container, logger);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_container.size());
    CPPUNIT_ASSERT(logger->empty());

    CPPUNIT_ASSERT_EQUAL(std::size_t(84), series_container.front()->get_dicom_container().size());
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::ut
