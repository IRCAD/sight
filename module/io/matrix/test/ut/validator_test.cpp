/************************************************************************
 *
 * Copyright (C) 2024-2026 IRCAD France
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

#include <core/com/slots.hpp>

#include <data/matrix4.hpp>

#include <service/op.hpp>

#include <utest/wait.hpp>

#include <doctest/doctest.h>
// cspell:ignore coefs

TEST_SUITE("sight::module::io::matrix::validator")
{
    using sig_key_t  = sight::core::com::signals::key_t;
    using slot_key_t = sight::core::com::slots::key_t;

    static sig_key_t s_valid_sig   = "valid";
    static sig_key_t s_invalid_sig = "invalid";

    static slot_key_t s_update_slot = "update";

//------------------------------------------------------------------------------

    TEST_CASE("basic")
    {
        // Create the service
        auto validator = sight::service::add("sight::module::io::matrix::validator");
        CHECK(validator);

        sight::data::matrix4::sptr mat = std::make_shared<sight::data::matrix4>();

        bool valid      = false;
        bool invalid    = true;
        auto slot_valid = sight::core::com::new_slot([&valid, &invalid](){valid = true; invalid = false;});
        slot_valid->set_worker(sight::core::thread::get_default_worker());

        auto slot_invalid = sight::core::com::new_slot([&invalid, &valid](){invalid = true; valid = false;});
        slot_invalid->set_worker(sight::core::thread::get_default_worker());

        validator->signal(s_valid_sig)->connect(slot_valid);
        validator->signal(s_invalid_sig)->connect(slot_invalid);

        validator->set_input(mat, "matrix");

        // Create the service configuration
        sight::service::config_t config;

        boost::property_tree::ptree config_child;
        config_child.put("<xmlattr>.interactive", false);
        config.add_child("config", config_child);

        // Start the service
        CHECK_NOTHROW(validator->set_config(config));
        CHECK_NOTHROW(validator->configure());
        CHECK_NOTHROW(validator->start().get());

        validator->slot(s_update_slot)->run();

        SIGHT_TEST_WAIT(valid == true);
        SIGHT_TEST_WAIT(invalid == false);

        CHECK(valid);
        CHECK(!invalid);

        // Stop the service
        CHECK_NOTHROW(validator->stop().get());
        sight::service::remove(validator);
    }

//------------------------------------------------------------------------------

    TEST_CASE("orthogonal")
    {
        // Create the service
        auto validator = sight::service::add("sight::module::io::matrix::validator");
        CHECK(validator);

        sight::data::matrix4::sptr mat          = std::make_shared<sight::data::matrix4>();
        sight::data::matrix4::container_t coefs = {
            0.001, 1., 0., .12,
            1., 0., 0., 12.,
            0., 0., -0.9999, 40.,
            0., 0., 0., 1.
        };

        *mat = coefs;

        bool valid      = false;
        bool invalid    = true;
        auto slot_valid = sight::core::com::new_slot([&valid, &invalid](){valid = true; invalid = false;});
        slot_valid->set_worker(sight::core::thread::get_default_worker());

        auto slot_invalid = sight::core::com::new_slot([&invalid, &valid](){invalid = true; valid = false;});
        slot_invalid->set_worker(sight::core::thread::get_default_worker());

        validator->signal(s_valid_sig)->connect(slot_valid);
        validator->signal(s_invalid_sig)->connect(slot_invalid);

        validator->set_input(mat, "matrix");

        // Create the service configuration
        sight::service::config_t config;

        boost::property_tree::ptree config_child;
        config_child.put("<xmlattr>.interactive", false);
        config.add_child("config", config_child);

        // Start the service
        CHECK_NOTHROW(validator->set_config(config));
        CHECK_NOTHROW(validator->configure());
        CHECK_NOTHROW(validator->start().get());

        validator->slot(s_update_slot)->run();

        SIGHT_TEST_WAIT(valid == false);
        SIGHT_TEST_WAIT(invalid == true);

        CHECK(!valid);
        CHECK(invalid);

        coefs = {
            0., 1., 0., .12,
            1., 0., 0., 12.,
            0., 0., -1.0, 40.,
            0., 0., 0., 1.
        };

        *mat = coefs;

        validator->slot(s_update_slot)->run();

        SIGHT_TEST_WAIT(valid == true);
        SIGHT_TEST_WAIT(invalid == false);

        CHECK(valid);
        CHECK(!invalid);

        // Stop the service
        CHECK_NOTHROW(validator->stop().get());
        sight::service::remove(validator);
    }

//------------------------------------------------------------------------------

    TEST_CASE("homogeneous")
    {
        // Create the service
        auto validator = sight::service::add("sight::module::io::matrix::validator");
        CHECK(validator);

        sight::data::matrix4::sptr mat          = std::make_shared<sight::data::matrix4>();
        sight::data::matrix4::container_t coefs = {
            1., 0., 0., 10.,
            0., 1., 0., 20.,
            0., 0., 1., -30.,
            1., 0.1, 0., 4.
        };

        *mat = coefs;

        bool valid   = false;
        bool invalid = true;

        auto slot_valid = sight::core::com::new_slot([&valid, &invalid](){valid = true; invalid = false;});
        slot_valid->set_worker(sight::core::thread::get_default_worker());

        auto slot_invalid = sight::core::com::new_slot([&invalid, &valid](){invalid = true; valid = false;});
        slot_invalid->set_worker(sight::core::thread::get_default_worker());

        validator->signal(s_valid_sig)->connect(slot_valid);
        validator->signal(s_invalid_sig)->connect(slot_invalid);

        validator->set_input(mat, "matrix");

        // Create the service configuration
        sight::service::config_t config;

        boost::property_tree::ptree config_child;
        config_child.put("<xmlattr>.interactive", false);
        config.add_child("config", config_child);

        // Start the service
        CHECK_NOTHROW(validator->set_config(config));
        CHECK_NOTHROW(validator->configure());
        CHECK_NOTHROW(validator->start().get());

        validator->slot(s_update_slot)->run();

        SIGHT_TEST_WAIT(valid == false);
        SIGHT_TEST_WAIT(invalid == true);

        CHECK(!valid);
        CHECK(invalid);

        coefs = {
            1., 0., 0., 10.,
            0., 1., 0., 20.,
            0., 0., 1., -30.,
            0., 0., 0., 1.
        };

        *mat = coefs;

        validator->slot(s_update_slot)->run();

        SIGHT_TEST_WAIT(valid == true);
        SIGHT_TEST_WAIT(invalid == false);

        CHECK(valid);
        CHECK(!invalid);

        // Stop the service
        CHECK_NOTHROW(validator->stop().get());
        sight::service::remove(validator);
    }

//------------------------------------------------------------------------------

    TEST_CASE("empty_mat")
    {
        // Create the service
        auto validator = sight::service::add("sight::module::io::matrix::validator");
        CHECK(validator);

        sight::data::matrix4::sptr mat = nullptr;

        bool valid      = false;
        bool invalid    = true;
        auto slot_valid = sight::core::com::new_slot([&valid, &invalid](){valid = true; invalid = false;});
        slot_valid->set_worker(sight::core::thread::get_default_worker());
        auto slot_invalid = sight::core::com::new_slot([&invalid, &valid](){invalid = true; valid = false;});
        slot_invalid->set_worker(sight::core::thread::get_default_worker());

        validator->signal(s_valid_sig)->connect(slot_valid);
        validator->signal(s_invalid_sig)->connect(slot_invalid);

        validator->set_input(mat, "matrix");

        // Create the service configuration
        sight::service::config_t config;

        boost::property_tree::ptree config_child;
        config_child.put("<xmlattr>.interactive", false);
        config.add_child("config", config_child);

        // Start the service
        CHECK_NOTHROW(validator->set_config(config));
        CHECK_NOTHROW(validator->configure());
        CHECK_NOTHROW(validator->start().get());

        validator->slot(s_update_slot)->run();

        SIGHT_TEST_WAIT(valid == false);
        SIGHT_TEST_WAIT(invalid == true);

        CHECK(!valid);
        CHECK(invalid);

        // Stop the service
        CHECK_NOTHROW(validator->stop().get());
        sight::service::remove(validator);
    }
} // TEST_SUITE
