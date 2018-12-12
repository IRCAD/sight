/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include "LandmarksTest.hpp"

#include <fwData/Exception.hpp>
#include <fwData/Landmarks.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::LandmarksTest );

namespace fwData
{
namespace ut
{

//------------------------------------------------------------------------------

void LandmarksTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void LandmarksTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void LandmarksTest::copyTest()
{
    ::fwData::Landmarks::sptr landmarks = ::fwData::Landmarks::New();
    const std::string GROUP1                    = "group_1";
    const std::string GROUP2                    = "group_2";
    const std::string GROUP3                    = "group_3";
    const ::fwData::Landmarks::ColorType COLOR1 = {{1.f, 0.0f, 0.0f, 1.0f}};
    const ::fwData::Landmarks::ColorType COLOR2 = {{0.f, 1.0f, 0.0f, 1.0f}};
    const ::fwData::Landmarks::ColorType COLOR3 = {{0.f, 0.0f, 1.0f, 1.0f}};
    const ::fwData::Landmarks::SizeType SIZE1   = 3.45f;
    const ::fwData::Landmarks::SizeType SIZE2   = 7.5f;
    const ::fwData::Landmarks::SizeType SIZE3   = 1.3f;
    const ::fwData::Landmarks::Shape SHAPE1     = ::fwData::Landmarks::Shape::SPHERE;
    const ::fwData::Landmarks::Shape SHAPE2     = ::fwData::Landmarks::Shape::CUBE;
    const ::fwData::Landmarks::Shape SHAPE3     = ::fwData::Landmarks::Shape::SPHERE;
    const bool VISIBILITY1                      = true;
    const bool VISIBILITY2                      = false;
    const bool VISIBILITY3                      = true;
    const ::fwData::Landmarks::PointType POINT1 = {{3.5, 5.8, 2.56}};
    const ::fwData::Landmarks::PointType POINT2 = {{8.25, 56.0, 45.4}};
    const ::fwData::Landmarks::PointType POINT3 = {{0.0, 0.0, 0.0}};
    const ::fwData::Landmarks::PointType POINT4 = {{0.5, 0.6, 0.7}};

    CPPUNIT_ASSERT_NO_THROW(landmarks->addGroup(GROUP1, COLOR1, SIZE1, SHAPE1, VISIBILITY1));
    CPPUNIT_ASSERT_NO_THROW(landmarks->addGroup(GROUP2, COLOR2, SIZE2, SHAPE2, VISIBILITY2));
    CPPUNIT_ASSERT_NO_THROW(landmarks->addGroup(GROUP3, COLOR3, SIZE3, SHAPE3, VISIBILITY3));
    CPPUNIT_ASSERT_NO_THROW(landmarks->addPoint(GROUP1, POINT1));
    CPPUNIT_ASSERT_NO_THROW(landmarks->addPoint(GROUP2, POINT2));
    CPPUNIT_ASSERT_NO_THROW(landmarks->addPoint(GROUP3, POINT3));
    CPPUNIT_ASSERT_NO_THROW(landmarks->addPoint(GROUP1, POINT4));

    CPPUNIT_ASSERT(landmarks);

    {
        // Check deep copy
        ::fwData::Landmarks::csptr landmarksCopy = ::fwData::Object::copy(landmarks);
        CPPUNIT_ASSERT(landmarksCopy);

        CPPUNIT_ASSERT_EQUAL(landmarks->getNumberOfGroups(), landmarksCopy->getNumberOfGroups());
        CPPUNIT_ASSERT_EQUAL(landmarks->getNumberOfPoints(), landmarksCopy->getNumberOfPoints());

        const ::fwData::Landmarks::GroupNameContainer names = landmarks->getGroupNames();

        for (const auto& name : names)
        {
            CPPUNIT_ASSERT_NO_THROW(landmarksCopy->getGroup(name));

            const ::fwData::Landmarks::LandmarksGroup& group     = landmarks->getGroup(name);
            const ::fwData::Landmarks::LandmarksGroup& groupCopy = landmarksCopy->getGroup(name);

            CPPUNIT_ASSERT(group.m_color == groupCopy.m_color);
            CPPUNIT_ASSERT_EQUAL(group.m_points.size(), groupCopy.m_points.size());
            CPPUNIT_ASSERT(group.m_points == groupCopy.m_points);
            CPPUNIT_ASSERT_EQUAL(group.m_size, groupCopy.m_size);
            CPPUNIT_ASSERT_EQUAL(int(group.m_shape), int(groupCopy.m_shape));
            CPPUNIT_ASSERT_EQUAL(group.m_visibility, groupCopy.m_visibility);
        }
    }

    {
        // Check shallow copy
        ::fwData::Landmarks::sptr landmarksCopy = ::fwData::Landmarks::New();
        landmarksCopy->shallowCopy(landmarks);
        CPPUNIT_ASSERT(landmarksCopy);

        CPPUNIT_ASSERT_EQUAL(landmarks->getNumberOfGroups(), landmarksCopy->getNumberOfGroups());
        CPPUNIT_ASSERT_EQUAL(landmarks->getNumberOfPoints(), landmarksCopy->getNumberOfPoints());

        const ::fwData::Landmarks::GroupNameContainer names = landmarks->getGroupNames();
        for (const auto& name : names)
        {
            CPPUNIT_ASSERT_NO_THROW(landmarksCopy->getGroup(name));

            const ::fwData::Landmarks::LandmarksGroup& group     = landmarks->getGroup(name);
            const ::fwData::Landmarks::LandmarksGroup& groupCopy = landmarksCopy->getGroup(name);

            CPPUNIT_ASSERT(group.m_color == groupCopy.m_color);
            CPPUNIT_ASSERT_EQUAL(group.m_points.size(), groupCopy.m_points.size());
            CPPUNIT_ASSERT(group.m_points == groupCopy.m_points);
            CPPUNIT_ASSERT_EQUAL(group.m_size, groupCopy.m_size);
            CPPUNIT_ASSERT_EQUAL(int(group.m_shape), int(groupCopy.m_shape));
            CPPUNIT_ASSERT_EQUAL(group.m_visibility, groupCopy.m_visibility);
        }
    }
}

//------------------------------------------------------------------------------

void LandmarksTest::groupTest()
{
    const std::string GROUP1                    = "group_1";
    const std::string GROUP2                    = "group_2";
    const std::string GROUP3                    = "group_3";
    const std::string NO_GROUP                  = "no_group";
    const ::fwData::Landmarks::ColorType COLOR1 = {{1.f, 0.0f, 0.0f, 1.0f}};
    const ::fwData::Landmarks::ColorType COLOR2 = {{0.f, 1.0f, 0.0f, 1.0f}};
    const ::fwData::Landmarks::ColorType COLOR3 = {{0.f, 0.0f, 1.0f, 1.0f}};
    const ::fwData::Landmarks::SizeType SIZE1   = 3.45f;
    const ::fwData::Landmarks::SizeType SIZE2   = 7.5f;
    const ::fwData::Landmarks::SizeType SIZE3   = 1.3f;
    const ::fwData::Landmarks::Shape SHAPE1     = ::fwData::Landmarks::Shape::SPHERE;
    const ::fwData::Landmarks::Shape SHAPE2     = ::fwData::Landmarks::Shape::CUBE;
    const ::fwData::Landmarks::Shape SHAPE3     = ::fwData::Landmarks::Shape::SPHERE;
    const bool VISIBILITY1                      = true;
    const bool VISIBILITY2                      = false;
    const bool VISIBILITY3                      = true;

    ::fwData::Landmarks::sptr landmarks = ::fwData::Landmarks::New();
    CPPUNIT_ASSERT_EQUAL(size_t(0), landmarks->getNumberOfGroups());
    CPPUNIT_ASSERT_EQUAL(size_t(0), landmarks->getNumberOfPoints());

    CPPUNIT_ASSERT_NO_THROW(landmarks->addGroup(GROUP1, COLOR1, SIZE1, SHAPE1, VISIBILITY1));
    CPPUNIT_ASSERT_NO_THROW(landmarks->addGroup(GROUP2, COLOR2, SIZE2, SHAPE2, VISIBILITY2));
    CPPUNIT_ASSERT_THROW(landmarks->addGroup(GROUP1), ::fwData::Exception);

    CPPUNIT_ASSERT_EQUAL(size_t(2), landmarks->getNumberOfGroups());
    CPPUNIT_ASSERT_EQUAL(size_t(0), landmarks->getNumberOfPoints());

    const ::fwData::Landmarks::GroupNameContainer names = landmarks->getGroupNames();
    CPPUNIT_ASSERT_EQUAL(size_t(2), names.size());

    CPPUNIT_ASSERT_EQUAL(GROUP1, names[0]);
    CPPUNIT_ASSERT_EQUAL(GROUP2, names[1]);

    // Check group 1
    CPPUNIT_ASSERT_NO_THROW(landmarks->getGroup(GROUP1));
    const ::fwData::Landmarks::LandmarksGroup& group1 = landmarks->getGroup(GROUP1);
    CPPUNIT_ASSERT(COLOR1 == group1.m_color);
    CPPUNIT_ASSERT_EQUAL(SIZE1, group1.m_size);
    CPPUNIT_ASSERT_EQUAL(int(SHAPE1), int(group1.m_shape));
    CPPUNIT_ASSERT_EQUAL(VISIBILITY1, group1.m_visibility);

    // Check group 2
    CPPUNIT_ASSERT_NO_THROW(landmarks->getGroup(GROUP2));
    const ::fwData::Landmarks::LandmarksGroup& group2 = landmarks->getGroup(GROUP2);
    CPPUNIT_ASSERT(COLOR2 == group2.m_color);
    CPPUNIT_ASSERT_EQUAL(SIZE2, group2.m_size);
    CPPUNIT_ASSERT_EQUAL(int(SHAPE2), int(group2.m_shape));
    CPPUNIT_ASSERT_EQUAL(VISIBILITY2, group2.m_visibility);

    // Check not existing group
    CPPUNIT_ASSERT_THROW(landmarks->getGroup(NO_GROUP), ::fwData::Exception);

    // check rename of a non existing group
    CPPUNIT_ASSERT_THROW(landmarks->renameGroup(NO_GROUP, GROUP3), ::fwData::Exception);

    // check rename to an existing group
    CPPUNIT_ASSERT_THROW(landmarks->renameGroup(GROUP1, GROUP2), ::fwData::Exception);

    // Remove group 1
    CPPUNIT_ASSERT_NO_THROW(landmarks->removeGroup(GROUP1));
    CPPUNIT_ASSERT_EQUAL(size_t(1), landmarks->getNumberOfGroups());
    const ::fwData::Landmarks::GroupNameContainer names2 = landmarks->getGroupNames();
    CPPUNIT_ASSERT_EQUAL(size_t(1), names2.size());

    CPPUNIT_ASSERT_EQUAL(GROUP2, names2[0]);

    // Rename group_2 to group_1
    CPPUNIT_ASSERT_NO_THROW(landmarks->renameGroup(GROUP2, GROUP3));

    CPPUNIT_ASSERT_EQUAL(size_t(1), landmarks->getNumberOfGroups());
    const ::fwData::Landmarks::GroupNameContainer names3 = landmarks->getGroupNames();
    CPPUNIT_ASSERT_EQUAL(size_t(1), names3.size());

    CPPUNIT_ASSERT_EQUAL(GROUP3, names3[0]);

    {
        const ::fwData::Landmarks::LandmarksGroup& group3 = landmarks->getGroup(GROUP3);
        CPPUNIT_ASSERT(COLOR2 == group3.m_color);
        CPPUNIT_ASSERT_EQUAL(SIZE2, group3.m_size);
        CPPUNIT_ASSERT_EQUAL(int(SHAPE2), int(group3.m_shape));
        CPPUNIT_ASSERT_EQUAL(VISIBILITY2, group3.m_visibility);
    }

    // change color
    CPPUNIT_ASSERT_NO_THROW(landmarks->setGroupColor(GROUP3, COLOR3));

    {
        const ::fwData::Landmarks::LandmarksGroup& group3 = landmarks->getGroup(GROUP3);
        CPPUNIT_ASSERT(COLOR3 == group3.m_color);
        CPPUNIT_ASSERT_EQUAL(SIZE2, group3.m_size);
        CPPUNIT_ASSERT_EQUAL(int(SHAPE2), int(group3.m_shape));
        CPPUNIT_ASSERT_EQUAL(VISIBILITY2, group3.m_visibility);
    }

    // change size
    CPPUNIT_ASSERT_NO_THROW(landmarks->setGroupSize(GROUP3, SIZE3));

    {
        const ::fwData::Landmarks::LandmarksGroup& group3 = landmarks->getGroup(GROUP3);
        CPPUNIT_ASSERT(COLOR3 == group3.m_color);
        CPPUNIT_ASSERT_EQUAL(SIZE3, group3.m_size);
        CPPUNIT_ASSERT_EQUAL(int(SHAPE2), int(group3.m_shape));
        CPPUNIT_ASSERT_EQUAL(VISIBILITY2, group3.m_visibility);
    }

    // change shape
    CPPUNIT_ASSERT_NO_THROW(landmarks->setGroupShape(GROUP3, SHAPE3));

    {
        const ::fwData::Landmarks::LandmarksGroup& group3 = landmarks->getGroup(GROUP3);
        CPPUNIT_ASSERT(COLOR3 == group3.m_color);
        CPPUNIT_ASSERT_EQUAL(SIZE3, group3.m_size);
        CPPUNIT_ASSERT_EQUAL(int(SHAPE3), int(group3.m_shape));
        CPPUNIT_ASSERT_EQUAL(VISIBILITY2, group3.m_visibility);
    }

    // change shape
    CPPUNIT_ASSERT_NO_THROW(landmarks->setGroupVisibility(GROUP3, VISIBILITY3));

    {
        const ::fwData::Landmarks::LandmarksGroup& group3 = landmarks->getGroup(GROUP3);
        CPPUNIT_ASSERT(COLOR3 == group3.m_color);
        CPPUNIT_ASSERT_EQUAL(SIZE3, group3.m_size);
        CPPUNIT_ASSERT_EQUAL(int(SHAPE3), int(group3.m_shape));
        CPPUNIT_ASSERT_EQUAL(VISIBILITY3, group3.m_visibility);
    }
}

//------------------------------------------------------------------------------

void LandmarksTest::pointsTest()
{
    const std::string GROUP1   = "group_1";
    const std::string GROUP2   = "group_2";
    const std::string GROUP3   = "group_3";
    const std::string NO_GROUP = "no_group";

    const ::fwData::Landmarks::PointType POINT1 = {{3.5, 5.8, 2.56}};
    const ::fwData::Landmarks::PointType POINT2 = {{8.25, 56.0, 45.4}};
    const ::fwData::Landmarks::PointType POINT3 = {{0.0, 0.0, 0.0}};
    const ::fwData::Landmarks::PointType POINT4 = {{0.5, 0.6, 0.7}};

    ::fwData::Landmarks::sptr landmarks = ::fwData::Landmarks::New();

    CPPUNIT_ASSERT_NO_THROW(landmarks->addGroup(GROUP1));
    CPPUNIT_ASSERT_NO_THROW(landmarks->addGroup(GROUP2));

    CPPUNIT_ASSERT_THROW(landmarks->addPoint(NO_GROUP, POINT1), ::fwData::Exception);

    CPPUNIT_ASSERT_NO_THROW(landmarks->addPoint(GROUP1, POINT1));
    CPPUNIT_ASSERT_EQUAL(size_t(1), landmarks->getNumberOfPoints(GROUP1));
    CPPUNIT_ASSERT_EQUAL(size_t(1), landmarks->getNumberOfPoints());

    const ::fwData::Landmarks::PointContainer& points = landmarks->getPoints(GROUP1);
    CPPUNIT_ASSERT_EQUAL(size_t(1), points.size());
    CPPUNIT_ASSERT(POINT1 == points[0]);
    CPPUNIT_ASSERT(POINT1 == landmarks->getPoint(GROUP1, 0));
    CPPUNIT_ASSERT_THROW(landmarks->getPoint(GROUP1, 23), std::out_of_range);

    CPPUNIT_ASSERT_NO_THROW(landmarks->addPoint(GROUP2, POINT2));
    CPPUNIT_ASSERT_EQUAL(size_t(1), landmarks->getNumberOfPoints(GROUP2));
    CPPUNIT_ASSERT_EQUAL(size_t(2), landmarks->getNumberOfPoints());

    CPPUNIT_ASSERT_NO_THROW(landmarks->addPoint(GROUP2, POINT3));
    CPPUNIT_ASSERT_EQUAL(size_t(2), landmarks->getNumberOfPoints(GROUP2));
    CPPUNIT_ASSERT_EQUAL(size_t(3), landmarks->getNumberOfPoints());

    const ::fwData::Landmarks::PointContainer& points2 = landmarks->getPoints(GROUP2);
    CPPUNIT_ASSERT_EQUAL(size_t(2), points2.size());
    CPPUNIT_ASSERT(POINT2 == points2[0]);
    CPPUNIT_ASSERT(POINT3 == points2[1]);
    CPPUNIT_ASSERT(POINT2 == landmarks->getPoint(GROUP2, 0));
    CPPUNIT_ASSERT(POINT3 == landmarks->getPoint(GROUP2, 1));

    // Check rename group
    CPPUNIT_ASSERT_NO_THROW(landmarks->renameGroup(GROUP2, GROUP3));
    const ::fwData::Landmarks::PointContainer& points3 = landmarks->getPoints(GROUP3);
    CPPUNIT_ASSERT_EQUAL(size_t(2), points3.size());
    CPPUNIT_ASSERT(POINT2 == points3[0]);
    CPPUNIT_ASSERT(POINT3 == points3[1]);
    CPPUNIT_ASSERT(POINT2 == landmarks->getPoint(GROUP3, 0));
    CPPUNIT_ASSERT(POINT3 == landmarks->getPoint(GROUP3, 1));

    // Check remove point
    CPPUNIT_ASSERT_THROW(landmarks->removePoint(NO_GROUP, 0), ::fwData::Exception);
    CPPUNIT_ASSERT_THROW(landmarks->removePoint(GROUP1, 2), std::out_of_range);
    CPPUNIT_ASSERT_NO_THROW(landmarks->removePoint(GROUP3, 1));

    CPPUNIT_ASSERT_EQUAL(size_t(1), landmarks->getNumberOfPoints(GROUP3));
    CPPUNIT_ASSERT_EQUAL(size_t(2), landmarks->getNumberOfPoints());
    CPPUNIT_ASSERT(POINT2 == landmarks->getPoint(GROUP3, 0));

    // Check remove group
    CPPUNIT_ASSERT_NO_THROW(landmarks->removeGroup(GROUP3));
    CPPUNIT_ASSERT_EQUAL(size_t(1), landmarks->getNumberOfPoints(GROUP1));
    CPPUNIT_ASSERT_EQUAL(size_t(1), landmarks->getNumberOfPoints());
    CPPUNIT_ASSERT_THROW(landmarks->getPoints(GROUP3), ::fwData::Exception);

    // Check insert point
    CPPUNIT_ASSERT_NO_THROW(landmarks->insertPoint(GROUP1, 0, POINT4));
    CPPUNIT_ASSERT_EQUAL(size_t(2), landmarks->getNumberOfPoints(GROUP1));
    CPPUNIT_ASSERT_EQUAL(size_t(2), landmarks->getNumberOfPoints());

    const ::fwData::Landmarks::PointContainer& points4 = landmarks->getPoints(GROUP1);
    CPPUNIT_ASSERT_EQUAL(size_t(2), points4.size());
    CPPUNIT_ASSERT(POINT4 == points4[0]);
    CPPUNIT_ASSERT(POINT1 == points4[1]);
    CPPUNIT_ASSERT(POINT4 == landmarks->getPoint(GROUP1, 0));
    CPPUNIT_ASSERT(POINT1 == landmarks->getPoint(GROUP1, 1));

    ::fwData::Landmarks::PointType& point = landmarks->getPoint(GROUP1, 0);
    point[2]                              = 18.6;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(18.6, landmarks->getPoint(GROUP1, 0)[2], 0.000001);

    // check clear points
    CPPUNIT_ASSERT_NO_THROW(landmarks->clearPoints(GROUP1));
    CPPUNIT_ASSERT_EQUAL(size_t(0), landmarks->getNumberOfPoints(GROUP1));
    CPPUNIT_ASSERT_EQUAL(size_t(0), landmarks->getNumberOfPoints());
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwData
