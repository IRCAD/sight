/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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
#include <data/image_series.hpp>
#include <data/map.hpp>
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

void series_set_test::generation_test()
{
    const unsigned char nb_img_series   = 3;
    const unsigned char nb_model_series = 4;
    auto series_set                     = utest_data::generator::series_set::create_series_set(
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
            series->get_series_instance_uid()
        );
        CPPUNIT_ASSERT_EQUAL(data::dicom::modality_t::ct, series->get_modality());
        CPPUNIT_ASSERT_EQUAL(std::string("20130418"), series->get_series_date());
        CPPUNIT_ASSERT_EQUAL(std::string("101010.101010"), series->get_series_time());
        CPPUNIT_ASSERT_EQUAL(std::string("Description"), series->get_series_description());
        CPPUNIT_ASSERT(performing_physician_name == series->get_performing_physician_name());

        CPPUNIT_ASSERT_EQUAL(std::string("NomSeriesSet1^PrenomSeriesSet1"), series->get_patient_name());
        CPPUNIT_ASSERT_EQUAL(std::string("4564383757"), series->get_patient_id());
        CPPUNIT_ASSERT_EQUAL(std::string("19710418"), series->get_patient_birth_date());
        CPPUNIT_ASSERT_EQUAL(std::string("O"), series->get_patient_sex());

        CPPUNIT_ASSERT_EQUAL(
            std::string("1.2.826.0.1.3680043.2.1125.44278200849347599055201494082232" + str.str()),
            series->get_study_instance_uid()
        );
        CPPUNIT_ASSERT_EQUAL(std::string("20130418"), series->get_study_date());
        CPPUNIT_ASSERT_EQUAL(std::string("095948.689872"), series->get_study_time());
        CPPUNIT_ASSERT_EQUAL(std::string("Dr^Jekyl"), series->get_referring_physician_name());
        CPPUNIT_ASSERT_EQUAL(std::string("Say 33."), series->get_study_description());
        CPPUNIT_ASSERT_EQUAL(std::string("042Y"), series->get_patient_age());

        CPPUNIT_ASSERT_EQUAL(std::string("IRCAD"), series->get_institution_name());

        data::image_series::sptr img_series   = std::dynamic_pointer_cast<data::image_series>(series);
        data::model_series::sptr model_series = std::dynamic_pointer_cast<data::model_series>(series);

        if(img_series)
        {
            ++nb_is;
            CPPUNIT_ASSERT(img_series->size_in_bytes() > 0);
        }
        else if(model_series)
        {
            ++nb_ms;
            CPPUNIT_ASSERT(!model_series->get_reconstruction_db().empty());
        }
    }

    CPPUNIT_ASSERT_EQUAL(nb_img_series, nb_is);
    CPPUNIT_ASSERT_EQUAL(nb_model_series, nb_ms);
}

} //namespace sight::utest_data::ut
