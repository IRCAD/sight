/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include "PointListTest.hpp"

#include <core/Exception.hpp>

#include <data/Point.hpp>

#include <geometry/data/PointList.hpp>

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include <random>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::geometry::data::ut::PointListTest);

namespace sight::geometry::data
{

namespace ut
{

//------------------------------------------------------------------------------

void PointListTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void PointListTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void PointListTest::computeDistance()
{
    const size_t nbPoints = 42;

    sight::data::PointList::sptr pl1;
    sight::data::PointList::sptr pl2;

    sight::data::Point::sptr p;

    // Simple test with empty point lists
    {
        pl1 = sight::data::PointList::New();
        pl2 = sight::data::PointList::New();

        // Compare the point lists
        auto outputArray = geometry::data::PointList::computeDistance(pl1, pl2);

        CPPUNIT_ASSERT(outputArray->numElements() == 0);
    }

    // Simple test with parallel point lists
    {
        pl1 = sight::data::PointList::New();
        pl2 = sight::data::PointList::New();

        // Build 2 pointlists:
        // The first one with increasing x values
        // And the second one with inscreasing x values but shifted in y
        for(size_t i = 0 ; i < nbPoints ; i++)
        {
            p = sight::data::Point::New(static_cast<float>(i), 0.0f, 0.0f);
            pl1->pushBack(p);

            p = sight::data::Point::New(static_cast<float>(i), 1.0f, 0.0f);
            pl2->pushBack(p);
        }

        // Compare the point lists
        auto outputArray = geometry::data::PointList::computeDistance(pl1, pl2);

        const auto dumpLock   = outputArray->lock();
        auto distanceArrayItr = outputArray->begin<double>();

        for(size_t i = 0 ; i < nbPoints ; i++, ++distanceArrayItr)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(*distanceArrayItr, 1.0, 1e-8);
        }
    }

    // Simple test with diverging point lists
    {
        pl1 = sight::data::PointList::New();
        pl2 = sight::data::PointList::New();

        // Build 2 point lists:
        // The first one with increasing x values
        // And the second one with increasing x values but shifted in y
        for(size_t i = 0 ; i < nbPoints ; i++)
        {
            p = sight::data::Point::New(static_cast<float>(i), 0.0f, 0.0f);
            pl1->pushBack(p);

            p = sight::data::Point::New(static_cast<float>(i), static_cast<float>(i), 0.0f);
            pl2->pushBack(p);
        }

        // Compare the point lists
        auto outputArray    = geometry::data::PointList::computeDistance(pl1, pl2);
        const auto dumpLock = outputArray->lock();

        auto distanceArrayItr = outputArray->begin<double>();

        for(size_t i = 0 ; i < nbPoints ; i++, ++distanceArrayItr)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(*distanceArrayItr, static_cast<double>(i), 1e-8);
        }
    }
}

//------------------------------------------------------------------------------

void PointListTest::transform()
{
    // Simple test with identity
    {
        // Test sample
        sight::data::PointList::sptr pl1 = sight::data::PointList::New();
        pl1->pushBack(sight::data::Point::New(0.0f, 0.0f, 0.0f));
        pl1->pushBack(sight::data::Point::New(1.0f, 0.0f, 0.0f));
        pl1->pushBack(sight::data::Point::New(1.0f, 1.0f, 0.0f));
        pl1->pushBack(sight::data::Point::New(0.0f, 1.0f, 0.0f));

        pl1->pushBack(sight::data::Point::New(0.0f, 0.0f, 1.0f));
        pl1->pushBack(sight::data::Point::New(1.0f, 0.0f, 1.0f));
        pl1->pushBack(sight::data::Point::New(1.0f, 1.0f, 1.0f));
        pl1->pushBack(sight::data::Point::New(0.0f, 1.0f, 1.0f));

        // Reference list
        const sight::data::PointList::PointListContainer points1 = pl1->getPoints();
        const size_t size                                        = points1.size();

        sight::data::PointList::sptr pl2 = sight::data::PointList::New();
        for(size_t i = 0 ; i < size ; i++)
        {
            const sight::data::Point::PointCoordArrayType tmp = points1[i]->getCoord();
            pl2->pushBack(sight::data::Point::New(tmp[0], tmp[1], tmp[2]));
        }

        const auto tf1 = sight::data::Matrix4::New();
        geometry::data::PointList::transform(pl1, tf1);

        const sight::data::PointList::PointListContainer points2 = pl2->getPoints();
        for(size_t i = 0 ; i < size ; i++)
        {
            const sight::data::Point::PointCoordArrayType tmp1 = points1[i]->getCoord();
            const sight::data::Point::PointCoordArrayType tmp2 = points2[i]->getCoord();

            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[0], tmp2[0], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[1], tmp2[1], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[2], tmp2[2], 1e-8);
        }
    }

    // Simple test with translation
    {
        std::vector<float> translation(3, 0.0f);
        translation[0] = 8.0;
        translation[1] = 16.0;
        translation[2] = 32.0;

        // Test sample
        sight::data::PointList::sptr pl1 = sight::data::PointList::New();
        pl1->pushBack(sight::data::Point::New(0.0f, 0.0f, 0.0f));
        pl1->pushBack(sight::data::Point::New(1.0f, 0.0f, 0.0f));
        pl1->pushBack(sight::data::Point::New(1.0f, 1.0f, 0.0f));
        pl1->pushBack(sight::data::Point::New(0.0f, 1.0f, 0.0f));

        pl1->pushBack(sight::data::Point::New(0.0f, 0.0f, 1.0f));
        pl1->pushBack(sight::data::Point::New(1.0f, 0.0f, 1.0f));
        pl1->pushBack(sight::data::Point::New(1.0f, 1.0f, 1.0f));
        pl1->pushBack(sight::data::Point::New(0.0f, 1.0f, 1.0f));

        // Reference list
        const sight::data::PointList::PointListContainer points1 = pl1->getPoints();
        const size_t size                                        = points1.size();

        sight::data::PointList::sptr pl2 = sight::data::PointList::New();
        for(size_t i = 0 ; i < size ; i++)
        {
            const sight::data::Point::PointCoordArrayType tmp = points1[i]->getCoord();
            pl2->pushBack(
                sight::data::Point::New(
                    tmp[0] + translation[0],
                    tmp[1] + translation[1],
                    tmp[2] + translation[2]
                )
            );
        }

        const auto tf1 = sight::data::Matrix4::New();
        tf1->setCoefficient(0, 3, translation[0]);
        tf1->setCoefficient(1, 3, translation[1]);
        tf1->setCoefficient(2, 3, translation[2]);
        geometry::data::PointList::transform(pl1, tf1);

        const sight::data::PointList::PointListContainer points2 = pl2->getPoints();
        for(size_t i = 0 ; i < size ; i++)
        {
            const sight::data::Point::PointCoordArrayType tmp1 = points1[i]->getCoord();
            const sight::data::Point::PointCoordArrayType tmp2 = points2[i]->getCoord();

            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[0], tmp2[0], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[1], tmp2[1], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[2], tmp2[2], 1e-8);
        }
    }

    // Simple test with rotation
    {
        sight::data::PointList::sptr pl1 = sight::data::PointList::New();
        pl1->pushBack(sight::data::Point::New(0.0f, 0.0f, 0.0f));
        pl1->pushBack(sight::data::Point::New(1.0f, 0.0f, 0.0f));
        pl1->pushBack(sight::data::Point::New(1.0f, 1.0f, 0.0f));
        pl1->pushBack(sight::data::Point::New(0.0f, 1.0f, 0.0f));

        pl1->pushBack(sight::data::Point::New(0.0f, 0.0f, 1.0f));
        pl1->pushBack(sight::data::Point::New(1.0f, 0.0f, 1.0f));
        pl1->pushBack(sight::data::Point::New(1.0f, 1.0f, 1.0f));
        pl1->pushBack(sight::data::Point::New(0.0f, 1.0f, 1.0f));

        sight::data::PointList::sptr pl2 = sight::data::PointList::New();
        pl2->pushBack(sight::data::Point::New(0.0f, 0.0f, 0.0f));
        pl2->pushBack(sight::data::Point::New(-1.0f, 0.0f, 0.0f));
        pl2->pushBack(sight::data::Point::New(-1.0f, -1.0f, 0.0f));
        pl2->pushBack(sight::data::Point::New(0.0f, -1.0f, 0.0f));

        pl2->pushBack(sight::data::Point::New(0.0f, 0.0f, 1.0f));
        pl2->pushBack(sight::data::Point::New(-1.0f, 0.0f, 1.0f));
        pl2->pushBack(sight::data::Point::New(-1.0f, -1.0f, 1.0f));
        pl2->pushBack(sight::data::Point::New(0.0f, -1.0f, 1.0f));

        // Perform a 180 degrees rotation around Z
        const auto tf1 = sight::data::Matrix4::New();
        tf1->setCoefficient(0, 0, -1.0f);
        tf1->setCoefficient(0, 1, 0.0f);
        tf1->setCoefficient(1, 0, 0.0f);
        tf1->setCoefficient(1, 1, -1.0f);

        geometry::data::PointList::transform(pl1, tf1);

        const sight::data::PointList::PointListContainer points1 = pl1->getPoints();
        size_t size                                              = points1.size();
        const sight::data::PointList::PointListContainer points2 = pl2->getPoints();

        for(size_t i = 0 ; i < size ; i++)
        {
            const sight::data::Point::PointCoordArrayType tmp1 = points1[i]->getCoord();
            const sight::data::Point::PointCoordArrayType tmp2 = points2[i]->getCoord();

            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[0], tmp2[0], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[1], tmp2[1], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[2], tmp2[2], 1e-8);
        }
    }
}

//------------------------------------------------------------------------------

void PointListTest::associate()
{
    const size_t nbPoints = 42;

    sight::data::PointList::sptr pl1;
    sight::data::PointList::sptr pl2;

    sight::data::Point::sptr p;

    // Simple test with empty point lists
    {
        pl1 = sight::data::PointList::New();
        pl2 = sight::data::PointList::New();

        // Associate empty point lists
        geometry::data::PointList::associate(pl1, pl2);

        // No results expected
    }

    // Test with simple matrices
    // Create two lists with the same sets of points and shift them with transformation matrices
    // Associating them should make the x components match
    {
        pl1 = sight::data::PointList::New();
        pl2 = sight::data::PointList::New();

        // Build 2 point lists with the same points, the point are in the inverse order in the second list
        for(size_t i = 0 ; i <= nbPoints ; i++)
        {
            p = sight::data::Point::New(static_cast<float>(i), 0.0f, 0.0f);
            pl1->pushBack(p);

            p = sight::data::Point::New(static_cast<float>(nbPoints - i), 0.0f, 0.0f);
            pl2->pushBack(p);
        }

        // Transform the point lists, shift the points in y
        auto tf1 = sight::data::Matrix4::New();
        tf1->setCoefficient(1, 3, 42.0);
        auto tf2 = sight::data::Matrix4::New();
        tf2->setCoefficient(1, 3, -42.0);

        geometry::data::PointList::transform(pl1, tf1);
        geometry::data::PointList::transform(pl2, tf2);

        // Associate the point lists
        geometry::data::PointList::associate(pl1, pl2);

        // Check that the two list are equal (re-ordered)
        // Only the last component should differ, as there is a (0,0,42) translation
        // on the first one and a (0,0,-42) translation on the second one
        const sight::data::PointList::PointListContainer points1 = pl1->getPoints();
        const sight::data::PointList::PointListContainer points2 = pl2->getPoints();

        const size_t size = points1.size();

        for(size_t i = 0 ; i < size ; i++)
        {
            const sight::data::Point::PointCoordArrayType tmp1 = points1[i]->getCoord();
            const sight::data::Point::PointCoordArrayType tmp2 = points2[i]->getCoord();

            // Check that the last component is equal to i
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[0], tmp2[0], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[0], static_cast<float>(i), 1e-8);

            // Check that the second component is equal to 42
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[1], -tmp2[1], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[1], static_cast<float>(42), 1e-8);

            // Check that the last component is equal to 0
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[2], tmp2[2], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[2], 0.0, 1e-8);
        }
    }

    // Test with random point lists
    // Fill two point lists with random points
    // Check that the matched points are really the closest
    {
        // Create an initial octahedron with points
        sight::data::PointList::sptr pl1 = sight::data::PointList::New();
        pl1->pushBack(sight::data::Point::New(1.0f, 0.0f, 0.0f));
        pl1->pushBack(sight::data::Point::New(2.0f, 0.0f, 1.0f));
        pl1->pushBack(sight::data::Point::New(2.0f, 0.8f, 0.0f));
        pl1->pushBack(sight::data::Point::New(2.0f, 0.0f, -1.0f));
        pl1->pushBack(sight::data::Point::New(2.0f, -0.8f, 0.0f));
        pl1->pushBack(sight::data::Point::New(3.0f, 0.0f, 0.0f));

        // Reference final octahedron
        sight::data::PointList::sptr pl2 = sight::data::PointList::New();
        pl2->pushBack(sight::data::Point::New(0.0f, 0.0f, 5.2f));
        pl2->pushBack(sight::data::Point::New(1.0f, 0.0f, 6.2f));
        pl2->pushBack(sight::data::Point::New(0.0f, 0.8f, 6.2f));
        pl2->pushBack(sight::data::Point::New(-1.0f, 0.0f, 6.2f));
        pl2->pushBack(sight::data::Point::New(0.0f, -0.8f, 6.2f));
        pl2->pushBack(sight::data::Point::New(0.0f, 0.0f, 7.2f));

        // Transform the point list
        auto tf1 = sight::data::Matrix4::New();
        // Shift the points in Z
        tf1->setCoefficient(2, 3, 4.2);

        // Add a 90 degrees rotation around Y
        tf1->setCoefficient(0, 0, 0.0f);
        tf1->setCoefficient(0, 2, 1.0f);
        tf1->setCoefficient(2, 0, 1.0f);
        tf1->setCoefficient(2, 2, 0.0f);

        geometry::data::PointList::transform(pl1, tf1);

        // Associate the point lists
        geometry::data::PointList::associate(pl1, pl2);

        // Check that the two lists are equal
        // The second one corresponds to a Z translation + a 90 degrees Y translation of the first one
        const sight::data::PointList::PointListContainer points1 = pl1->getPoints();
        const sight::data::PointList::PointListContainer points2 = pl2->getPoints();

        const size_t size      = points1.size();
        const int nbComponents = 3;

        for(size_t i = 0 ; i < size ; i++)
        {
            const sight::data::Point::PointCoordArrayType tmp1 = points1[i]->getCoord();
            const sight::data::Point::PointCoordArrayType tmp2 = points2[i]->getCoord();

            // Compare the components
            for(int j = 0 ; j < nbComponents ; j++)
            {
                CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[j], tmp2[j], 1e-5);
            }
        }
    }
}

//------------------------------------------------------------------------------

void PointListTest::removeClosestPointNominal()
{
    const size_t nbPoints           = 42;
    sight::data::PointList::sptr pl = sight::data::PointList::New();

    // Remove points in an empty list
    for(size_t i = 0 ; i < nbPoints ; ++i)
    {
        const auto p =
            sight::data::Point::New(static_cast<float>(i), static_cast<float>(i), static_cast<float>(i));
        CPPUNIT_ASSERT(
            geometry::data::PointList::removeClosestPoint(
                pl,
                p,
                std::numeric_limits<float>::min()
            )
            == nullptr
        );
    }

    // Build a list
    for(size_t i = 0 ; i < nbPoints ; ++i)
    {
        const auto p =
            sight::data::Point::New(static_cast<float>(i), static_cast<float>(i), static_cast<float>(i));
        pl->pushBack(p);
    }

    // Remove points with an unmatched delta
    for(size_t i = 0 ; i < nbPoints ; ++i)
    {
        const auto p = sight::data::Point::New(
            static_cast<float>(nbPoints + 1),
            static_cast<float>(nbPoints + 1),
            static_cast<float>(nbPoints + 1)
        );
        CPPUNIT_ASSERT(
            geometry::data::PointList::removeClosestPoint(
                pl,
                p,
                std::numeric_limits<float>::min()
            )
            == nullptr
        );
    }

    // Remove points
    for(size_t i = 0 ; i < nbPoints ; ++i)
    {
        const auto p =
            sight::data::Point::New(static_cast<float>(i), static_cast<float>(i), static_cast<float>(i));
        const auto pRes =
            geometry::data::PointList::removeClosestPoint(pl, p, std::numeric_limits<float>::max());
        CPPUNIT_ASSERT(pRes != nullptr);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(pRes->getCoord()[0], p->getCoord()[0], std::numeric_limits<float>::min());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(pRes->getCoord()[1], p->getCoord()[1], std::numeric_limits<float>::min());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(pRes->getCoord()[2], p->getCoord()[2], std::numeric_limits<float>::min());
    }
}

//------------------------------------------------------------------------------

void PointListTest::removeClosestPointExtreme()
{
    const size_t nbPoints           = 42;
    sight::data::PointList::sptr pl = sight::data::PointList::New();

    // Build a list
    for(size_t i = 0 ; i < nbPoints ; ++i)
    {
        const auto p =
            sight::data::Point::New(static_cast<float>(i), static_cast<float>(i), static_cast<float>(i));
        pl->pushBack(p);
    }

    // Remove points with negative delta
    for(size_t i = 0 ; i < nbPoints ; ++i)
    {
        const auto p =
            sight::data::Point::New(static_cast<float>(i), static_cast<float>(i), static_cast<float>(i));
        CPPUNIT_ASSERT(
            geometry::data::PointList::removeClosestPoint(
                pl,
                p,
                -std::numeric_limits<float>::max()
            )
            == nullptr
        );
    }

    // Remove points with biggest delta
    for(size_t i = 0 ; i < nbPoints ; ++i)
    {
        const auto p =
            sight::data::Point::New(static_cast<float>(i), static_cast<float>(i), static_cast<float>(i));
        const auto pRes =
            geometry::data::PointList::removeClosestPoint(pl, p, std::numeric_limits<float>::max());
        CPPUNIT_ASSERT(pRes != nullptr);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(pRes->getCoord()[0], p->getCoord()[0], i + std::numeric_limits<float>::max());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(pRes->getCoord()[1], p->getCoord()[1], i + std::numeric_limits<float>::max());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(pRes->getCoord()[2], p->getCoord()[2], i + std::numeric_limits<float>::max());
    }
}

} //namespace ut

} //namespace sight::geometry::data
