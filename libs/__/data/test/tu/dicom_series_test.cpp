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

#include "dicom_series_test.hpp"

#include <core/os/temp_path.hpp>
#include <core/type.hpp>

#include <data/array.hpp>

#include <filesystem>
#include <fstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::dicom_seriesTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void dicom_seriesTest::setUp()
{
    // Set up context before running a test.
    m_series = std::make_shared<data::dicom_series>();
}

//------------------------------------------------------------------------------

void dicom_seriesTest::tearDown()
{
    // Clean up after the test run.
    m_series.reset();
}

//------------------------------------------------------------------------------

void dicom_seriesTest::dicomTest()
{
    CPPUNIT_ASSERT(m_series);

    //Create Path
    core::os::temp_file tmp_file;
    std::ofstream file;
    file.open(tmp_file, std::ofstream::out);
    file << "42";
    file.close();

    //None
    const std::size_t nb_instances = 100;
    CPPUNIT_ASSERT(!m_series->isInstanceAvailable(42));
    m_series->setNumberOfInstances(nb_instances);
    CPPUNIT_ASSERT_EQUAL(nb_instances, m_series->numInstances());

    //Paths
    m_series->addDicomPath(42, tmp_file);
    CPPUNIT_ASSERT(m_series->isInstanceAvailable(42));

    //Binaries
    core::memory::buffer_object::sptr buffer_obj = std::make_shared<core::memory::buffer_object>();
    m_series->addBinary(1664, buffer_obj);
    CPPUNIT_ASSERT_EQUAL(buffer_obj, m_series->getDicomContainer().at(1664));
}

//------------------------------------------------------------------------------

void dicom_seriesTest::equalityTest()
{
    auto dicom_series1 = std::make_shared<data::dicom_series>();
    auto dicom_series2 = std::make_shared<data::dicom_series>();

    CPPUNIT_ASSERT(*dicom_series1 == *dicom_series2 && !(*dicom_series1 != *dicom_series2));

    dicom_series1->setNumberOfInstances(666);
    CPPUNIT_ASSERT(*dicom_series1 != *dicom_series2 && !(*dicom_series1 == *dicom_series2));
    dicom_series2->setNumberOfInstances(dicom_series1->numInstances());
    CPPUNIT_ASSERT(*dicom_series1 == *dicom_series2 && !(*dicom_series1 != *dicom_series2));

    dicom_series1->setSOPClassUIDs({"1", "2", "3"});
    CPPUNIT_ASSERT(*dicom_series1 != *dicom_series2 && !(*dicom_series1 == *dicom_series2));
    dicom_series2->setSOPClassUIDs(dicom_series1->getSOPClassUIDs());
    CPPUNIT_ASSERT(*dicom_series1 == *dicom_series2 && !(*dicom_series1 != *dicom_series2));

    dicom_series1->setComputedTagValues({{"4", "4"}, {"5", "5"}, {"6", "6"}});
    CPPUNIT_ASSERT(*dicom_series1 != *dicom_series2 && !(*dicom_series1 == *dicom_series2));
    dicom_series2->setComputedTagValues(dicom_series1->getComputedTagValues());
    CPPUNIT_ASSERT(*dicom_series1 == *dicom_series2 && !(*dicom_series1 != *dicom_series2));

    auto buffer_object = std::make_shared<core::memory::buffer_object>();
    buffer_object->allocate(2);
    auto lock    = buffer_object->lock();
    auto* buffer = reinterpret_cast<char*>(lock.buffer());
    buffer[0] = 'a';
    buffer[1] = '\0';

    dicom_series1->setDicomContainer({{7, buffer_object}});
    CPPUNIT_ASSERT(*dicom_series1 != *dicom_series2 && !(*dicom_series1 == *dicom_series2));
    dicom_series2->setDicomContainer(dicom_series1->getDicomContainer());
    CPPUNIT_ASSERT(*dicom_series1 == *dicom_series2 && !(*dicom_series1 != *dicom_series2));

    dicom_series1->setFirstInstanceNumber(1);
    CPPUNIT_ASSERT(*dicom_series1 != *dicom_series2 && !(*dicom_series1 == *dicom_series2));
    dicom_series2->setFirstInstanceNumber(dicom_series1->getFirstInstanceNumber());
    CPPUNIT_ASSERT(*dicom_series1 == *dicom_series2 && !(*dicom_series1 != *dicom_series2));

    // Test also deepcopy, just for fun
    auto dicom_series3 = std::make_shared<data::dicom_series>();
    dicom_series3->deep_copy(dicom_series1);
    CPPUNIT_ASSERT(*dicom_series1 == *dicom_series3 && !(*dicom_series1 != *dicom_series3));
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut
