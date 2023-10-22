/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "series_set_test.hpp"

#include <data/activity.hpp>
#include <data/composite.hpp>
#include <data/image_series.hpp>
#include <data/model_series.hpp>
#include <data/series.hpp>
#include <data/series_set.hpp>
#include <data/string.hpp>

#include <utest_data/generator/series_set.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::utest_data::ut::series_set_test);

namespace sight::utest_data::ut
{

//------------------------------------------------------------------------------

void series_set_test::setUp()
{
}

//------------------------------------------------------------------------------

void series_set_test::tearDown()
{
}

//------------------------------------------------------------------------------

void series_set_test::generationTest()
{
    const unsigned char nb_img_series   = 3;
    const unsigned char nb_model_series = 4;
    auto series_set                     = utest_data::generator::series_set::createSeriesSet(
        nb_img_series,
        nb_model_series
    );

    CPPUNIT_ASSERT_EQUAL((std::size_t) (nb_img_series + nb_model_series), series_set->size());

    CPPUNIT_ASSERT(std::dynamic_pointer_cast<data::image_series>(series_set->at(0)));

    const std::string performing_physician_name =
        "Adams^John Robert Quincy^^Rev.^B.A. M.Div.\\Morrison-Jones^Susan^^^Ph.D., Chief Executive Officer\\Doe^John";

    unsigned char nb_is = 0;
    unsigned char nb_ms = 0;

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
        CPPUNIT_ASSERT(performing_physician_name == series->getPerformingPhysicianName());

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

        data::image_series::sptr img_series   = std::dynamic_pointer_cast<data::image_series>(series);
        data::model_series::sptr model_series = std::dynamic_pointer_cast<data::model_series>(series);

        if(img_series)
        {
            ++nb_is;
            CPPUNIT_ASSERT(img_series->getSizeInBytes() > 0);
        }
        else if(model_series)
        {
            ++nb_ms;
            CPPUNIT_ASSERT(!model_series->getReconstructionDB().empty());
        }
    }

    CPPUNIT_ASSERT_EQUAL(nb_img_series, nb_is);
    CPPUNIT_ASSERT_EQUAL(nb_model_series, nb_ms);
}

} //namespace sight::utest_data::ut
