/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "DicomDirTest.hpp"

#include <core/log/Logger.hpp>

#include <data/DicomSeries.hpp>

#include <io/dicom/helper/DicomDir.hpp>

#include <utest/Filter.hpp>

#include <utestData/Data.hpp>

#include <boost/algorithm/string/replace.hpp>
#include <boost/foreach.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::ut::DicomDirTest);

namespace sight::io::dicom::ut
{

//------------------------------------------------------------------------------

void DicomDirTest::setUp()
{
    // Set up context before running a test.
    if(utest::Filter::ignoreSlowTests())
    {
        std::cout << std::endl << "Ignoring slow " << std::endl;
    }
    else
    {
        std::cout << std::endl << "Executing slow tests.." << std::endl;
    }
}

//------------------------------------------------------------------------------

void DicomDirTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void DicomDirTest::readDicomDir()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    const std::filesystem::path path = utestData::Data::dir()
                                       / "sight/Patient/Dicom/DicomDB/82-MR-SAGITTAL-KNEE-DICOMDIR";
    const std::string pathStr = boost::algorithm::replace_all_copy(path.string(), "\\", "/");

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    std::vector<data::DicomSeries::sptr> seriesContainer;

    core::log::Logger::sptr logger = core::log::Logger::New();

    // Read DICOMDIR file
    io::dicom::helper::DicomDir::retrieveDicomSeries(path / "DICOMDIR", seriesContainer, logger);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesContainer.size());
    CPPUNIT_ASSERT(logger->empty());

    CPPUNIT_ASSERT_EQUAL(std::size_t(84), seriesContainer.front()->getDicomContainer().size());
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::ut
