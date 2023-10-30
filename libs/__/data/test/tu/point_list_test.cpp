/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "point_list_test.hpp"

#include <data/point.hpp>
#include <data/point_list.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::point_list_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void point_list_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void point_list_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void point_list_test::copy_test()
{
    data::point_list::sptr pl1 = std::make_shared<data::point_list>();
    data::point_list::sptr pl2 = std::make_shared<data::point_list>();
    data::point_list::sptr pl3 = std::make_shared<data::point_list>();

    data::point::sptr point1 = std::make_shared<data::point>(1.0F, 2.0F, 3.0F);

    pl1->push_back(point1);

    CPPUNIT_ASSERT_NO_THROW(pl2->shallow_copy(pl1));

    CPPUNIT_ASSERT_EQUAL(pl1->get_points()[0], pl2->get_points()[0]);
}

//------------------------------------------------------------------------------

void point_list_test::getter_test()
{
    data::point_list::sptr pl1 = std::make_shared<data::point_list>();
    data::point::sptr point1   = std::make_shared<data::point>(1.0F, 2.0F, 3.0F);

    pl1->push_back(point1);

    data::point::sptr point2 = pl1->get_points()[0];

    CPPUNIT_ASSERT_EQUAL(point1->get_coord()[0], point2->get_coord()[0]);
    CPPUNIT_ASSERT_EQUAL(point1->get_coord()[1], point2->get_coord()[1]);
    CPPUNIT_ASSERT_EQUAL(point1->get_coord()[2], point2->get_coord()[2]);

    data::point_list::container_t& container = pl1->get_points();

    CPPUNIT_ASSERT_EQUAL(point1->get_coord()[0], container[0]->get_coord()[0]);
    CPPUNIT_ASSERT_EQUAL(point1->get_coord()[1], container[0]->get_coord()[1]);
    CPPUNIT_ASSERT_EQUAL(point1->get_coord()[2], container[0]->get_coord()[2]);
}

//------------------------------------------------------------------------------

void point_list_test::setter_test()
{
    data::point_list::sptr pl1 = std::make_shared<data::point_list>();
    data::point::sptr point1   = std::make_shared<data::point>(1.0F, 2.0F, 3.0F);
    data::point::sptr point2   = std::make_shared<data::point>(4.0F, 5.0F, 6.0F);

    std::vector<data::point::sptr> vec;
    vec.push_back(point1);
    vec.push_back(point2);

    CPPUNIT_ASSERT_NO_THROW(pl1->set_points(vec));

    data::point_list::container_t& container = pl1->get_points();

    for(unsigned p = 0 ; p < vec.size() ; ++p)
    {
        for(unsigned int i = 0 ; i < 3 ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(vec[p]->get_coord()[i], container[p]->get_coord()[i]);
        }
    }
}

//------------------------------------------------------------------------------

void point_list_test::push_test()
{
    data::point_list::sptr pl1 = std::make_shared<data::point_list>();
    data::point::sptr point1   = std::make_shared<data::point>(1.0F, 2.0F, 3.0F);
    data::point::sptr point2   = std::make_shared<data::point>(4.0F, 5.0F, 6.0F);

    std::vector<data::point::sptr> vec;
    vec.push_back(point1);
    vec.push_back(point2);

    CPPUNIT_ASSERT_NO_THROW(pl1->push_back(point1));
    CPPUNIT_ASSERT_NO_THROW(pl1->push_back(point2));

    data::point_list::container_t& container = pl1->get_points();

    for(unsigned p = 0 ; p < vec.size() ; ++p)
    {
        for(unsigned int i = 0 ; i < 3 ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(vec[p]->get_coord()[i], container[p]->get_coord()[i]);
        }
    }
}

//------------------------------------------------------------------------------

void point_list_test::remove_test()
{
    const std::size_t nb_points = 42;
    data::point_list::sptr pl   = std::make_shared<data::point_list>();

    // Remove first
    {
        // Build a list
        for(std::size_t i = 0 ; i < nb_points ; ++i)
        {
            const auto p = std::make_shared<data::point>(.0F, .0F, .0F);
            pl->push_back(p);
        }

        // remove the first
        std::size_t size = nb_points;
        while(!pl->get_points().empty())
        {
            pl->remove(0);
            CPPUNIT_ASSERT_EQUAL(--size, pl->get_points().size());
        }
    }

    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(0), pl->get_points().size());

    // Remove last
    {
        // Build a list
        for(std::size_t i = 0 ; i < nb_points ; ++i)
        {
            const auto p = std::make_shared<data::point>(.0F, .0F, .0F);
            pl->push_back(p);
        }

        // remove the last
        std::size_t size = nb_points;
        while(!pl->get_points().empty())
        {
            const std::size_t index = pl->get_points().size() - 1;
            pl->remove(index);
            CPPUNIT_ASSERT_EQUAL(--size, pl->get_points().size());
        }
    }

    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(0), pl->get_points().size());

    // Check that the correct one is removed
    {
        // Build a list
        for(std::size_t i = 0 ; i < nb_points ; ++i)
        {
            const auto p = std::make_shared<data::point>(static_cast<float>(i), .0F, .0F);
            pl->push_back(p);
        }

        std::size_t size = nb_points;
        while(!pl->get_points().empty())
        {
            const std::size_t index = size / 2;
            const auto ref          = pl->get_points()[index];
            pl->remove(index);
            CPPUNIT_ASSERT_EQUAL(--size, pl->get_points().size());
            for(const auto& p : pl->get_points())
            {
                CPPUNIT_ASSERT(ref->get_coord()[0] != p->get_coord()[0]);
            }
        }
    }
}

//------------------------------------------------------------------------------

void point_list_test::clear_test()
{
    const std::size_t nb_points = 42;
    data::point_list::sptr pl   = std::make_shared<data::point_list>();

    CPPUNIT_ASSERT(pl->get_points().empty());
    pl->clear();
    CPPUNIT_ASSERT(pl->get_points().empty());

    // Build a list
    for(std::size_t i = 0 ; i < nb_points ; i++)
    {
        const auto p = std::make_shared<data::point>(.0F, .0F, .0F);
        pl->push_back(p);
    }

    CPPUNIT_ASSERT(pl->get_points().size() == nb_points);
    pl->clear();
    CPPUNIT_ASSERT(pl->get_points().empty());
}

} // namespace sight::data::ut
