/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "helper/PointListTest.hpp"

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/PointList.hpp>

#include <fwCore/Exception.hpp>

#include <fwData/Point.hpp>

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include <random>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDataTools::ut::PointListTest );

namespace fwDataTools
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

    ::fwData::PointList::sptr pl1;
    ::fwData::PointList::sptr pl2;

    ::fwData::Point::sptr p;

    // Simple test with empty point lists
    {
        pl1 = ::fwData::PointList::New();
        pl2 = ::fwData::PointList::New();

        // Compare the point lists
        ::fwData::Array::sptr outputArray = ::fwDataTools::helper::PointList::computeDistance(pl1, pl2);

        CPPUNIT_ASSERT(outputArray->getNumberOfElements() == 0);
    }

    // Simple test with parallel point lists
    {
        pl1 = ::fwData::PointList::New();
        pl2 = ::fwData::PointList::New();

        // Build 2 pointlists:
        // The first one with increasing x values
        // And the second one with inscreasing x values but shifted in y
        for(size_t i = 0; i < nbPoints; i++)
        {
            p = ::fwData::Point::New(static_cast<float>(i), 0.0f, 0.0f);
            pl1->pushBack(p);

            p = ::fwData::Point::New(static_cast<float>(i), 1.0f, 0.0f);
            pl2->pushBack(p);
        }

        // Compare the point lists
        ::fwData::Array::sptr outputArray = ::fwDataTools::helper::PointList::computeDistance(pl1, pl2);

        ::fwDataTools::helper::Array arrayHelper(outputArray);
        double* distanceArray = arrayHelper.begin<double>();

        for(size_t i = 0; i < nbPoints; i++)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(distanceArray[i], 1.0, 1e-8);
        }
    }

    // Simple test with diverging point lists
    {
        pl1 = ::fwData::PointList::New();
        pl2 = ::fwData::PointList::New();

        // Build 2 point lists:
        // The first one with increasing x values
        // And the second one with increasing x values but shifted in y
        for(size_t i = 0; i < nbPoints; i++)
        {
            p = ::fwData::Point::New(static_cast<float>(i), 0.0f, 0.0f);
            pl1->pushBack(p);

            p = ::fwData::Point::New(static_cast<float>(i), static_cast<float>(i), 0.0f);
            pl2->pushBack(p);
        }

        // Compare the point lists
        ::fwData::Array::sptr outputArray = ::fwDataTools::helper::PointList::computeDistance(pl1, pl2);

        ::fwDataTools::helper::Array arrayHelper(outputArray);
        double* distanceArray = arrayHelper.begin<double>();

        for(size_t i = 0; i < nbPoints; i++)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(distanceArray[i], static_cast<double>(i), 1e-8);
        }
    }
}

//------------------------------------------------------------------------------

