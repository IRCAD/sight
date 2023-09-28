/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "demangler_test.hpp"

#include "core/base.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::ut::demangler_test);

namespace sight::core::ut
{

//------------------------------------------------------------------------------

void demangler_test::setUp()
{
}

//-----------------------------------------------------------------------------

class idemangler_test : public core::base_object
{
public:

    SIGHT_DECLARE_CLASS(idemangler_test, core::base_object);
    ~idemangler_test() override;
};

idemangler_test::~idemangler_test()
= default;

//-----------------------------------------------------------------------------

class odemangler_test : public idemangler_test
{
public:

    SIGHT_DECLARE_CLASS(odemangler_test, idemangler_test);
    ~odemangler_test() override;
};

odemangler_test::~odemangler_test()
= default;

//-----------------------------------------------------------------------------

void demangler_test::tearDown()
{
}

//-----------------------------------------------------------------------------

void demangler_test::class_name_test()
{
    CPPUNIT_ASSERT_EQUAL(std::string("sight::core::ut::idemangler_test"), core::ut::idemangler_test::classname());
    CPPUNIT_ASSERT_EQUAL(std::string("sight::core::ut::odemangler_test"), core::ut::odemangler_test::classname());

    CPPUNIT_ASSERT_EQUAL(std::string("idemangler_test"), core::ut::idemangler_test::leaf_classname());
    CPPUNIT_ASSERT_EQUAL(std::string("odemangler_test"), core::ut::odemangler_test::leaf_classname());

    odemangler_test::sptr object         = std::make_shared<odemangler_test>();
    idemangler_test::sptr generic_object = object;
    CPPUNIT_ASSERT_EQUAL(std::string("sight::core::ut::odemangler_test"), object->get_classname());
    CPPUNIT_ASSERT_EQUAL(std::string("sight::core::ut::odemangler_test"), generic_object->get_classname());

    CPPUNIT_ASSERT_EQUAL(std::string("odemangler_test"), object->get_leaf_classname());
    CPPUNIT_ASSERT_EQUAL(std::string("odemangler_test"), generic_object->get_leaf_classname());
}

//-----------------------------------------------------------------------------

} // namespace sight::core::ut
