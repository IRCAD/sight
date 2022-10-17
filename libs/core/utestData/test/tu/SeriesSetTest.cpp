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

#include "SeriesSetTest.hpp"

#include <data/Activity.hpp>
#include <data/Composite.hpp>
#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>
#include <data/Series.hpp>
#include <data/SeriesSet.hpp>
#include <data/String.hpp>

#include <utestData/generator/SeriesSet.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::utestData::ut::SeriesSetTest);

namespace sight::utestData::ut
{

//------------------------------------------------------------------------------

void SeriesSetTest::setUp()
{
}

//------------------------------------------------------------------------------

void SeriesSetTest::tearDown()
{
}

//------------------------------------------------------------------------------

void SeriesSetTest::generationTest()
{
    const unsigned char nbImgSeries   = 3;
    const unsigned char nbModelSeries = 4;
    auto series_set                   = utestData::generator::SeriesSet::createSeriesSet(nbImgSeries, nbModelSeries);

    CPPUNIT_ASSERT_EQUAL((std::size_t) (nbImgSeries + nbModelSeries), series_set->size());

    CPPUNIT_ASSERT(data::ImageSeries::dynamicCast(series_set->at(0)));

    const std::string performingPhysicianName =
        "Adams^John Robert Quincy^^Rev.^B.A. M.Div.\\Morrison-Jones^Susan^^^Ph.D., Chief Executive Officer\\Doe^John";

    unsigned char nbIS = 0;
    unsigned char nbMS = 0;

    unsigned int count = 1;
    std::stringstream str;
    for(const auto& series : *series_set)
    {
        str.str("");
        str.width(4);
        str.fill('0');
        str << count++;
        CPPUNIT_ASSERT_EQUAL(
            std::string("1.2.826.0.1.3680043.2.1125.102906542887009256605006409108689" + str.str()),
            series->getSeriesInstanceUID()
        );
        CPPUNIT_ASSERT_EQUAL(std::string("CT"), series->getModality());
        CPPUNIT_ASSERT_EQUAL(std::string("20130418"), series->getSeriesDate());
        CPPUNIT_ASSERT_EQUAL(std::string("101010.101010"), series->getSeriesTime());
        CPPUNIT_ASSERT_EQUAL(std::string("Description"), series->getSeriesDescription());
        CPPUNIT_ASSERT(performingPhysicianName == series->getPerformingPhysicianName());

        CPPUNIT_ASSERT_EQUAL(std::string("NomSeriesSet1^PrenomSeriesSet1"), series->getPatientName());
        CPPUNIT_ASSERT_EQUAL(std::string("4564383757"), series->getPatientID());
        CPPUNIT_ASSERT_EQUAL(std::string("19710418"), series->getPatientBirthDate());
        CPPUNIT_ASSERT_EQUAL(std::string("O"), series->getPatientSex());

        CPPUNIT_ASSERT_EQUAL(
            std::string("1.2.826.0.1.3680043.2.1125.44278200849347599055201494082232" + str.str()),
            series->getStudyInstanceUID()
        );
        CPPUNIT_ASSERT_EQUAL(std::string("20130418"), series->getStudyDate());
        CPPUNIT_ASSERT_EQUAL(std::string("095948.689872"), series->getStudyTime());
        CPPUNIT_ASSERT_EQUAL(std::string("Dr^Jekyl"), series->getReferringPhysicianName());
        CPPUNIT_ASSERT_EQUAL(std::string("Say 33."), series->getStudyDescription());
        CPPUNIT_ASSERT_EQUAL(std::string("042Y"), series->getPatientAge());

        CPPUNIT_ASSERT_EQUAL(std::string("IRCAD"), series->getInstitutionName());

        data::ImageSeries::sptr imgSeries   = data::ImageSeries::dynamicCast(series);
        data::ModelSeries::sptr modelSeries = data::ModelSeries::dynamicCast(series);

        if(imgSeries)
        {
            ++nbIS;
            CPPUNIT_ASSERT(imgSeries->getSizeInBytes() > 0);
        }
        else if(modelSeries)
        {
            ++nbMS;
            CPPUNIT_ASSERT(!modelSeries->getReconstructionDB().empty());
        }
    }

    CPPUNIT_ASSERT_EQUAL(nbImgSeries, nbIS);
    CPPUNIT_ASSERT_EQUAL(nbModelSeries, nbMS);
}

} //namespace sight::utestData::ut
