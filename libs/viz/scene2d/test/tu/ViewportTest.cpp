/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "ViewportTest.hpp"

#include <core/runtime/Convert.hpp>

#include <service/base.hpp>

#include <viz/scene2d/data/parser/Viewport.hpp>
#include <viz/scene2d/data/Viewport.hpp>

#include <boost/property_tree/xml_parser.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::scene2d::ut::ViewportTest);

//------------------------------------------------------------------------------

namespace sight::viz::scene2d
{

namespace ut
{

//------------------------------------------------------------------------------

void ViewportTest::setUp()
{
}

//------------------------------------------------------------------------------

void ViewportTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ViewportTest::testParser()
{
    service::IService::ConfigType config;

    std::stringstream config_string;
    config_string << "<object uid=\"test\" type=\"sight::data::Viewport\">"
                     "<config x=\"-700\" y=\"0.1\" width=\"100\" height=\"1.3\" />"
                     "</object>";
    boost::property_tree::read_xml(config_string, config);

    // Create object configuration
    const auto cfg = core::runtime::Convert::fromPropertyTree(config);

    auto parser = sight::service::add<data::parser::Viewport>("sight::viz::scene2d::data::parser::Viewport");
    parser->setObjectConfig(cfg);

    auto viewport = sight::viz::scene2d::data::Viewport::New();
    parser->createConfig(viewport);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(-700., viewport->getX(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(.1, viewport->getY(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(100., viewport->getWidth(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.3, viewport->getHeight(), 0.00001);
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::viz::scene2d
