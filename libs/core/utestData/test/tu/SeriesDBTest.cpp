/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "SeriesDBTest.hpp"

#include <data/ActivitySeries.hpp>
#include <data/Composite.hpp>
#include <data/Equipment.hpp>
#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>
#include <data/Patient.hpp>
#include <data/Series.hpp>
#include <data/SeriesDB.hpp>
#include <data/String.hpp>
#include <data/Study.hpp>

#include <utestData/generator/SeriesDB.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::utestData::ut::SeriesDBTest);

namespace sight::utestData
{

namespace ut
{

//------------------------------------------------------------------------------

void SeriesDBTest::setUp()
{
}

//------------------------------------------------------------------------------

void SeriesDBTest::tearDown()
{
}

//------------------------------------------------------------------------------

void SeriesDBTest::generationTest()
{
    const unsigned char nbImgSeries      = 3;
    const unsigned char nbModelSeries    = 4;
    const unsigned char nbActivitySeries = 5;
    data::SeriesDB::sptr seriesDB;
    seriesDB = utestData::generator::SeriesDB::createSeriesDB(nbImgSeries, nbModelSeries, nbActivitySeries);

    data::SeriesDB::ContainerType seriesContainer = seriesDB->getContainer();
    CPPUNIT_ASSERT_EQUAL((std::size_t) (nbImgSeries + nbModelSeries + nbActivitySeries), seriesContainer.size());

    CPPUNIT_ASSERT(data::ImageSeries::dynamicCast(seriesContainer[0]));

    data::DicomValuesType performingPhysiciansName;
    performingPhysiciansName.push_back("Dr^Jekyl");
    performingPhysiciansName.push_back("Dr^House");
    performingPhysiciansName.push_back("Dr^Einstein ");

    unsigned char nbIS = 0;
    unsigned char nbMS = 0;
    unsigned char nbAS = 0;

    unsigned int count = 1;
    std::stringstream str;
    for(data::Series::sptr series : seriesContainer)
    {
        str.str("");
        str.width(4);
        str.fill('0');
        str << count++;
        CPPUNIT_ASSERT_EQUAL(
            std::string("1.2.826.0.1.3680043.2.1125.102906542887009256605006409108689" + str.str()),
            series->getInstanceUID()
        );
        CPPUNIT_ASSERT_EQUAL(std::string("CT"), series->getModality());
        CPPUNIT_ASSERT_EQUAL(std::string("20130418"), series->getDate());
        CPPUNIT_ASSERT_EQUAL(std::string("101010.101010 "), series->getTime());
        CPPUNIT_ASSERT_EQUAL(std::string("Description "), series->getDescription());
        CPPUNIT_ASSERT(performingPhysiciansName == series->getPerformingPhysiciansName());

        data::Patient::sptr patient = series->getPatient();
        CPPUNIT_ASSERT_EQUAL(std::string("NomSeriesDB1^PrenomSeriesDB1"), patient->getName());
        CPPUNIT_ASSERT_EQUAL(std::string("4564383757"), patient->getPatientId());
        CPPUNIT_ASSERT_EQUAL(std::string("19710418"), patient->getBirthdate());
        CPPUNIT_ASSERT_EQUAL(std::string("O "), patient->getSex());

        data::Study::sptr study = series->getStudy();
        CPPUNIT_ASSERT_EQUAL(
            std::string("1.2.826.0.1.3680043.2.1125.44278200849347599055201494082232" + str.str()),
            study->getInstanceUID()
        );
        CPPUNIT_ASSERT_EQUAL(std::string("20130418"), study->getDate());
        CPPUNIT_ASSERT_EQUAL(std::string("095948.689872 "), study->getTime());
        CPPUNIT_ASSERT_EQUAL(std::string("Dr^Jekyl"), study->getReferringPhysicianName());
        CPPUNIT_ASSERT_EQUAL(std::string("Say 33. "), study->getDescription());
        CPPUNIT_ASSERT_EQUAL(std::string("042Y"), study->getPatientAge());

        data::Equipment::sptr equipement = series->getEquipment();
        CPPUNIT_ASSERT_EQUAL(std::string("hospital"), equipement->getInstitutionName());

        data::ImageSeries::sptr imgSeries         = data::ImageSeries::dynamicCast(series);
        data::ModelSeries::sptr modelSeries       = data::ModelSeries::dynamicCast(series);
        data::ActivitySeries::sptr activitySeries = data::ActivitySeries::dynamicCast(series);

        if(imgSeries)
        {
            ++nbIS;
            CPPUNIT_ASSERT(imgSeries->getImage());
        }
        else if(modelSeries)
        {
            ++nbMS;
            CPPUNIT_ASSERT(!modelSeries->getReconstructionDB().empty());
        }
        else if(activitySeries)
        {
            ++nbAS;
            CPPUNIT_ASSERT_EQUAL(std::string("IdOfTheConfig"), activitySeries->getActivityConfigId());
            data::Composite::sptr composite = activitySeries->getData();
            CPPUNIT_ASSERT(composite);
            CPPUNIT_ASSERT(composite->find("key1") != composite->end());
            data::String::sptr value = data::String::dynamicCast(composite->getContainer()["key1"]);
            CPPUNIT_ASSERT(value);
            CPPUNIT_ASSERT_EQUAL(std::string("ValueOfKey1"), value->value());
        }
    }

    CPPUNIT_ASSERT_EQUAL(nbImgSeries, nbIS);
    CPPUNIT_ASSERT_EQUAL(nbModelSeries, nbMS);
    CPPUNIT_ASSERT_EQUAL(nbActivitySeries, nbAS);
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::utestData
