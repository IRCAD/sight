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

#include "DicomSeriesTest.hpp"

#include <core/os/TempPath.hpp>
#include <core/Type.hpp>

#include <data/Array.hpp>

#include <filesystem>
#include <fstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::DicomSeriesTest);

namespace sight::data::ut
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
    core::os::TempFile tmpFile;
    std::ofstream file;
    file.open(tmpFile, std::ofstream::out);
    file << "42";
    file.close();

    //None
    const std::size_t nb_instances = 100;
    CPPUNIT_ASSERT(!m_series->isInstanceAvailable(42));
    m_series->setNumberOfInstances(nb_instances);
    CPPUNIT_ASSERT_EQUAL(nb_instances, m_series->numInstances());

    //Paths
    m_series->addDicomPath(42, tmpFile);
    CPPUNIT_ASSERT(m_series->isInstanceAvailable(42));

    //Binaries
    core::memory::BufferObject::sptr bufferObj = core::memory::BufferObject::New();
    m_series->addBinary(1664, bufferObj);
    CPPUNIT_ASSERT_EQUAL(bufferObj, m_series->getDicomContainer().at(1664));
}

//------------------------------------------------------------------------------

void DicomSeriesTest::equalityTest()
{
    auto dicomSeries1 = data::DicomSeries::New();
    auto dicomSeries2 = data::DicomSeries::New();

    CPPUNIT_ASSERT(*dicomSeries1 == *dicomSeries2 && !(*dicomSeries1 != *dicomSeries2));

    dicomSeries1->setNumberOfInstances(666);
    CPPUNIT_ASSERT(*dicomSeries1 != *dicomSeries2 && !(*dicomSeries1 == *dicomSeries2));
    dicomSeries2->setNumberOfInstances(dicomSeries1->numInstances());
    CPPUNIT_ASSERT(*dicomSeries1 == *dicomSeries2 && !(*dicomSeries1 != *dicomSeries2));

    dicomSeries1->setSOPClassUIDs({"1", "2", "3"});
    CPPUNIT_ASSERT(*dicomSeries1 != *dicomSeries2 && !(*dicomSeries1 == *dicomSeries2));
    dicomSeries2->setSOPClassUIDs(dicomSeries1->getSOPClassUIDs());
    CPPUNIT_ASSERT(*dicomSeries1 == *dicomSeries2 && !(*dicomSeries1 != *dicomSeries2));

    dicomSeries1->setComputedTagValues({{"4", "4"}, {"5", "5"}, {"6", "6"}});
    CPPUNIT_ASSERT(*dicomSeries1 != *dicomSeries2 && !(*dicomSeries1 == *dicomSeries2));
    dicomSeries2->setComputedTagValues(dicomSeries1->getComputedTagValues());
    CPPUNIT_ASSERT(*dicomSeries1 == *dicomSeries2 && !(*dicomSeries1 != *dicomSeries2));

    auto bufferObject = core::memory::BufferObject::New();
    bufferObject->allocate(2);
    auto lock    = bufferObject->lock();
    auto* buffer = reinterpret_cast<char*>(lock.getBuffer());
    buffer[0] = 'a';
    buffer[1] = '\0';

    dicomSeries1->setDicomContainer({{7, bufferObject}});
    CPPUNIT_ASSERT(*dicomSeries1 != *dicomSeries2 && !(*dicomSeries1 == *dicomSeries2));
    dicomSeries2->setDicomContainer(dicomSeries1->getDicomContainer());
    CPPUNIT_ASSERT(*dicomSeries1 == *dicomSeries2 && !(*dicomSeries1 != *dicomSeries2));

    dicomSeries1->setFirstInstanceNumber(1);
    CPPUNIT_ASSERT(*dicomSeries1 != *dicomSeries2 && !(*dicomSeries1 == *dicomSeries2));
    dicomSeries2->setFirstInstanceNumber(dicomSeries1->getFirstInstanceNumber());
    CPPUNIT_ASSERT(*dicomSeries1 == *dicomSeries2 && !(*dicomSeries1 != *dicomSeries2));

    // Test also deepcopy, just for fun
    auto dicomSeries3 = data::DicomSeries::New();
    dicomSeries3->deepCopy(dicomSeries1);
    CPPUNIT_ASSERT(*dicomSeries1 == *dicomSeries3 && !(*dicomSeries1 != *dicomSeries3));
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut
