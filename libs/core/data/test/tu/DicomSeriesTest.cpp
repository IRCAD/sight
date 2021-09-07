/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "DicomSeriesTest.hpp"

#include <core/tools/System.hpp>
#include <core/tools/Type.hpp>

#include <data/Array.hpp>

#include <filesystem>
#include <fstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::DicomSeriesTest);

namespace sight::data
{

namespace ut
{

//------------------------------------------------------------------------------

void DicomSeriesTest::setUp()
{
    // Set up context before running a test.
    m_series = data::DicomSeries::New();
}

//------------------------------------------------------------------------------

void DicomSeriesTest::tearDown()
{
    // Clean up after the test run.
    m_series.reset();
}

//------------------------------------------------------------------------------

void DicomSeriesTest::dicomTest()
{
    CPPUNIT_ASSERT(m_series);

    //Create Path
    const std::filesystem::path path = core::tools::System::getTemporaryFolder() / "dicomtest";
    std::filesystem::create_directories(path);
    const std::string filename = path.string() + "/" + "file";
    std::ofstream file;
    file.open(filename.c_str(), std::ofstream::out);
    file << "42";
    file.close();

    //None
    const size_t nbInstances = 100;
    CPPUNIT_ASSERT(!m_series->isInstanceAvailable(42));
    m_series->setNumberOfInstances(nbInstances);
    CPPUNIT_ASSERT_EQUAL(nbInstances, m_series->getNumberOfInstances());

    //Paths
    m_series->addDicomPath(42, filename);
    CPPUNIT_ASSERT(m_series->isInstanceAvailable(42));

    //Binaries
    core::memory::BufferObject::sptr bufferObj = core::memory::BufferObject::New();
    m_series->addBinary(1664, bufferObj);
    CPPUNIT_ASSERT_EQUAL(bufferObj, m_series->getDicomContainer().at(1664));
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::data
