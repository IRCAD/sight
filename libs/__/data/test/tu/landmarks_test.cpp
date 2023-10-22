/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

void landmarks_test::copyTest()
{
    data::landmarks::sptr landmarks                   = std::make_shared<data::landmarks>();
    static const std::string GROUP_1                  = "group_1";
    static const std::string GROUP_2                  = "group_2";
    static const std::string GROUP_3                  = "group_3";
    static constexpr data::landmarks::color_t COLOR_1 = {{1.F, 0.0F, 0.0F, 1.0F}};
    static constexpr data::landmarks::color_t COLOR_2 = {{0.F, 1.0F, 0.0F, 1.0F}};
    static constexpr data::landmarks::color_t COLOR_3 = {{0.F, 0.0F, 1.0F, 1.0F}};
    static constexpr data::landmarks::size_t SIZE_1   = 3.45F;
    static constexpr data::landmarks::size_t SIZE_2   = 7.5F;
    static constexpr data::landmarks::size_t SIZE_3   = 1.3F;
    static constexpr data::landmarks::Shape SHAPE_1   = data::landmarks::Shape::SPHERE;
    static constexpr data::landmarks::Shape SHAPE_2   = data::landmarks::Shape::CUBE;
    static constexpr data::landmarks::Shape SHAPE_3   = data::landmarks::Shape::SPHERE;
    static constexpr bool VISIBILITY_1                = true;
    static constexpr bool VISIBILITY_2                = false;
    static constexpr bool VISIBILITY_3                = true;
    static constexpr data::landmarks::point_t POINT_1 = {{3.5, 5.8, 2.56}};
    static constexpr data::landmarks::point_t POINT_2 = {{8.25, 56.0, 45.4}};
    static constexpr data::landmarks::point_t POINT_3 = {{0.0, 0.0, 0.0}};
    static constexpr data::landmarks::point_t POINT_4 = {{0.5, 0.6, 0.7}};

    CPPUNIT_ASSERT_NO_THROW(landmarks->addGroup(GROUP_1, COLOR_1, SIZE_1, SHAPE_1, VISIBILITY_1));
    CPPUNIT_ASSERT_NO_THROW(landmarks->addGroup(GROUP_2, COLOR_2, SIZE_2, SHAPE_2, VISIBILITY_2));
    CPPUNIT_ASSERT_NO_THROW(landmarks->addGroup(GROUP_3, COLOR_3, SIZE_3, SHAPE_3, VISIBILITY_3));
    CPPUNIT_ASSERT_NO_THROW(landmarks->addPoint(GROUP_1, POINT_1));
    CPPUNIT_ASSERT_NO_THROW(landmarks->addPoint(GROUP_2, POINT_2));
    CPPUNIT_ASSERT_NO_THROW(landmarks->addPoint(GROUP_3, POINT_3));
    CPPUNIT_ASSERT_NO_THROW(landmarks->addPoint(GROUP_1, POINT_4));

    CPPUNIT_ASSERT(landmarks);

    {
        // Check deep copy
        data::landmarks::csptr landmarks_copy = data::object::copy(landmarks);
        CPPUNIT_ASSERT(landmarks_copy);

        CPPUNIT_ASSERT_EQUAL(landmarks->numGroups(), landmarks_copy->numGroups());
        CPPUNIT_ASSERT_EQUAL(landmarks->numPoints(), landmarks_copy->numPoints());

        const data::landmarks::GroupNameContainer names = landmarks->getGroupNames();

        for(const auto& name : names)
        {
            CPPUNIT_ASSERT_NO_THROW(landmarks_copy->getGroup(name));

            const data::landmarks::LandmarksGroup& group      = landmarks->getGroup(name);
            const data::landmarks::LandmarksGroup& group_copy = landmarks_copy->getGroup(name);

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

        CPPUNIT_ASSERT_EQUAL(landmarks->numGroups(), landmarks_copy->numGroups());
        CPPUNIT_ASSERT_EQUAL(landmarks->numPoints(), landmarks_copy->numPoints());

        const data::landmarks::GroupNameContainer names = landmarks->getGroupNames();
        for(const auto& name : names)
        {
            CPPUNIT_ASSERT_NO_THROW(landmarks_copy->getGroup(name));

            const data::landmarks::LandmarksGroup& group      = landmarks->getGroup(name);
            const data::landmarks::LandmarksGroup& group_copy = landmarks_copy->getGroup(name);

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

void landmarks_test::groupTest()
{
    const std::string GROUP_1              = "group_1";
    const std::string GROUP_2              = "group_2";
    const std::string GROUP_3              = "group_3";
    const std::string no_group             = "no_group";
    const data::landmarks::color_t COLOR_1 = {{1.F, 0.0F, 0.0F, 1.0F}};
    const data::landmarks::color_t COLOR_2 = {{0.F, 1.0F, 0.0F, 1.0F}};
    const data::landmarks::color_t COLOR_3 = {{0.F, 0.0F, 1.0F, 1.0F}};
    const data::landmarks::size_t SIZE_1   = 3.45F;
    const data::landmarks::size_t SIZE_2   = 7.5F;
    const data::landmarks::size_t SIZE_3   = 1.3F;
    const data::landmarks::Shape SHAPE_1   = data::landmarks::Shape::SPHERE;
    const data::landmarks::Shape SHAPE_2   = data::landmarks::Shape::CUBE;
    const data::landmarks::Shape SHAPE_3   = data::landmarks::Shape::SPHERE;
    const bool VISIBILITY_1                = true;
    const bool VISIBILITY_2                = false;
    const bool VISIBILITY_3                = true;

    data::landmarks::sptr landmarks = std::make_shared<data::landmarks>();
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), landmarks->numGroups());
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), landmarks->numPoints());

    CPPUNIT_ASSERT_NO_THROW(landmarks->addGroup(GROUP_1, COLOR_1, SIZE_1, SHAPE_1, VISIBILITY_1));
    CPPUNIT_ASSERT_NO_THROW(landmarks->addGroup(GROUP_2, COLOR_2, SIZE_2, SHAPE_2, VISIBILITY_2));
    CPPUNIT_ASSERT_THROW(landmarks->addGroup(GROUP_1), data::exception);

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), landmarks->numGroups());
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), landmarks->numPoints());

    const data::landmarks::GroupNameContainer names = landmarks->getGroupNames();
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), names.size());

    CPPUNIT_ASSERT_EQUAL(GROUP_1, names[0]);
    CPPUNIT_ASSERT_EQUAL(GROUP_2, names[1]);

    // Check group 1
    CPPUNIT_ASSERT_NO_THROW(landmarks->getGroup(GROUP_1));
    const data::landmarks::LandmarksGroup& group1 = landmarks->getGroup(GROUP_1);
    CPPUNIT_ASSERT(COLOR_1 == group1.m_color);
    CPPUNIT_ASSERT_EQUAL(SIZE_1, group1.m_size);
    CPPUNIT_ASSERT_EQUAL(int(SHAPE_1), int(group1.m_shape));
    CPPUNIT_ASSERT_EQUAL(VISIBILITY_1, group1.m_visibility);

    // Check group 2
    CPPUNIT_ASSERT_NO_THROW(landmarks->getGroup(GROUP_2));
    const data::landmarks::LandmarksGroup& group2 = landmarks->getGroup(GROUP_2);
    CPPUNIT_ASSERT(COLOR_2 == group2.m_color);
    CPPUNIT_ASSERT_EQUAL(SIZE_2, group2.m_size);
    CPPUNIT_ASSERT_EQUAL(int(SHAPE_2), int(group2.m_shape));
    CPPUNIT_ASSERT_EQUAL(VISIBILITY_2, group2.m_visibility);

    // Check not existing group
    CPPUNIT_ASSERT_THROW(landmarks->getGroup(no_group), data::exception);

    // check rename of a non existing group
    CPPUNIT_ASSERT_THROW(landmarks->renameGroup(no_group, GROUP_3), data::exception);

    // check rename to an existing group
    CPPUNIT_ASSERT_THROW(landmarks->renameGroup(GROUP_1, GROUP_2), data::exception);

    // Remove group 1
    CPPUNIT_ASSERT_NO_THROW(landmarks->removeGroup(GROUP_1));
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), landmarks->numGroups());
    const data::landmarks::GroupNameContainer names2 = landmarks->getGroupNames();
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), names2.size());

    CPPUNIT_ASSERT_EQUAL(GROUP_2, names2[0]);

    // Rename group_2 to group_1
    CPPUNIT_ASSERT_NO_THROW(landmarks->renameGroup(GROUP_2, GROUP_3));

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), landmarks->numGroups());
    const data::landmarks::GroupNameContainer names3 = landmarks->getGroupNames();
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), names3.size());

    CPPUNIT_ASSERT_EQUAL(GROUP_3, names3[0]);

    {
        const data::landmarks::LandmarksGroup& group3 = landmarks->getGroup(GROUP_3);
        CPPUNIT_ASSERT(COLOR_2 == group3.m_color);
        CPPUNIT_ASSERT_EQUAL(SIZE_2, group3.m_size);
        CPPUNIT_ASSERT_EQUAL(int(SHAPE_2), int(group3.m_shape));
        CPPUNIT_ASSERT_EQUAL(VISIBILITY_2, group3.m_visibility);
    }

    // change color
    CPPUNIT_ASSERT_NO_THROW(landmarks->setGroupColor(GROUP_3, COLOR_3));

    {
        const data::landmarks::LandmarksGroup& group3 = landmarks->getGroup(GROUP_3);
        CPPUNIT_ASSERT(COLOR_3 == group3.m_color);
        CPPUNIT_ASSERT_EQUAL(SIZE_2, group3.m_size);
        CPPUNIT_ASSERT_EQUAL(int(SHAPE_2), int(group3.m_shape));
        CPPUNIT_ASSERT_EQUAL(VISIBILITY_2, group3.m_visibility);
    }

    // change size
    CPPUNIT_ASSERT_NO_THROW(landmarks->setGroupSize(GROUP_3, SIZE_3));

    {
        const data::landmarks::LandmarksGroup& group3 = landmarks->getGroup(GROUP_3);
        CPPUNIT_ASSERT(COLOR_3 == group3.m_color);
        CPPUNIT_ASSERT_EQUAL(SIZE_3, group3.m_size);
        CPPUNIT_ASSERT_EQUAL(int(SHAPE_2), int(group3.m_shape));
        CPPUNIT_ASSERT_EQUAL(VISIBILITY_2, group3.m_visibility);
    }

    // change shape
    CPPUNIT_ASSERT_NO_THROW(landmarks->setGroupShape(GROUP_3, SHAPE_3));

    {
        const data::landmarks::LandmarksGroup& group3 = landmarks->getGroup(GROUP_3);
        CPPUNIT_ASSERT(COLOR_3 == group3.m_color);
        CPPUNIT_ASSERT_EQUAL(SIZE_3, group3.m_size);
        CPPUNIT_ASSERT_EQUAL(int(SHAPE_3), int(group3.m_shape));
        CPPUNIT_ASSERT_EQUAL(VISIBILITY_2, group3.m_visibility);
    }

    // change shape
    CPPUNIT_ASSERT_NO_THROW(landmarks->setGroupVisibility(GROUP_3, VISIBILITY_3));

    {
        const data::landmarks::LandmarksGroup& group3 = landmarks->getGroup(GROUP_3);
        CPPUNIT_ASSERT(COLOR_3 == group3.m_color);
        CPPUNIT_ASSERT_EQUAL(SIZE_3, group3.m_size);
        CPPUNIT_ASSERT_EQUAL(int(SHAPE_3), int(group3.m_shape));
        CPPUNIT_ASSERT_EQUAL(VISIBILITY_3, group3.m_visibility);
    }
}

