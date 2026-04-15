/************************************************************************
 *
 * Copyright (C) 2014-2026 IRCAD France
 * Copyright (C) 2014-2016 IHU Strasbourg
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

#include <data/color.hpp>
#include <data/material.hpp>
#include <data/model_series.hpp>
#include <data/tools/model_series.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::tools::model_series")
{
//------------------------------------------------------------------------------

    TEST_CASE("add_reconstruction")
    {
        sight::data::model_series::sptr model_series = std::make_shared<sight::data::model_series>();
        sight::data::reconstruction::sptr rec1       = std::make_shared<sight::data::reconstruction>();

        sight::data::tools::model_series::add_reconstruction(model_series, rec1);

        const sight::data::model_series::reconstruction_vector_t& rec_db = model_series->get_reconstruction_db();
        CHECK_EQ(rec1, rec_db[0]);

        sight::data::reconstruction::sptr rec2 = std::make_shared<sight::data::reconstruction>();

        sight::data::tools::model_series::add_reconstruction(model_series, rec2);

        CHECK_EQ(rec1, rec_db[0]);
        CHECK_EQ(rec2, rec_db[1]);

        sight::data::reconstruction::sptr rec3 = std::make_shared<sight::data::reconstruction>();
        sight::data::tools::model_series::add_reconstruction(model_series, rec3);

        CHECK_EQ(rec1, rec_db[0]);
        CHECK_EQ(rec2, rec_db[1]);
        CHECK_EQ(rec3, rec_db[2]);

        CHECK_THROWS_AS(
            sight::data::tools::model_series::add_reconstruction(model_series, rec1),
            sight::core::exception
        );

        sight::data::reconstruction::sptr null_rec;
        CHECK_THROWS_AS(
            sight::data::tools::model_series::add_reconstruction(model_series, null_rec),
            sight::core::exception
        );
    }

//------------------------------------------------------------------------------

    TEST_CASE("create_reconstruction_from_mesh")
    {
        sight::data::mesh::sptr mesh   = std::make_shared<sight::data::mesh>();
        sight::data::color::sptr color = std::make_shared<sight::data::color>(1.0F, 0.3F, 0.7F, 0.5F);

        {
            const std::string organ_name     = "Liver1";
            const std::string structure_type = "Liver";

            auto rec = sight::data::tools::model_series::create_reconstruction_from_mesh(
                mesh,
                organ_name,
                structure_type,
                color,
                sight::data::material::surface
            );

            CHECK_EQ(rec->get_mesh(), mesh);
            CHECK_EQ(rec->get_organ_name(), organ_name);
            CHECK_EQ(rec->get_structure_type(), structure_type);
            CHECK_EQ(rec->get_is_visible(), true);
            CHECK_EQ(rec->get_material()->get_representation_mode(), sight::data::material::surface);

            auto ambient = rec->get_material()->diffuse();
            CHECK_EQ(ambient->red(), 1.0F);
            CHECK_EQ(ambient->green(), 0.3F);
            CHECK_EQ(ambient->blue(), 0.7F);
            CHECK_EQ(ambient->alpha(), 0.5F);
        }

        {
            const std::string organ_name     = "Foo";
            const std::string structure_type = "Foofoo";

            auto rec = sight::data::tools::model_series::create_reconstruction_from_mesh(
                mesh,
                organ_name,
                structure_type,
                color,
                sight::data::material::wireframe,
                false
            );

            CHECK_EQ(rec->get_mesh(), mesh);
            CHECK_EQ(rec->get_organ_name(), organ_name);
            CHECK_EQ(rec->get_structure_type(), structure_type);
            CHECK_EQ(rec->get_is_visible(), false);
            CHECK_EQ(rec->get_material()->get_representation_mode(), sight::data::material::wireframe);

            auto ambient = rec->get_material()->diffuse();
            CHECK_EQ(ambient->red(), 1.0F);
            CHECK_EQ(ambient->green(), 0.3F);
            CHECK_EQ(ambient->blue(), 0.7F);
            CHECK_EQ(ambient->alpha(), 0.5F);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("add_mesh")
    {
        sight::data::model_series::sptr model_series =
            std::make_shared<sight::data::model_series>();
        const sight::data::model_series::reconstruction_vector_t& rec_db = model_series->get_reconstruction_db();

        {
            sight::data::mesh::sptr mesh     = std::make_shared<sight::data::mesh>();
            sight::data::color::sptr color   = std::make_shared<sight::data::color>(1.0F, 0.3F, 0.7F, 0.5F);
            const std::string organ_name     = "Liver1";
            const std::string structure_type = "Liver";

            sight::data::tools::model_series::add_mesh(
                model_series,
                mesh,
                organ_name,
                structure_type,
                color,
                sight::data::material::surface
            );

            auto rec = rec_db[0];

            CHECK_EQ(rec->get_mesh(), mesh);
            CHECK_EQ(rec->get_organ_name(), organ_name);
            CHECK_EQ(rec->get_structure_type(), structure_type);
            CHECK_EQ(rec->get_is_visible(), true);
            CHECK_EQ(rec->get_material()->get_representation_mode(), sight::data::material::surface);

            auto ambient = rec->get_material()->diffuse();
            CHECK_EQ(ambient->red(), 1.0F);
            CHECK_EQ(ambient->green(), 0.3F);
            CHECK_EQ(ambient->blue(), 0.7F);
            CHECK_EQ(ambient->alpha(), 0.5F);
        }

        {
            sight::data::mesh::sptr mesh     = std::make_shared<sight::data::mesh>();
            sight::data::color::sptr color   = std::make_shared<sight::data::color>(0.1F, 0.3F, 0.7F, 1.5F);
            const std::string organ_name     = "Foo";
            const std::string structure_type = "Foofoo";

            sight::data::tools::model_series::add_mesh(
                model_series,
                mesh,
                organ_name,
                structure_type,
                color,
                sight::data::material::wireframe,
                false
            );
            auto rec = rec_db[1];

            CHECK_EQ(rec->get_mesh(), mesh);
            CHECK_EQ(rec->get_organ_name(), organ_name);
            CHECK_EQ(rec->get_structure_type(), structure_type);
            CHECK_EQ(rec->get_is_visible(), false);
            CHECK_EQ(rec->get_material()->get_representation_mode(), sight::data::material::wireframe);

            auto ambient = rec->get_material()->diffuse();
            CHECK_EQ(ambient->red(), 0.1F);
            CHECK_EQ(ambient->green(), 0.3F);
            CHECK_EQ(ambient->blue(), 0.7F);
            CHECK_EQ(ambient->alpha(), 1.5F);
        }
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::data::tools")
