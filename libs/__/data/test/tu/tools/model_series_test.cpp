/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "model_series_test.hpp"

#include <data/color.hpp>
#include <data/material.hpp>
#include <data/model_series.hpp>
#include <data/tools/model_series.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::tools::ut::model_series_test);

namespace sight::data::tools::ut
{

//------------------------------------------------------------------------------

void model_series_test::setUp()
{
}

//------------------------------------------------------------------------------

void model_series_test::tearDown()
{
}

//------------------------------------------------------------------------------

void model_series_test::add_reconstruction()
{
    data::model_series::sptr model_series = std::make_shared<data::model_series>();
    data::reconstruction::sptr rec1       = std::make_shared<data::reconstruction>();

    data::tools::model_series::add_reconstruction(model_series, rec1);

    const data::model_series::reconstruction_vector_t& rec_db = model_series->get_reconstruction_db();
    CPPUNIT_ASSERT_EQUAL(rec1, rec_db[0]);

    data::reconstruction::sptr rec2 = std::make_shared<data::reconstruction>();

    data::tools::model_series::add_reconstruction(model_series, rec2);

    CPPUNIT_ASSERT_EQUAL(rec1, rec_db[0]);
    CPPUNIT_ASSERT_EQUAL(rec2, rec_db[1]);

    data::reconstruction::sptr rec3 = std::make_shared<data::reconstruction>();
    data::tools::model_series::add_reconstruction(model_series, rec3);

    CPPUNIT_ASSERT_EQUAL(rec1, rec_db[0]);
    CPPUNIT_ASSERT_EQUAL(rec2, rec_db[1]);
    CPPUNIT_ASSERT_EQUAL(rec3, rec_db[2]);

    CPPUNIT_ASSERT_THROW(data::tools::model_series::add_reconstruction(model_series, rec1), core::exception);

    data::reconstruction::sptr null_rec;
    CPPUNIT_ASSERT_THROW(data::tools::model_series::add_reconstruction(model_series, null_rec), core::exception);
}

//------------------------------------------------------------------------------

void model_series_test::create_reconstruction_from_mesh()
{
    data::mesh::sptr mesh   = std::make_shared<data::mesh>();
    data::color::sptr color = std::make_shared<data::color>(1.0F, 0.3F, 0.7F, 0.5F);

    {
        const std::string organ_name     = "Liver1";
        const std::string structure_type = "Liver";

        auto rec = data::tools::model_series::create_reconstruction_from_mesh(
            mesh,
            organ_name,
            structure_type,
            color,
            data::material::surface
        );

        CPPUNIT_ASSERT_EQUAL(rec->get_mesh(), mesh);
        CPPUNIT_ASSERT_EQUAL(rec->get_organ_name(), organ_name);
        CPPUNIT_ASSERT_EQUAL(rec->get_structure_type(), structure_type);
        CPPUNIT_ASSERT_EQUAL(rec->get_is_visible(), true);
        CPPUNIT_ASSERT_EQUAL(rec->get_material()->get_representation_mode(), data::material::surface);

        auto ambient = rec->get_material()->diffuse();
        CPPUNIT_ASSERT_EQUAL(ambient->red(), 1.0F);
        CPPUNIT_ASSERT_EQUAL(ambient->green(), 0.3F);
        CPPUNIT_ASSERT_EQUAL(ambient->blue(), 0.7F);
        CPPUNIT_ASSERT_EQUAL(ambient->alpha(), 0.5F);
    }

    {
        const std::string organ_name     = "Foo";
        const std::string structure_type = "Foofoo";

        auto rec = data::tools::model_series::create_reconstruction_from_mesh(
            mesh,
            organ_name,
            structure_type,
            color,
            data::material::wireframe,
            false
        );

        CPPUNIT_ASSERT_EQUAL(rec->get_mesh(), mesh);
        CPPUNIT_ASSERT_EQUAL(rec->get_organ_name(), organ_name);
        CPPUNIT_ASSERT_EQUAL(rec->get_structure_type(), structure_type);
        CPPUNIT_ASSERT_EQUAL(rec->get_is_visible(), false);
        CPPUNIT_ASSERT_EQUAL(rec->get_material()->get_representation_mode(), data::material::wireframe);

        auto ambient = rec->get_material()->diffuse();
        CPPUNIT_ASSERT_EQUAL(ambient->red(), 1.0F);
        CPPUNIT_ASSERT_EQUAL(ambient->green(), 0.3F);
        CPPUNIT_ASSERT_EQUAL(ambient->blue(), 0.7F);
        CPPUNIT_ASSERT_EQUAL(ambient->alpha(), 0.5F);
    }
}

//------------------------------------------------------------------------------

void model_series_test::add_mesh()
{
    data::model_series::sptr model_series                     = std::make_shared<data::model_series>();
    const data::model_series::reconstruction_vector_t& rec_db = model_series->get_reconstruction_db();

    {
        data::mesh::sptr mesh            = std::make_shared<data::mesh>();
        data::color::sptr color          = std::make_shared<data::color>(1.0F, 0.3F, 0.7F, 0.5F);
        const std::string organ_name     = "Liver1";
        const std::string structure_type = "Liver";

        data::tools::model_series::add_mesh(
            model_series,
            mesh,
            organ_name,
            structure_type,
            color,
            data::material::surface
        );

        auto rec = rec_db[0];

        CPPUNIT_ASSERT_EQUAL(rec->get_mesh(), mesh);
        CPPUNIT_ASSERT_EQUAL(rec->get_organ_name(), organ_name);
        CPPUNIT_ASSERT_EQUAL(rec->get_structure_type(), structure_type);
        CPPUNIT_ASSERT_EQUAL(rec->get_is_visible(), true);
        CPPUNIT_ASSERT_EQUAL(rec->get_material()->get_representation_mode(), data::material::surface);

        auto ambient = rec->get_material()->diffuse();
        CPPUNIT_ASSERT_EQUAL(ambient->red(), 1.0F);
        CPPUNIT_ASSERT_EQUAL(ambient->green(), 0.3F);
        CPPUNIT_ASSERT_EQUAL(ambient->blue(), 0.7F);
        CPPUNIT_ASSERT_EQUAL(ambient->alpha(), 0.5F);
    }

    {
        data::mesh::sptr mesh            = std::make_shared<data::mesh>();
        data::color::sptr color          = std::make_shared<data::color>(0.1F, 0.3F, 0.7F, 1.5F);
        const std::string organ_name     = "Foo";
        const std::string structure_type = "Foofoo";

        data::tools::model_series::add_mesh(
            model_series,
            mesh,
            organ_name,
            structure_type,
            color,
            data::material::wireframe,
            false
        );
        auto rec = rec_db[1];

        CPPUNIT_ASSERT_EQUAL(rec->get_mesh(), mesh);
        CPPUNIT_ASSERT_EQUAL(rec->get_organ_name(), organ_name);
        CPPUNIT_ASSERT_EQUAL(rec->get_structure_type(), structure_type);
        CPPUNIT_ASSERT_EQUAL(rec->get_is_visible(), false);
        CPPUNIT_ASSERT_EQUAL(rec->get_material()->get_representation_mode(), data::material::wireframe);

        auto ambient = rec->get_material()->diffuse();
        CPPUNIT_ASSERT_EQUAL(ambient->red(), 0.1F);
        CPPUNIT_ASSERT_EQUAL(ambient->green(), 0.3F);
        CPPUNIT_ASSERT_EQUAL(ambient->blue(), 0.7F);
        CPPUNIT_ASSERT_EQUAL(ambient->alpha(), 1.5F);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::data::tools::ut
