/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include "validator_test.hpp"

#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <data/matrix4.hpp>

#include <service/op.hpp>

#include <utest/wait.hpp>
// cspell:ignore coefs

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::matrix::ut::validator_test);

namespace sight::module::io::matrix::ut
{

using sig_key_t  = sight::core::com::signals::key_t;
using slot_key_t = sight::core::com::slots::key_t;

static sig_key_t s_valid_sig   = "valid";
static sig_key_t s_invalid_sig = "invalid";

static slot_key_t s_update_slot = "update";

//------------------------------------------------------------------------------

void validator_test::setUp()
{
}

//------------------------------------------------------------------------------

void validator_test::tearDown()
{
}

//------------------------------------------------------------------------------

void validator_test::basic_test()
{
    // Create the service
    auto validator = service::add("sight::module::io::matrix::validator");
    CPPUNIT_ASSERT_MESSAGE("Failed to create service 'sight::module::io::matrix::validator'", validator);

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
    service::config_t config;

    boost::property_tree::ptree config_child;
    config_child.put("<xmlattr>.interactive", false);
    config.add_child("config", config_child);

    // Start the service
    CPPUNIT_ASSERT_NO_THROW(validator->set_config(config));
    CPPUNIT_ASSERT_NO_THROW(validator->configure());
    CPPUNIT_ASSERT_NO_THROW(validator->start().wait());

    validator->slot(s_update_slot)->run();

    SIGHT_TEST_WAIT(valid == true);
    SIGHT_TEST_WAIT(invalid == false);

    CPPUNIT_ASSERT(valid);
    CPPUNIT_ASSERT(!invalid);

    // Stop the service
    CPPUNIT_ASSERT_NO_THROW(validator->stop().wait());
    service::remove(validator);
}

//------------------------------------------------------------------------------

void validator_test::orthogonal_test()
{
    // Create the service
    auto validator = service::add("sight::module::io::matrix::validator");
    CPPUNIT_ASSERT_MESSAGE("Failed to create service 'sight::module::io::matrix::validator'", validator);

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
    service::config_t config;

    boost::property_tree::ptree config_child;
    config_child.put("<xmlattr>.interactive", false);
    config.add_child("config", config_child);

    // Start the service
    CPPUNIT_ASSERT_NO_THROW(validator->set_config(config));
    CPPUNIT_ASSERT_NO_THROW(validator->configure());
    CPPUNIT_ASSERT_NO_THROW(validator->start().wait());

    validator->slot(s_update_slot)->run();

    SIGHT_TEST_WAIT(valid == false);
    SIGHT_TEST_WAIT(invalid == true);

    CPPUNIT_ASSERT(!valid);
    CPPUNIT_ASSERT(invalid);

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

    CPPUNIT_ASSERT(valid);
    CPPUNIT_ASSERT(!invalid);

    // Stop the service
    CPPUNIT_ASSERT_NO_THROW(validator->stop().wait());
    service::remove(validator);
}

//------------------------------------------------------------------------------

void validator_test::homogeneous_test()
{
    // Create the service
    auto validator = service::add("sight::module::io::matrix::validator");
    CPPUNIT_ASSERT_MESSAGE("Failed to create service 'sight::module::io::matrix::validator'", validator);

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
    service::config_t config;

    boost::property_tree::ptree config_child;
    config_child.put("<xmlattr>.interactive", false);
    config.add_child("config", config_child);

    // Start the service
    CPPUNIT_ASSERT_NO_THROW(validator->set_config(config));
    CPPUNIT_ASSERT_NO_THROW(validator->configure());
    CPPUNIT_ASSERT_NO_THROW(validator->start().wait());

    validator->slot(s_update_slot)->run();

    SIGHT_TEST_WAIT(valid == false);
    SIGHT_TEST_WAIT(invalid == true);

    CPPUNIT_ASSERT(!valid);
    CPPUNIT_ASSERT(invalid);

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

    CPPUNIT_ASSERT(valid);
    CPPUNIT_ASSERT(!invalid);

    // Stop the service
    CPPUNIT_ASSERT_NO_THROW(validator->stop().wait());
    service::remove(validator);
}

//------------------------------------------------------------------------------

void validator_test::empty_mat_test()
{
    // Create the service
    auto validator = service::add("sight::module::io::matrix::validator");
    CPPUNIT_ASSERT_MESSAGE("Failed to create service 'sight::module::io::matrix::validator'", validator);

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
    service::config_t config;

    boost::property_tree::ptree config_child;
    config_child.put("<xmlattr>.interactive", false);
    config.add_child("config", config_child);

    // Start the service
    CPPUNIT_ASSERT_NO_THROW(validator->set_config(config));
    CPPUNIT_ASSERT_NO_THROW(validator->configure());
    CPPUNIT_ASSERT_NO_THROW(validator->start().wait());

    validator->slot(s_update_slot)->run();

    SIGHT_TEST_WAIT(valid == false);
    SIGHT_TEST_WAIT(invalid == true);

    CPPUNIT_ASSERT(!valid);
    CPPUNIT_ASSERT(invalid);

    // Stop the service
    CPPUNIT_ASSERT_NO_THROW(validator->stop().wait());
    service::remove(validator);
}

} // namespace sight::module::io::matrix::ut
