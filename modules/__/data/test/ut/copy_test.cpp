/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "copy_test.hpp"

#include <data/string.hpp>

#include <service/op.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::data::ut::copy_test);

namespace sight::module::data::ut
{

//------------------------------------------------------------------------------

void copy_test::setUp()
{
    m_copy = service::add("sight::module::data::copy");
}

//------------------------------------------------------------------------------

void copy_test::tearDown()
{
    CPPUNIT_ASSERT_NO_THROW(m_copy->stop().get());
    service::remove(m_copy);
}

//------------------------------------------------------------------------------

void copy_test::on_start_test()
{
    using namespace std::literals::string_literals;

    auto source = std::make_shared<sight::data::string>("Hello world");
    m_copy->set_input(source, "source");
    boost::property_tree::ptree ptree;
    ptree.put("in", "");
    ptree.put("out", "");
    ptree.put("mode", "copyOnStart");
    m_copy->set_config(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_copy->configure());

    CPPUNIT_ASSERT_NO_THROW(m_copy->start().get());
    auto out_target = m_copy->output<sight::data::string>("outTarget");
    CPPUNIT_ASSERT_EQUAL("Hello world"s, out_target.lock()->get_value());
}

//------------------------------------------------------------------------------

void copy_test::on_update_test()
{
    using namespace std::literals::string_literals;

    auto source = std::make_shared<sight::data::string>("Hello world");
    m_copy->set_input(source, "source");
    auto target = std::make_shared<sight::data::string>();
    m_copy->set_inout(target, "target");
    boost::property_tree::ptree ptree;
    ptree.put("in", "");
    ptree.put("inout", "");
    m_copy->set_config(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_copy->configure());
    CPPUNIT_ASSERT_NO_THROW(m_copy->start().get());

    CPPUNIT_ASSERT_NO_THROW(m_copy->update().get());
    CPPUNIT_ASSERT_EQUAL("Hello world"s, target->get_value());
}

//------------------------------------------------------------------------------

} // namespace sight::module::data::ut
