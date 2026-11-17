/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <data/activity.hpp>
#include <data/image_series.hpp>
#include <data/model_series.hpp>
#include <data/series.hpp>
#include <data/series_set.hpp>
#include <data/string.hpp>

#include <utest_data/generator/series_set.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::utest_data::series_set")
{
    TEST_CASE("generation")
    {
        const unsigned char nb_img_series   = 3;
        const unsigned char nb_model_series = 4;
        auto series_set                     = sight::utest_data::generator::series_set::create_series_set(
            nb_img_series,
            nb_model_series
        );

        CHECK_EQ((std::size_t) (nb_img_series + nb_model_series), series_set->size());

        CHECK(std::dynamic_pointer_cast<sight::data::image_series>(series_set->at(0)));

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
            CHECK_EQ(
                std::string("1.2.826.0.1.3680043.2.1125.102906542887009256605006409108689" + str.str()),
                series->get_series_instance_uid()
            );
            CHECK_EQ(sight::data::dicom::modality_t::ct, series->get_modality());
            CHECK_EQ(std::string("20130418"), series->get_series_date());
            CHECK_EQ(std::string("101010.101010"), series->get_series_time());
            CHECK_EQ(std::string("Description"), series->get_series_description());
            CHECK(performing_physician_name == series->get_performing_physician_name());

            CHECK_EQ(std::string("NomSeriesSet1^PrenomSeriesSet1"), series->get_patient_name());
            CHECK_EQ(std::string("4564383757"), series->get_patient_id());
            CHECK_EQ(std::string("19710418"), series->get_patient_birth_date());
            CHECK_EQ(std::string("O"), series->get_patient_sex());

            CHECK_EQ(
                std::string("1.2.826.0.1.3680043.2.1125.44278200849347599055201494082232" + str.str()),
                series->get_study_instance_uid()
            );
            CHECK_EQ(std::string("20130418"), series->get_study_date());
            CHECK_EQ(std::string("095948.689872"), series->get_study_time());
            CHECK_EQ(std::string("Dr^Jekyl"), series->get_referring_physician_name());
            CHECK_EQ(std::string("Say 33."), series->get_study_description());
            CHECK_EQ(std::string("042Y"), series->get_patient_age());

            CHECK_EQ(std::string("IRCAD"), series->get_institution_name());

            auto img_series   = std::dynamic_pointer_cast<sight::data::image_series>(series);
            auto model_series = std::dynamic_pointer_cast<sight::data::model_series>(series);

            if(img_series)
            {
                ++nb_is;
                CHECK(img_series->size_in_bytes() > 0);
            }
            else if(model_series)
            {
                ++nb_ms;
                CHECK(!model_series->get_reconstruction_db().empty());
            }
        }

        CHECK_EQ(nb_img_series, nb_is);
        CHECK_EQ(nb_model_series, nb_ms);
    }
} // end TEST_SUITE
