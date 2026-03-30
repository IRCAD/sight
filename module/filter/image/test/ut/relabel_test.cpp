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

#include <boost/property_tree/xml_parser.hpp>

#include <doctest/doctest.h>

#include <data/image.hpp>
#include <data/integer.hpp>
#include <service/base.hpp>

#include <utest/service_fixture.hpp>

TEST_SUITE("sight::module::filter::image::relabel")
{
    namespace
    {

    struct relabel_tester : public sight::utest::service_fixture
    {
        explicit relabel_tester(const sight::service::config_t& _config = sight::service::config_t()) :
            sight::utest::service_fixture("sight::module::filter::image::relabel"),
            m_mask(std::make_shared<sight::data::image>()),
            m_dictionary(std::make_shared<sight::data::map>())
        {
            CHECK(m_service != nullptr);

            m_service->set_inout(m_mask, "image", false);
            m_service->set_input(m_dictionary, "dictionary", false);

            CHECK_NOTHROW(m_service->configure(_config));
            CHECK_NOTHROW(m_service->start().get());
        }

        sight::data::image::sptr m_mask;
        sight::data::map::sptr m_dictionary;
    };

    } // namespace

//------------------------------------------------------------------------------

    TEST_CASE("dictionary")
    {
        relabel_tester tester;

        auto dump_lock = tester.m_mask->dump_lock();

        // Paint a mask, each slice has a different value, starting from 0
        constexpr std::size_t size = 12;
        tester.m_mask->resize(
            {size, size, size},
            sight::core::type::UINT8,
            sight::data::image::pixel_format_t::gray_scale
        );

        for(std::size_t x = 0 ; x < size ; ++x)
        {
            for(std::size_t y = 0 ; y < size ; ++y)
            {
                uint8_t value = 0;

                for(std::size_t z = 0 ; z < size ; ++z)
                {
                    tester.m_mask->at<uint8_t>(x, y, z) = value++;
                }
            }
        }

        // Setup the filter
        constexpr std::uint8_t new_value = 42;
        tester.m_dictionary->insert_or_assign("0", std::make_shared<sight::data::integer>(new_value));
        tester.m_dictionary->insert_or_assign("1", std::make_shared<sight::data::integer>(new_value));
        tester.m_dictionary->insert_or_assign("2", std::make_shared<sight::data::integer>(new_value));
        tester.m_dictionary->insert_or_assign("3", std::make_shared<sight::data::integer>(new_value));
        tester.m_dictionary->insert_or_assign("4", std::make_shared<sight::data::integer>(new_value));
        tester.m_dictionary->insert_or_assign("5", std::make_shared<sight::data::integer>(new_value));
        tester.m_dictionary->insert_or_assign("6", std::make_shared<sight::data::integer>(new_value));
        tester.m_dictionary->insert_or_assign("7", std::make_shared<sight::data::integer>(new_value));
        tester.m_dictionary->insert_or_assign("8", std::make_shared<sight::data::integer>(new_value));
        tester.m_dictionary->insert_or_assign("9", std::make_shared<sight::data::integer>(new_value));
        tester.m_dictionary->insert_or_assign("10", std::make_shared<sight::data::integer>(new_value));
        tester.m_dictionary->insert_or_assign("*", std::make_shared<sight::data::integer>(new_value));

        // Run the filter
        tester.m_service->update().get();

        // Check the result
        for(std::size_t x = 0 ; x < size ; ++x)
        {
            for(std::size_t y = 0 ; y < size ; ++y)
            {
                for(std::size_t z = 0 ; z < size ; ++z)
                {
                    const std::string message = "(x=" + std::to_string(x)
                                                + ", y=" + std::to_string(y)
                                                + ", z=" + std::to_string(z) + ")";

                    CHECK_MESSAGE(new_value == tester.m_mask->at<uint8_t>(x, y, z), message);
                }
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("config")
    {
        constexpr std::uint8_t new_value = 42;

        // Setup XML configuration
        std::stringstream config_string;
        config_string << "<config>";
        config_string << R"(    <map from="0" to=")" << std::to_string(new_value) << "\" />";
        config_string << R"(    <map from="1" to=")" << std::to_string(new_value) << "\" />";
        config_string << R"(    <map from="2" to=")" << std::to_string(new_value) << "\" />";
        config_string << R"(    <map from="3" to=")" << std::to_string(new_value) << "\" />";
        config_string << R"(    <map from="4" to=")" << std::to_string(new_value) << "\" />";
        config_string << R"(    <map from="5" to=")" << std::to_string(new_value) << "\" />";
        config_string << R"(    <map from="6" to=")" << std::to_string(new_value) << "\" />";
        config_string << R"(    <map from="7" to=")" << std::to_string(new_value) << "\" />";
        config_string << R"(    <map from="8" to=")" << std::to_string(new_value) << "\" />";
        config_string << R"(    <map from="9" to=")" << std::to_string(new_value) << "\" />";
        config_string << R"(    <map from="10" to=")" << std::to_string(new_value) << "\" />";
        config_string << R"(    <map from="*" to=")" << std::to_string(new_value) << "\" />";
        config_string << "</config>";

        sight::service::config_t config;
        boost::property_tree::read_xml(config_string, config);

        relabel_tester tester(config);

        auto dump_lock = tester.m_mask->dump_lock();

        // Paint a mask, each slice has a different value, starting from 0
        constexpr std::size_t size = 12;
        tester.m_mask->resize(
            {size, size, size},
            sight::core::type::UINT8,
            sight::data::image::pixel_format_t::gray_scale
        );

        for(std::size_t x = 0 ; x < size ; ++x)
        {
            for(std::size_t y = 0 ; y < size ; ++y)
            {
                uint8_t value = 0;

                for(std::size_t z = 0 ; z < size ; ++z)
                {
                    tester.m_mask->at<uint8_t>(x, y, z) = value++;
                }
            }
        }

        // Run the filter
        tester.m_service->update().get();

        // Check the result
        for(std::size_t x = 0 ; x < size ; ++x)
        {
            for(std::size_t y = 0 ; y < size ; ++y)
            {
                for(std::size_t z = 0 ; z < size ; ++z)
                {
                    const std::string message = "(x=" + std::to_string(x)
                                                + ", y=" + std::to_string(y)
                                                + ", z=" + std::to_string(z) + ")";

                    CHECK_MESSAGE(new_value == tester.m_mask->at<uint8_t>(x, y, z), message);
                }
            }
        }
    }
} // TEST_SUITE
