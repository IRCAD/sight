/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
 * Copyright (C) 2017-2021 IHU Strasbourg
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

#include "landmarks_test.hpp"

#include <data/exception.hpp>
#include <data/landmarks.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::landmarks_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void landmarks_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void landmarks_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void landmarks_test::copy_test()
{
    data::landmarks::sptr landmarks                     = std::make_shared<data::landmarks>();
    static const std::string s_GROUP_1                  = "group_1";
    static const std::string s_GROUP_2                  = "group_2";
    static const std::string s_GROUP_3                  = "group_3";
    static constexpr data::landmarks::color_t s_COLOR_1 = {{1.F, 0.0F, 0.0F, 1.0F}};
    static constexpr data::landmarks::color_t s_COLOR_2 = {{0.F, 1.0F, 0.0F, 1.0F}};
    static constexpr data::landmarks::color_t s_COLOR_3 = {{0.F, 0.0F, 1.0F, 1.0F}};
    static constexpr data::landmarks::size_t s_SIZE_1   = 3.45F;
    static constexpr data::landmarks::size_t s_SIZE_2   = 7.5F;
    static constexpr data::landmarks::size_t s_SIZE_3   = 1.3F;
    static constexpr data::landmarks::shape s_SHAPE_1   = data::landmarks::shape::sphere;
    static constexpr data::landmarks::shape s_SHAPE_2   = data::landmarks::shape::cube;
    static constexpr data::landmarks::shape s_SHAPE_3   = data::landmarks::shape::sphere;
    static constexpr bool s_VISIBILITY_1                = true;
    static constexpr bool s_VISIBILITY_2                = false;
    static constexpr bool s_VISIBILITY_3                = true;
    static constexpr data::landmarks::point_t s_POINT_1 = {{3.5, 5.8, 2.56}};
    static constexpr data::landmarks::point_t s_POINT_2 = {{8.25, 56.0, 45.4}};
    static constexpr data::landmarks::point_t s_POINT_3 = {{0.0, 0.0, 0.0}};
    static constexpr data::landmarks::point_t s_POINT_4 = {{0.5, 0.6, 0.7}};

    CPPUNIT_ASSERT_NO_THROW(landmarks->add_group(s_GROUP_1, s_COLOR_1, s_SIZE_1, s_SHAPE_1, s_VISIBILITY_1));
    CPPUNIT_ASSERT_NO_THROW(landmarks->add_group(s_GROUP_2, s_COLOR_2, s_SIZE_2, s_SHAPE_2, s_VISIBILITY_2));
    CPPUNIT_ASSERT_NO_THROW(landmarks->add_group(s_GROUP_3, s_COLOR_3, s_SIZE_3, s_SHAPE_3, s_VISIBILITY_3));
    CPPUNIT_ASSERT_NO_THROW(landmarks->add_point(s_GROUP_1, s_POINT_1));
    CPPUNIT_ASSERT_NO_THROW(landmarks->add_point(s_GROUP_2, s_POINT_2));
    CPPUNIT_ASSERT_NO_THROW(landmarks->add_point(s_GROUP_3, s_POINT_3));
    CPPUNIT_ASSERT_NO_THROW(landmarks->add_point(s_GROUP_1, s_POINT_4));

    CPPUNIT_ASSERT(landmarks);

    {
        // Check deep copy
        data::landmarks::csptr landmarks_copy = data::object::copy(landmarks);
        CPPUNIT_ASSERT(landmarks_copy);

        CPPUNIT_ASSERT_EQUAL(landmarks->num_groups(), landmarks_copy->num_groups());
        CPPUNIT_ASSERT_EQUAL(landmarks->num_points(), landmarks_copy->num_points());

        const data::landmarks::GroupNameContainer names = landmarks->get_group_names();

        for(const auto& name : names)
        {
            CPPUNIT_ASSERT_NO_THROW(landmarks_copy->get_group(name));

            const data::landmarks::landmarks_group& group      = landmarks->get_group(name);
            const data::landmarks::landmarks_group& group_copy = landmarks_copy->get_group(name);

            CPPUNIT_ASSERT(group.m_color == group_copy.m_color);
            CPPUNIT_ASSERT_EQUAL(group.m_points.size(), group_copy.m_points.size());
            CPPUNIT_ASSERT(group.m_points == group_copy.m_points);
            CPPUNIT_ASSERT_EQUAL(group.m_size, group_copy.m_size);
            CPPUNIT_ASSERT_EQUAL(int(group.m_shape), int(group_copy.m_shape));
            CPPUNIT_ASSERT_EQUAL(group.m_visibility, group_copy.m_visibility);
        }

        CPPUNIT_ASSERT(*landmarks_copy == *landmarks);
    }

    {
        // Check shallow copy
        data::landmarks::sptr landmarks_copy = std::make_shared<data::landmarks>();
        CPPUNIT_ASSERT(*landmarks_copy != *landmarks);

        landmarks_copy->shallow_copy(landmarks);
        CPPUNIT_ASSERT(landmarks_copy);

        CPPUNIT_ASSERT_EQUAL(landmarks->num_groups(), landmarks_copy->num_groups());
        CPPUNIT_ASSERT_EQUAL(landmarks->num_points(), landmarks_copy->num_points());

        const data::landmarks::GroupNameContainer names = landmarks->get_group_names();
        for(const auto& name : names)
        {
            CPPUNIT_ASSERT_NO_THROW(landmarks_copy->get_group(name));

            const data::landmarks::landmarks_group& group      = landmarks->get_group(name);
            const data::landmarks::landmarks_group& group_copy = landmarks_copy->get_group(name);

            CPPUNIT_ASSERT(group.m_color == group_copy.m_color);
            CPPUNIT_ASSERT_EQUAL(group.m_points.size(), group_copy.m_points.size());
            CPPUNIT_ASSERT(group.m_points == group_copy.m_points);
            CPPUNIT_ASSERT_EQUAL(group.m_size, group_copy.m_size);
            CPPUNIT_ASSERT_EQUAL(int(group.m_shape), int(group_copy.m_shape));
            CPPUNIT_ASSERT_EQUAL(group.m_visibility, group_copy.m_visibility);
        }

        CPPUNIT_ASSERT(*landmarks_copy == *landmarks);
    }
}