//------------------------------------------------------------------------------

void landmarks_test::pointsTest()
{
    const std::string GROUP_1  = "group_1";
    const std::string GROUP_2  = "group_2";
    const std::string GROUP_3  = "group_3";
    const std::string no_group = "no_group";

    const data::landmarks::point_t POINT_1 = {{3.5, 5.8, 2.56}};
    const data::landmarks::point_t POINT_2 = {{8.25, 56.0, 45.4}};
    const data::landmarks::point_t POINT_3 = {{0.0, 0.0, 0.0}};
    const data::landmarks::point_t POINT_4 = {{0.5, 0.6, 0.7}};

    data::landmarks::sptr landmarks = std::make_shared<data::landmarks>();

    CPPUNIT_ASSERT_NO_THROW(landmarks->addGroup(GROUP_1));
    CPPUNIT_ASSERT_NO_THROW(landmarks->addGroup(GROUP_2));

    CPPUNIT_ASSERT_THROW(landmarks->addPoint(no_group, POINT_1), data::exception);

    CPPUNIT_ASSERT_NO_THROW(landmarks->addPoint(GROUP_1, POINT_1));
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), landmarks->numPoints(GROUP_1));
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), landmarks->numPoints());

    const data::landmarks::PointContainer& points = landmarks->getPoints(GROUP_1);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), points.size());
    CPPUNIT_ASSERT(POINT_1 == points[0]);
    CPPUNIT_ASSERT(POINT_1 == landmarks->getPoint(GROUP_1, 0));
    CPPUNIT_ASSERT_THROW(landmarks->getPoint(GROUP_1, 23), std::out_of_range);

    CPPUNIT_ASSERT_NO_THROW(landmarks->addPoint(GROUP_2, POINT_2));
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), landmarks->numPoints(GROUP_2));
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), landmarks->numPoints());

    CPPUNIT_ASSERT_NO_THROW(landmarks->addPoint(GROUP_2, POINT_3));
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), landmarks->numPoints(GROUP_2));
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), landmarks->numPoints());

    const data::landmarks::PointContainer& points2 = landmarks->getPoints(GROUP_2);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), points2.size());
    CPPUNIT_ASSERT(POINT_2 == points2[0]);
    CPPUNIT_ASSERT(POINT_3 == points2[1]);
    CPPUNIT_ASSERT(POINT_2 == landmarks->getPoint(GROUP_2, 0));
    CPPUNIT_ASSERT(POINT_3 == landmarks->getPoint(GROUP_2, 1));

    // Check rename group
    CPPUNIT_ASSERT_NO_THROW(landmarks->renameGroup(GROUP_2, GROUP_3));
    const data::landmarks::PointContainer& points3 = landmarks->getPoints(GROUP_3);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), points3.size());
    CPPUNIT_ASSERT(POINT_2 == points3[0]);
    CPPUNIT_ASSERT(POINT_3 == points3[1]);
    CPPUNIT_ASSERT(POINT_2 == landmarks->getPoint(GROUP_3, 0));
    CPPUNIT_ASSERT(POINT_3 == landmarks->getPoint(GROUP_3, 1));

    // Check remove point
    CPPUNIT_ASSERT_THROW(landmarks->removePoint(no_group, 0), data::exception);
    CPPUNIT_ASSERT_THROW(landmarks->removePoint(GROUP_1, 2), std::out_of_range);
    CPPUNIT_ASSERT_NO_THROW(landmarks->removePoint(GROUP_3, 1));

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), landmarks->numPoints(GROUP_3));
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), landmarks->numPoints());
    CPPUNIT_ASSERT(POINT_2 == landmarks->getPoint(GROUP_3, 0));

    // Check remove group
    CPPUNIT_ASSERT_NO_THROW(landmarks->removeGroup(GROUP_3));
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), landmarks->numPoints(GROUP_1));
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), landmarks->numPoints());
    CPPUNIT_ASSERT_THROW(landmarks->getPoints(GROUP_3), data::exception);

    // Check insert point
    CPPUNIT_ASSERT_NO_THROW(landmarks->insertPoint(GROUP_1, 0, POINT_4));
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), landmarks->numPoints(GROUP_1));
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), landmarks->numPoints());

    const data::landmarks::PointContainer& points4 = landmarks->getPoints(GROUP_1);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), points4.size());
    CPPUNIT_ASSERT(POINT_4 == points4[0]);
    CPPUNIT_ASSERT(POINT_1 == points4[1]);
    CPPUNIT_ASSERT(POINT_4 == landmarks->getPoint(GROUP_1, 0));
    CPPUNIT_ASSERT(POINT_1 == landmarks->getPoint(GROUP_1, 1));

    data::landmarks::point_t& point = landmarks->getPoint(GROUP_1, 0);
    point[2] = 18.6;
    CPPUNIT_ASSERT_EQUAL(18.6, landmarks->getPoint(GROUP_1, 0)[2]);

    // check clear points
    CPPUNIT_ASSERT_NO_THROW(landmarks->clearPoints(GROUP_1));
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), landmarks->numPoints(GROUP_1));
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), landmarks->numPoints());
}

//------------------------------------------------------------------------------

void landmarks_test::equalityTest()
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

    TEST(addGroup("1"));
    TEST(renameGroup("1", "2"));
    TEST(setGroupColor("2", {0, 1, 1, 1}));
    TEST(setGroupColor("2", {0, 0, 1, 1}));
    TEST(setGroupColor("2", {0, 0, 0, 1}));
    TEST(setGroupColor("2", {0, 0, 0, 0}));
    TEST(setGroupSize("2", 2));
    TEST(setGroupShape("2", data::landmarks::Shape::CUBE));
    TEST(setGroupVisibility("2", false));
    TEST(addPoint("2", {1, 2, 3}));
    TEST(clearPoints("2"));
    TEST(removeGroup("2"));

    #undef TEST
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut
