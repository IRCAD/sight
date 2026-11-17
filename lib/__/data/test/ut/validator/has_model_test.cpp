/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include <data/model_series.hpp>
#include <data/reconstruction.hpp>
#include <data/string.hpp>
#include <data/validator/base.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::validator::has_model")
{
    namespace factory = sight::data::validator::factory;

//------------------------------------------------------------------------------

    TEST_CASE("single_organ")
    {
        auto validator = factory::make("sight::data::validator::has_model");
        CHECK(validator);

        auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
        CHECK(obj_validator);

        sight::data::validator::config_t config;
        // Add organ element
        sight::data::validator::config_t organ;
        organ.put("<xmlattr>.name", "Masses");
        organ.put("<xmlattr>.type", "Liver");
        config.add_child("organ", organ);

        validator->configure(config);

        sight::data::validator::return_t validation;
        auto model_series = std::make_shared<sight::data::model_series>();

        // Test: empty model_series should fail
        validation = obj_validator->validate(model_series);
        CHECK_EQ(false, validation.first);

        // Test: add the required reconstruction
        auto recon = std::make_shared<sight::data::reconstruction>();
        recon->set_organ_name("Masses");
        recon->set_structure_type("Liver");
        auto reconstruction_db = model_series->get_reconstruction_db();
        reconstruction_db.push_back(recon);
        model_series->set_reconstruction_db(reconstruction_db);

        validation = obj_validator->validate(model_series);
        CHECK_EQ(true, validation.first);
    }

//------------------------------------------------------------------------------

    TEST_CASE("multiple_organs")
    {
        auto validator = factory::make("sight::data::validator::has_model");
        CHECK(validator);

        auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
        CHECK(obj_validator);

        sight::data::validator::config_t config;
        // Add first organ element
        sight::data::validator::config_t organ1;
        organ1.put("<xmlattr>.name", "Masses");
        organ1.put("<xmlattr>.type", "Liver");
        config.add_child("organ", organ1);

        // Add second organ element
        sight::data::validator::config_t organ2;
        organ2.put("<xmlattr>.name", "Vessels");
        organ2.put("<xmlattr>.type", "Liver");
        config.add_child("organ", organ2);

        validator->configure(config);

        sight::data::validator::return_t validation;
        auto model_series = std::make_shared<sight::data::model_series>();

        // Test: empty model_series should fail
        validation = obj_validator->validate(model_series);
        CHECK_EQ(false, validation.first);

        // Test: add only first organ (should fail)
        auto recon1 = std::make_shared<sight::data::reconstruction>();
        recon1->set_organ_name("Masses");
        recon1->set_structure_type("Liver");
        auto reconstruction_db = model_series->get_reconstruction_db();
        reconstruction_db.push_back(recon1);
        model_series->set_reconstruction_db(reconstruction_db);

        validation = obj_validator->validate(model_series);
        CHECK_EQ(false, validation.first);

        // Test: add second organ (should pass)
        auto recon2 = std::make_shared<sight::data::reconstruction>();
        recon2->set_organ_name("Vessels");
        recon2->set_structure_type("Liver");
        reconstruction_db.push_back(recon2);
        model_series->set_reconstruction_db(reconstruction_db);

        validation = obj_validator->validate(model_series);
        CHECK_EQ(true, validation.first);
    }

//------------------------------------------------------------------------------

    TEST_CASE("wrong_type")
    {
        auto validator = factory::make("sight::data::validator::has_model");
        CHECK(validator);

        auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
        CHECK(obj_validator);

        sight::data::validator::config_t config;
        sight::data::validator::config_t organ;
        organ.put("<xmlattr>.name", "Masses");
        organ.put("<xmlattr>.type", "Liver");
        config.add_child("organ", organ);

        validator->configure(config);

        sight::data::validator::return_t validation;
        auto model_series = std::make_shared<sight::data::model_series>();

        // Add reconstruction with correct name but wrong type
        auto recon = std::make_shared<sight::data::reconstruction>();
        recon->set_organ_name("Masses");
        recon->set_structure_type("Kidney"); // Wrong type
        auto reconstruction_db = model_series->get_reconstruction_db();
        reconstruction_db.push_back(recon);
        model_series->set_reconstruction_db(reconstruction_db);

        validation = obj_validator->validate(model_series);
        CHECK_EQ(false, validation.first);
    }

//------------------------------------------------------------------------------

    TEST_CASE("wrong_name")
    {
        auto validator = factory::make("sight::data::validator::has_model");
        CHECK(validator);

        auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
        CHECK(obj_validator);

        sight::data::validator::config_t config;
        sight::data::validator::config_t organ;
        organ.put("<xmlattr>.name", "Masses");
        organ.put("<xmlattr>.type", "Liver");
        config.add_child("organ", organ);

        validator->configure(config);

        sight::data::validator::return_t validation;
        auto model_series = std::make_shared<sight::data::model_series>();

        // Add reconstruction with correct type but wrong name
        auto recon = std::make_shared<sight::data::reconstruction>();
        recon->set_organ_name("Fibrosis"); // Wrong name
        recon->set_structure_type("Liver");
        auto reconstruction_db = model_series->get_reconstruction_db();
        reconstruction_db.push_back(recon);
        model_series->set_reconstruction_db(reconstruction_db);

        validation = obj_validator->validate(model_series);
        CHECK_EQ(false, validation.first);
    }

//------------------------------------------------------------------------------

    TEST_CASE("multiple_reconstructions")
    {
        auto validator = factory::make("sight::data::validator::has_model");
        CHECK(validator);

        auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
        CHECK(obj_validator);

        sight::data::validator::config_t config;
        sight::data::validator::config_t organ;
        organ.put("<xmlattr>.name", "Vessels");
        organ.put("<xmlattr>.type", "Kidney");
        config.add_child("organ", organ);

        validator->configure(config);

        sight::data::validator::return_t validation;
        auto model_series = std::make_shared<sight::data::model_series>();

        // Add multiple reconstructions (including the one we're looking for)
        auto recon1 = std::make_shared<sight::data::reconstruction>();
        recon1->set_organ_name("Masses");
        recon1->set_structure_type("Liver");
        auto reconstruction_db = model_series->get_reconstruction_db();
        reconstruction_db.push_back(recon1);
        model_series->set_reconstruction_db(reconstruction_db);

        auto recon2 = std::make_shared<sight::data::reconstruction>();
        recon2->set_organ_name("Vessels");
        recon2->set_structure_type("Kidney");
        reconstruction_db.push_back(recon2);
        model_series->set_reconstruction_db(reconstruction_db);

        auto recon3 = std::make_shared<sight::data::reconstruction>();
        recon3->set_organ_name("Fibrosis");
        recon3->set_structure_type("Liver");
        reconstruction_db.push_back(recon3);
        model_series->set_reconstruction_db(reconstruction_db);

        validation = obj_validator->validate(model_series);
        CHECK_EQ(true, validation.first);
    }

//------------------------------------------------------------------------------

    TEST_CASE("invalid_object_type")
    {
        auto validator = factory::make("sight::data::validator::has_model");
        CHECK(validator);

        auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
        CHECK(obj_validator);

        sight::data::validator::config_t config;
        sight::data::validator::config_t organ;
        organ.put("<xmlattr>.name", "Masses");
        organ.put("<xmlattr>.type", "Liver");
        config.add_child("organ", organ);

        validator->configure(config);

        sight::data::validator::return_t validation;
        // Create a non-model_series object
        auto string_obj = std::make_shared<sight::data::string>();

        validation = obj_validator->validate(string_obj);
        CHECK_EQ(false, validation.first);
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::data::validator::has_model")