//------------------------------------------------------------------------------

void landmarks_test::group_test()
{
    const std::string group_1              = "group_1";
    const std::string group_2              = "group_2";
    const std::string group_3              = "group_3";
    const std::string no_group             = "no_group";
    const data::landmarks::color_t color_1 = {{1.F, 0.0F, 0.0F, 1.0F}};
    const data::landmarks::color_t color_2 = {{0.F, 1.0F, 0.0F, 1.0F}};
    const data::landmarks::color_t color_3 = {{0.F, 0.0F, 1.0F, 1.0F}};
    const data::landmarks::size_t size_1   = 3.45F;
    const data::landmarks::size_t size_2   = 7.5F;
    const data::landmarks::size_t size_3   = 1.3F;
    const data::landmarks::shape shape_1   = data::landmarks::shape::sphere;
    const data::landmarks::shape shape_2   = data::landmarks::shape::cube;
    const data::landmarks::shape shape_3   = data::landmarks::shape::sphere;
    const bool visibility_1                = true;
    const bool visibility_2                = false;
    const bool visibility_3                = true;

    data::landmarks::sptr landmarks = std::make_shared<data::landmarks>();
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), landmarks->num_groups());
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), landmarks->num_points());

    CPPUNIT_ASSERT_NO_THROW(landmarks->add_group(group_1, color_1, size_1, shape_1, visibility_1));
    CPPUNIT_ASSERT_NO_THROW(landmarks->add_group(group_2, color_2, size_2, shape_2, visibility_2));
    CPPUNIT_ASSERT_THROW(landmarks->add_group(group_1), data::exception);

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), landmarks->num_groups());
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), landmarks->num_points());

    const data::landmarks::GroupNameContainer names = landmarks->get_group_names();
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), names.size());

    CPPUNIT_ASSERT_EQUAL(group_1, names[0]);
    CPPUNIT_ASSERT_EQUAL(group_2, names[1]);

    // Check group 1
    CPPUNIT_ASSERT_NO_THROW(landmarks->get_group(group_1));
    const data::landmarks::landmarks_group& group1 = landmarks->get_group(group_1);
    CPPUNIT_ASSERT(color_1 == group1.m_color);
    CPPUNIT_ASSERT_EQUAL(size_1, group1.m_size);
    CPPUNIT_ASSERT_EQUAL(int(shape_1), int(group1.m_shape));
    CPPUNIT_ASSERT_EQUAL(visibility_1, group1.m_visibility);

    // Check group 2
    CPPUNIT_ASSERT_NO_THROW(landmarks->get_group(group_2));
    const data::landmarks::landmarks_group& group2 = landmarks->get_group(group_2);
    CPPUNIT_ASSERT(color_2 == group2.m_color);
    CPPUNIT_ASSERT_EQUAL(size_2, group2.m_size);
    CPPUNIT_ASSERT_EQUAL(int(shape_2), int(group2.m_shape));
    CPPUNIT_ASSERT_EQUAL(visibility_2, group2.m_visibility);

    // Check not existing group
    CPPUNIT_ASSERT_THROW(landmarks->get_group(no_group), data::exception);

    // check rename of a non existing group
    CPPUNIT_ASSERT_THROW(landmarks->rename_group(no_group, group_3), data::exception);

    // check rename to an existing group
    CPPUNIT_ASSERT_THROW(landmarks->rename_group(group_1, group_2), data::exception);

    // Remove group 1
    CPPUNIT_ASSERT_NO_THROW(landmarks->remove_group(group_1));
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), landmarks->num_groups());
    const data::landmarks::GroupNameContainer names2 = landmarks->get_group_names();
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), names2.size());

    CPPUNIT_ASSERT_EQUAL(group_2, names2[0]);

    // Rename group_2 to group_1
    CPPUNIT_ASSERT_NO_THROW(landmarks->rename_group(group_2, group_3));

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), landmarks->num_groups());
    const data::landmarks::GroupNameContainer names3 = landmarks->get_group_names();
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), names3.size());

    CPPUNIT_ASSERT_EQUAL(group_3, names3[0]);

    {
        const data::landmarks::landmarks_group& group3 = landmarks->get_group(group_3);
        CPPUNIT_ASSERT(color_2 == group3.m_color);
        CPPUNIT_ASSERT_EQUAL(size_2, group3.m_size);
        CPPUNIT_ASSERT_EQUAL(int(shape_2), int(group3.m_shape));
        CPPUNIT_ASSERT_EQUAL(visibility_2, group3.m_visibility);
    }

    // change color
    CPPUNIT_ASSERT_NO_THROW(landmarks->set_group_color(group_3, color_3));

    {
        const data::landmarks::landmarks_group& group3 = landmarks->get_group(group_3);
        CPPUNIT_ASSERT(color_3 == group3.m_color);
        CPPUNIT_ASSERT_EQUAL(size_2, group3.m_size);
        CPPUNIT_ASSERT_EQUAL(int(shape_2), int(group3.m_shape));
        CPPUNIT_ASSERT_EQUAL(visibility_2, group3.m_visibility);
    }

    // change size
    CPPUNIT_ASSERT_NO_THROW(landmarks->set_group_size(group_3, size_3));

    {
        const data::landmarks::landmarks_group& group3 = landmarks->get_group(group_3);
        CPPUNIT_ASSERT(color_3 == group3.m_color);
        CPPUNIT_ASSERT_EQUAL(size_3, group3.m_size);
        CPPUNIT_ASSERT_EQUAL(int(shape_2), int(group3.m_shape));
        CPPUNIT_ASSERT_EQUAL(visibility_2, group3.m_visibility);
    }

    // change shape
    CPPUNIT_ASSERT_NO_THROW(landmarks->set_group_shape(group_3, shape_3));

    {
        const data::landmarks::landmarks_group& group3 = landmarks->get_group(group_3);
        CPPUNIT_ASSERT(color_3 == group3.m_color);
        CPPUNIT_ASSERT_EQUAL(size_3, group3.m_size);
        CPPUNIT_ASSERT_EQUAL(int(shape_3), int(group3.m_shape));
        CPPUNIT_ASSERT_EQUAL(visibility_2, group3.m_visibility);
    }

    // change shape
    CPPUNIT_ASSERT_NO_THROW(landmarks->set_group_visibility(group_3, visibility_3));

    {
        const data::landmarks::landmarks_group& group3 = landmarks->get_group(group_3);
        CPPUNIT_ASSERT(color_3 == group3.m_color);
        CPPUNIT_ASSERT_EQUAL(size_3, group3.m_size);
        CPPUNIT_ASSERT_EQUAL(int(shape_3), int(group3.m_shape));
        CPPUNIT_ASSERT_EQUAL(visibility_3, group3.m_visibility);
    }
}

//------------------------------------------------------------------------------

void landmarks_test::points_test()
{
    const std::string group_1  = "group_1";
    const std::string group_2  = "group_2";
    const std::string group_3  = "group_3";
    const std::string no_group = "no_group";

    const data::landmarks::point_t point_1 = {{3.5, 5.8, 2.56}};
    const data::landmarks::point_t point_2 = {{8.25, 56.0, 45.4}};
    const data::landmarks::point_t point_3 = {{0.0, 0.0, 0.0}};
    const data::landmarks::point_t point_4 = {{0.5, 0.6, 0.7}};

    data::landmarks::sptr landmarks = std::make_shared<data::landmarks>();

    CPPUNIT_ASSERT_NO_THROW(landmarks->add_group(group_1));
    CPPUNIT_ASSERT_NO_THROW(landmarks->add_group(group_2));

    CPPUNIT_ASSERT_THROW(landmarks->add_point(no_group, point_1), data::exception);

    CPPUNIT_ASSERT_NO_THROW(landmarks->add_point(group_1, point_1));
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), landmarks->num_points(group_1));
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), landmarks->num_points());

    const data::landmarks::point_container& points = landmarks->get_points(group_1);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), points.size());
    CPPUNIT_ASSERT(point_1 == points[0]);
    CPPUNIT_ASSERT(point_1 == landmarks->get_point(group_1, 0));
    CPPUNIT_ASSERT_THROW(landmarks->get_point(group_1, 23), std::out_of_range);

    CPPUNIT_ASSERT_NO_THROW(landmarks->add_point(group_2, point_2));
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), landmarks->num_points(group_2));
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), landmarks->num_points());

    CPPUNIT_ASSERT_NO_THROW(landmarks->add_point(group_2, point_3));
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), landmarks->num_points(group_2));
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), landmarks->num_points());

    const data::landmarks::point_container& points2 = landmarks->get_points(group_2);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), points2.size());
    CPPUNIT_ASSERT(point_2 == points2[0]);
    CPPUNIT_ASSERT(point_3 == points2[1]);
    CPPUNIT_ASSERT(point_2 == landmarks->get_point(group_2, 0));
    CPPUNIT_ASSERT(point_3 == landmarks->get_point(group_2, 1));

    // Check rename group
    CPPUNIT_ASSERT_NO_THROW(landmarks->rename_group(group_2, group_3));
    const data::landmarks::point_container& points3 = landmarks->get_points(group_3);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), points3.size());
    CPPUNIT_ASSERT(point_2 == points3[0]);
    CPPUNIT_ASSERT(point_3 == points3[1]);
    CPPUNIT_ASSERT(point_2 == landmarks->get_point(group_3, 0));
    CPPUNIT_ASSERT(point_3 == landmarks->get_point(group_3, 1));

    // Check remove point
    CPPUNIT_ASSERT_THROW(landmarks->remove_point(no_group, 0), data::exception);
    CPPUNIT_ASSERT_THROW(landmarks->remove_point(group_1, 2), std::out_of_range);
    CPPUNIT_ASSERT_NO_THROW(landmarks->remove_point(group_3, 1));

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), landmarks->num_points(group_3));
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), landmarks->num_points());
    CPPUNIT_ASSERT(point_2 == landmarks->get_point(group_3, 0));

    // Check remove group
    CPPUNIT_ASSERT_NO_THROW(landmarks->remove_group(group_3));
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), landmarks->num_points(group_1));
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), landmarks->num_points());
    CPPUNIT_ASSERT_THROW(landmarks->get_points(group_3), data::exception);

    // Check insert point
    CPPUNIT_ASSERT_NO_THROW(landmarks->insert_point(group_1, 0, point_4));
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), landmarks->num_points(group_1));
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), landmarks->num_points());

    const data::landmarks::point_container& points4 = landmarks->get_points(group_1);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), points4.size());
    CPPUNIT_ASSERT(point_4 == points4[0]);
    CPPUNIT_ASSERT(point_1 == points4[1]);
    CPPUNIT_ASSERT(point_4 == landmarks->get_point(group_1, 0));
    CPPUNIT_ASSERT(point_1 == landmarks->get_point(group_1, 1));

    data::landmarks::point_t& point = landmarks->get_point(group_1, 0);
    point[2] = 18.6;
    CPPUNIT_ASSERT_EQUAL(18.6, landmarks->get_point(group_1, 0)[2]);

    // check clear points
    CPPUNIT_ASSERT_NO_THROW(landmarks->clear_points(group_1));
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), landmarks->num_points(group_1));
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), landmarks->num_points());
}

//------------------------------------------------------------------------------

void landmarks_test::equality_test()
{
    auto landmarks1 = std::make_shared<data::landmarks>();
    auto landmarks2 = std::make_shared<data::landmarks>();

    CPPUNIT_ASSERT(*landmarks1 == *landmarks2 && !(*landmarks1 != *landmarks2));

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(op) \
            landmarks1->op; \
            CPPUNIT_ASSERT_MESSAGE( \
                "The landmarks must be different when using " #op " on the first landmarks", \
                *landmarks1 != *landmarks2 && !(*landmarks1 == *landmarks2) \
            ); \
            landmarks2->op; \
            CPPUNIT_ASSERT_MESSAGE( \
                "The landmarks must be equal when using " #op " on the both landmarks", \
                *landmarks1 == *landmarks2 && !(*landmarks1 != *landmarks2) \
            );

    TEST(add_group("1"));
    TEST(rename_group("1", "2"));
    TEST(set_group_color("2", {0, 1, 1, 1}));
    TEST(set_group_color("2", {0, 0, 1, 1}));
    TEST(set_group_color("2", {0, 0, 0, 1}));
    TEST(set_group_color("2", {0, 0, 0, 0}));
    TEST(set_group_size("2", 2));
    TEST(set_group_shape("2", data::landmarks::shape::cube));
    TEST(set_group_visibility("2", false));
    TEST(add_point("2", {1, 2, 3}));
    TEST(clear_points("2"));
    TEST(remove_group("2"));

    #undef TEST
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut
