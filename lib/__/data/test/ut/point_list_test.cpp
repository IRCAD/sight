/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <data/point.hpp>
#include <data/point_list.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::point_list")
{
//------------------------------------------------------------------------------

    TEST_CASE("copy")
    {
        sight::data::point_list::sptr pl1 = std::make_shared<sight::data::point_list>();
        sight::data::point_list::sptr pl2 = std::make_shared<sight::data::point_list>();
        sight::data::point_list::sptr pl3 = std::make_shared<sight::data::point_list>();

        sight::data::point::sptr point1 = std::make_shared<sight::data::point>(1.0F, 2.0F, 3.0F);

        pl1->push_back(point1);

        CHECK_NOTHROW(pl2->shallow_copy(pl1));

        CHECK_EQ(pl1->get_points()[0], pl2->get_points()[0]);
    }

//------------------------------------------------------------------------------

    TEST_CASE("getter")
    {
        sight::data::point_list::sptr pl1 = std::make_shared<sight::data::point_list>();
        sight::data::point::sptr point1   = std::make_shared<sight::data::point>(1.0F, 2.0F, 3.0F);

        pl1->push_back(point1);

        sight::data::point::sptr point2 = pl1->get_points()[0];

        CHECK_EQ((*point1)[0], (*point2)[0]);
        CHECK_EQ((*point1)[1], (*point2)[1]);
        CHECK_EQ((*point1)[2], (*point2)[2]);

        sight::data::point_list::container_t& container = pl1->get_points();

        CHECK_EQ((*point1)[0], (*container[0])[0]);
        CHECK_EQ((*point1)[1], (*container[0])[1]);
        CHECK_EQ((*point1)[2], (*container[0])[2]);
    }

//------------------------------------------------------------------------------

    TEST_CASE("setter")
    {
        sight::data::point_list::sptr pl1 = std::make_shared<sight::data::point_list>();
        sight::data::point::sptr point1   = std::make_shared<sight::data::point>(1.0F, 2.0F, 3.0F);
        sight::data::point::sptr point2   = std::make_shared<sight::data::point>(4.0F, 5.0F, 6.0F);

        std::vector<sight::data::point::sptr> vec;
        vec.push_back(point1);
        vec.push_back(point2);

        CHECK_NOTHROW(pl1->set_points(vec));

        sight::data::point_list::container_t& container = pl1->get_points();

        for(unsigned p = 0 ; p < vec.size() ; ++p)
        {
            for(unsigned int i = 0 ; i < 3 ; ++i)
            {
                CHECK_EQ((*vec[p])[i], (*container[p])[i]);
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("push")
    {
        sight::data::point_list::sptr pl1 = std::make_shared<sight::data::point_list>();
        sight::data::point::sptr point1   = std::make_shared<sight::data::point>(1.0F, 2.0F, 3.0F);
        sight::data::point::sptr point2   = std::make_shared<sight::data::point>(4.0F, 5.0F, 6.0F);

        std::vector<sight::data::point::sptr> vec;
        vec.push_back(point1);
        vec.push_back(point2);

        CHECK_NOTHROW(pl1->push_back(point1));
        CHECK_NOTHROW(pl1->push_back(point2));

        sight::data::point_list::container_t& container = pl1->get_points();

        for(unsigned p = 0 ; p < vec.size() ; ++p)
        {
            for(unsigned int i = 0 ; i < 3 ; ++i)
            {
                CHECK_EQ((*vec[p])[i], (*container[p])[i]);
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("remove")
    {
        const std::size_t nb_points      = 42;
        sight::data::point_list::sptr pl = std::make_shared<sight::data::point_list>();

        // Remove first
        {
            // Build a list
            for(std::size_t i = 0 ; i < nb_points ; ++i)
            {
                const auto p = std::make_shared<sight::data::point>(.0F, .0F, .0F);
                pl->push_back(p);
            }

            // remove the first
            std::size_t size = nb_points;
            while(!pl->get_points().empty())
            {
                pl->remove(0);
                CHECK_EQ(--size, pl->get_points().size());
            }
        }

        CHECK_EQ(static_cast<std::size_t>(0), pl->get_points().size());

        // Remove last
        {
            // Build a list
            for(std::size_t i = 0 ; i < nb_points ; ++i)
            {
                const auto p = std::make_shared<sight::data::point>(.0F, .0F, .0F);
                pl->push_back(p);
            }

            // remove the last
            std::size_t size = nb_points;
            while(!pl->get_points().empty())
            {
                const std::size_t index = pl->get_points().size() - 1;
                pl->remove(index);
                CHECK_EQ(--size, pl->get_points().size());
            }
        }

        CHECK_EQ(static_cast<std::size_t>(0), pl->get_points().size());

        // Check that the correct one is removed
        {
            // Build a list
            for(std::size_t i = 0 ; i < nb_points ; ++i)
            {
                const auto p = std::make_shared<sight::data::point>(static_cast<float>(i), .0F, .0F);
                pl->push_back(p);
            }

            std::size_t size = nb_points;
            while(!pl->get_points().empty())
            {
                const std::size_t index = size / 2;
                const auto ref          = pl->get_points()[index];
                pl->remove(index);
                CHECK_EQ(--size, pl->get_points().size());
                for(const auto& p : pl->get_points())
                {
                    CHECK((*ref)[0] != (*p)[0]);
                }
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("clear")
    {
        const std::size_t nb_points      = 42;
        sight::data::point_list::sptr pl = std::make_shared<sight::data::point_list>();

        CHECK(pl->get_points().empty());
        pl->clear();
        CHECK(pl->get_points().empty());

        // Build a list
        for(std::size_t i = 0 ; i < nb_points ; i++)
        {
            const auto p = std::make_shared<sight::data::point>(.0F, .0F, .0F);
            pl->push_back(p);
        }

        CHECK(pl->get_points().size() == nb_points);
        pl->clear();
        CHECK(pl->get_points().empty());
    }
} // TEST_SUITE("sight::data::point_list")