void PointListTest::transform()
{
    // Simple test with identity
    {
        // Test sample
        ::fwData::PointList::sptr pl1 = ::fwData::PointList::New();
        pl1->pushBack(::fwData::Point::New(0.0f, 0.0f, 0.0f));
        pl1->pushBack(::fwData::Point::New(1.0f, 0.0f, 0.0f));
        pl1->pushBack(::fwData::Point::New(1.0f, 1.0f, 0.0f));
        pl1->pushBack(::fwData::Point::New(0.0f, 1.0f, 0.0f));

        pl1->pushBack(::fwData::Point::New(0.0f, 0.0f, 1.0f));
        pl1->pushBack(::fwData::Point::New(1.0f, 0.0f, 1.0f));
        pl1->pushBack(::fwData::Point::New(1.0f, 1.0f, 1.0f));
        pl1->pushBack(::fwData::Point::New(0.0f, 1.0f, 1.0f));

        // Reference list
        const ::fwData::PointList::PointListContainer points1 = pl1->getPoints();
        const size_t size                                     = points1.size();

        ::fwData::PointList::sptr pl2 = ::fwData::PointList::New();
        for(size_t i = 0; i < size; i++)
        {
            const ::fwData::Point::PointCoordArrayType tmp = points1[i]->getCRefCoord();
            pl2->pushBack(::fwData::Point::New(tmp[0], tmp[1], tmp[2]));
        }

        const ::fwData::TransformationMatrix3D::sptr tf1 = ::fwData::TransformationMatrix3D::New();
        ::fwDataTools::helper::PointList::transform(pl1, tf1);

        const ::fwData::PointList::PointListContainer points2 = pl2->getPoints();
        for(size_t i = 0; i < size; i++)
        {
            const ::fwData::Point::PointCoordArrayType tmp1 = points1[i]->getCRefCoord();
            const ::fwData::Point::PointCoordArrayType tmp2 = points2[i]->getCRefCoord();

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
        ::fwData::PointList::sptr pl1 = ::fwData::PointList::New();
        pl1->pushBack(::fwData::Point::New(0.0f, 0.0f, 0.0f));
        pl1->pushBack(::fwData::Point::New(1.0f, 0.0f, 0.0f));
        pl1->pushBack(::fwData::Point::New(1.0f, 1.0f, 0.0f));
        pl1->pushBack(::fwData::Point::New(0.0f, 1.0f, 0.0f));

        pl1->pushBack(::fwData::Point::New(0.0f, 0.0f, 1.0f));
        pl1->pushBack(::fwData::Point::New(1.0f, 0.0f, 1.0f));
        pl1->pushBack(::fwData::Point::New(1.0f, 1.0f, 1.0f));
        pl1->pushBack(::fwData::Point::New(0.0f, 1.0f, 1.0f));

        // Reference list
        const ::fwData::PointList::PointListContainer points1 = pl1->getPoints();
        const size_t size                                     = points1.size();

        ::fwData::PointList::sptr pl2 = ::fwData::PointList::New();
        for(size_t i = 0; i < size; i++)
        {
            const ::fwData::Point::PointCoordArrayType tmp = points1[i]->getCRefCoord();
            pl2->pushBack(::fwData::Point::New(tmp[0] + translation[0], tmp[1] + translation[1],
                                               tmp[2] + translation[2]));
        }

        const ::fwData::TransformationMatrix3D::sptr tf1 = ::fwData::TransformationMatrix3D::New();
        tf1->setCoefficient(0, 3, translation[0]);
        tf1->setCoefficient(1, 3, translation[1]);
        tf1->setCoefficient(2, 3, translation[2]);
        ::fwDataTools::helper::PointList::transform(pl1, tf1);

        const ::fwData::PointList::PointListContainer points2 = pl2->getPoints();
        for(size_t i = 0; i < size; i++)
        {
            const ::fwData::Point::PointCoordArrayType tmp1 = points1[i]->getCRefCoord();
            const ::fwData::Point::PointCoordArrayType tmp2 = points2[i]->getCRefCoord();

            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[0], tmp2[0], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[1], tmp2[1], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[2], tmp2[2], 1e-8);
        }
    }

    // Simple test with rotation
    {
        ::fwData::PointList::sptr pl1 = ::fwData::PointList::New();
        pl1->pushBack(::fwData::Point::New(0.0f, 0.0f, 0.0f));
        pl1->pushBack(::fwData::Point::New(1.0f, 0.0f, 0.0f));
        pl1->pushBack(::fwData::Point::New(1.0f, 1.0f, 0.0f));
        pl1->pushBack(::fwData::Point::New(0.0f, 1.0f, 0.0f));

        pl1->pushBack(::fwData::Point::New(0.0f, 0.0f, 1.0f));
        pl1->pushBack(::fwData::Point::New(1.0f, 0.0f, 1.0f));
        pl1->pushBack(::fwData::Point::New(1.0f, 1.0f, 1.0f));
        pl1->pushBack(::fwData::Point::New(0.0f, 1.0f, 1.0f));

        ::fwData::PointList::sptr pl2 = ::fwData::PointList::New();
        pl2->pushBack(::fwData::Point::New(0.0f, 0.0f, 0.0f));
        pl2->pushBack(::fwData::Point::New(-1.0f, 0.0f, 0.0f));
        pl2->pushBack(::fwData::Point::New(-1.0f, -1.0f, 0.0f));
        pl2->pushBack(::fwData::Point::New(0.0f, -1.0f, 0.0f));

        pl2->pushBack(::fwData::Point::New(0.0f, 0.0f, 1.0f));
        pl2->pushBack(::fwData::Point::New(-1.0f, 0.0f, 1.0f));
        pl2->pushBack(::fwData::Point::New(-1.0f, -1.0f, 1.0f));
        pl2->pushBack(::fwData::Point::New(0.0f, -1.0f, 1.0f));

        // Perform a 180° rotation around Z
        const ::fwData::TransformationMatrix3D::sptr tf1 = ::fwData::TransformationMatrix3D::New();
        tf1->setCoefficient(0, 0, -1.0f);
        tf1->setCoefficient(0, 1, 0.0f);
        tf1->setCoefficient(1, 0, 0.0f);
        tf1->setCoefficient(1, 1, -1.0f);

        ::fwDataTools::helper::PointList::transform(pl1, tf1);

        const ::fwData::PointList::PointListContainer points1 = pl1->getPoints();
        size_t size                                           = points1.size();
        const ::fwData::PointList::PointListContainer points2 = pl2->getPoints();

        for(size_t i = 0; i < size; i++)
        {
            const ::fwData::Point::PointCoordArrayType tmp1 = points1[i]->getCRefCoord();
            const ::fwData::Point::PointCoordArrayType tmp2 = points2[i]->getCRefCoord();

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

    ::fwData::PointList::sptr pl1;
    ::fwData::PointList::sptr pl2;

    ::fwData::Point::sptr p;

    // Simple test with empty point lists
    {
        pl1 = ::fwData::PointList::New();
        pl2 = ::fwData::PointList::New();

        // Associate empty point lists
        ::fwDataTools::helper::PointList::associate(pl1, pl2);

        CPPUNIT_ASSERT(checkAssociation(pl1, pl2));
    }

    // Test with simple matrices
    // Create two lists with the same sets of points and shift them with transformation matrices
    // Associating them should make the x components match
    {
        pl1 = ::fwData::PointList::New();
        pl2 = ::fwData::PointList::New();

        // Build 2 point lists with the same points, the point are in the inverse order in the second list
        for(size_t i = 0; i <= nbPoints; i++)
        {
            p = ::fwData::Point::New(static_cast<float>(i), 0.0f, 0.0f);
            pl1->pushBack(p);

            p = ::fwData::Point::New(static_cast<float>(nbPoints - i), 0.0f, 0.0f);
            pl2->pushBack(p);
        }

        // Transform the point lists, shift the points in y
        ::fwData::TransformationMatrix3D::sptr tf1 = ::fwData::TransformationMatrix3D::New();
        tf1->setCoefficient(1, 3, 42.0);
        ::fwData::TransformationMatrix3D::sptr tf2 = ::fwData::TransformationMatrix3D::New();
        tf2->setCoefficient(1, 3, -42.0);

        ::fwDataTools::helper::PointList::transform(pl1, tf1);
        ::fwDataTools::helper::PointList::transform(pl2, tf2);

        // Associate the point lists
        ::fwDataTools::helper::PointList::associate(pl1, pl2);

        CPPUNIT_ASSERT(checkAssociation(pl1, pl2));
    }

    // Test with random point lists
    // Fill two point lists with random points
    // Check that the matched points are really the closest
    {
        pl1 = ::fwData::PointList::New();
        pl2 = ::fwData::PointList::New();

        std::default_random_engine generator;
        std::uniform_real_distribution<double> distribution(-10, 10);

        // Build 2 point lists with the same points, the point are in the inverse order in the second list
        for(size_t i = 0; i <= nbPoints; i++)
        {
            p = ::fwData::Point::New(distribution(generator), distribution(generator), distribution(generator));
            pl1->pushBack(p);

            p = ::fwData::Point::New(distribution(generator), distribution(generator), distribution(generator));
            pl2->pushBack(p);
        }

        // Transform the point lists, shift the points in y
        ::fwData::TransformationMatrix3D::sptr tf1 = ::fwData::TransformationMatrix3D::New();
        tf1->setCoefficient(1, 3, 42.0);
        ::fwData::TransformationMatrix3D::sptr tf2 = ::fwData::TransformationMatrix3D::New();
        tf2->setCoefficient(1, 3, -42.0);

        ::fwDataTools::helper::PointList::transform(pl1, tf1);
        ::fwDataTools::helper::PointList::transform(pl2, tf2);

        // Associate the point lists
        ::fwDataTools::helper::PointList::associate(pl1, pl2);

        CPPUNIT_ASSERT(checkAssociation(pl1, pl2));
    }
}

//------------------------------------------------------------------------------

bool PointListTest::checkAssociation(const ::fwData::PointList::csptr& pl1, const ::fwData::PointList::csptr& pl2)
{
    bool associationOK = true;

    const ::fwData::PointList::PointListContainer points1 = pl1->getCRefPoints();
    const ::fwData::PointList::PointListContainer points2 = pl2->getCRefPoints();

    const size_t size1 = points1.size();
    const size_t size2 = points2.size();

    size_t closestPointRank;
    double closestPointDistance;

    // Take each point of list1
    for(size_t i = 0; i < size1; ++i)
    {
        closestPointRank     = -1;
        closestPointDistance = std::numeric_limits<double>::max();

        const ::fwData::Point::PointCoordArrayType tmp1 = points1[i]->getCRefCoord();
        const ::glm::dvec3 p1                           = ::glm::dvec3(tmp1[0], tmp1[1], tmp1[2]);

        // ... And compare it to each point of list 2 to find the closest point
        // We start at the current index i, because the previous points are already associated
        // Normally it should be the point with the same rank
        for(size_t j = i; j < size2; j++)
        {
            ::fwData::Point::PointCoordArrayType tmp2 = points2[j]->getCRefCoord();
            const ::glm::dvec3 p2 = ::glm::dvec3(tmp2[0], tmp2[1], tmp2[2]);

            double d = ::glm::distance(p1, p2);

            if(d < closestPointDistance)
            {
                closestPointRank     = j;
                closestPointDistance = d;
            }
        }

        // If the closest point is not the point of same rank
        // Then there is an error
        if(i != closestPointRank)
        {
            associationOK = false;
            break;
        }
    }

    return associationOK;
}

} //namespace ut
} //namespace fwDataTools
