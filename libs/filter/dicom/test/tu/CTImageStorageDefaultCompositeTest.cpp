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

#include "CTImageStorageDefaultCompositeTest.hpp"

#include <filter/dicom/factory/new.hpp>
#include <filter/dicom/filter.hpp>
#include <filter/dicom/helper/Filter.hpp>

#include <io/dicom/reader/series_set.hpp>

#include <utest_data/Data.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::dicom::ut::CTImageStorageDefaultCompositeTest);

namespace sight::filter::dicom::ut
{

//------------------------------------------------------------------------------

void CTImageStorageDefaultCompositeTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void CTImageStorageDefaultCompositeTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void CTImageStorageDefaultCompositeTest::simpleApplication()
{
    auto series_set = std::make_shared<data::series_set>();

    const std::string filename       = "08-CT-PACS";
    const std::filesystem::path path = utest_data::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    // Read DicomSeries
    auto reader = std::make_shared<io::dicom::reader::series_set>();
    reader->set_object(series_set);
    reader->set_folder(path);
    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

    // Retrieve DicomSeries
    data::dicom_series::sptr dicomSeriesA = std::dynamic_pointer_cast<data::dicom_series>((*series_set)[0]);
    data::dicom_series::sptr dicomSeriesB = std::make_shared<data::dicom_series>();
    dicomSeriesB->deep_copy(dicomSeriesA);
    CPPUNIT_ASSERT(dicomSeriesA);
    CPPUNIT_ASSERT(dicomSeriesB);
    std::vector<data::dicom_series::sptr> dicomSeriesContainerA;
    std::vector<data::dicom_series::sptr> dicomSeriesContainerB;
    dicomSeriesContainerA.push_back(dicomSeriesA);
    dicomSeriesContainerB.push_back(dicomSeriesB);

    // Apply composite filter on the first DicomSeries
    sight::filter::dicom::filter::sptr filter = sight::filter::dicom::factory::make(
        "sight::filter::dicom::composite::CTImageStorageDefaultComposite"
    );
    CPPUNIT_ASSERT(filter);
    sight::filter::dicom::helper::Filter::applyFilter(dicomSeriesContainerA, filter, true);

    // Apply other filters on the second DicomSeries
    filter = sight::filter::dicom::factory::make("sight::filter::dicom::splitter::AcquisitionNumberSplitter");
    CPPUNIT_ASSERT(filter);
    sight::filter::dicom::helper::Filter::applyFilter(dicomSeriesContainerB, filter, true);
    filter = sight::filter::dicom::factory::make("sight::filter::dicom::sorter::InstanceNumberSorter");
    CPPUNIT_ASSERT(filter);
    sight::filter::dicom::helper::Filter::applyFilter(dicomSeriesContainerB, filter, true);
    filter = sight::filter::dicom::factory::make("sight::filter::dicom::sorter::ImagePositionPatientSorter");
    CPPUNIT_ASSERT(filter);
    sight::filter::dicom::helper::Filter::applyFilter(dicomSeriesContainerB, filter, true);
    filter = sight::filter::dicom::factory::make("sight::filter::dicom::splitter::ImagePositionPatientSplitter");
    CPPUNIT_ASSERT(filter);
    sight::filter::dicom::helper::Filter::applyFilter(dicomSeriesContainerB, filter, true);
    filter = sight::filter::dicom::factory::make("sight::filter::dicom::sorter::ImagePositionPatientSorter");
    CPPUNIT_ASSERT(filter);
    sight::filter::dicom::helper::Filter::applyFilter(dicomSeriesContainerB, filter, true);
    filter = sight::filter::dicom::factory::make("sight::filter::dicom::modifier::SliceThicknessModifier");
    CPPUNIT_ASSERT(filter);
    sight::filter::dicom::helper::Filter::applyFilter(dicomSeriesContainerB, filter, true);

    // Compare the two series
    CPPUNIT_ASSERT_EQUAL(dicomSeriesContainerA.size(), dicomSeriesContainerB.size());
    for(unsigned int j = 0 ; j < dicomSeriesContainerA.size() ; ++j) // For every series
    {
        for(unsigned int i = 0 ; i < dicomSeriesContainerA[j]->getDicomContainer().size() ; ++i) // For every instances
        {
            CPPUNIT_ASSERT(
                std::memcmp(
                    dicomSeriesContainerA[j]->getDicomContainer().at(i)->buffer(),
                    dicomSeriesContainerB[j]->getDicomContainer().at(i)->buffer(),
                    dicomSeriesContainerA[j]->getDicomContainer().at(i)->size()
                ) == 0
            );
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::filter::dicom::ut
