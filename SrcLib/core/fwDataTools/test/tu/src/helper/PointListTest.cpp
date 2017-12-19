/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "helper/PointListTest.hpp"

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/PointList.hpp>

#include <fwCore/Exception.hpp>

#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>

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
        // And the second one with inscreasing x values but shifted in y
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

void PointListTest::associatePointLists()
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
        ::fwDataTools::helper::PointList::associate(pl1, ::fwData::TransformationMatrix3D::New(),
                                                    pl2, ::fwData::TransformationMatrix3D::New());
    }

    // Simple test with parallel point lists
    {
        pl1 = ::fwData::PointList::New();
        pl2 = ::fwData::PointList::New();

        // Build 2 point lists:
        // The first one with increasing x values
        // And the second one with inscreasing x values but shifted in y
        for(size_t i = 0; i <= nbPoints; i++)
        {
            p = ::fwData::Point::New(static_cast<float>(i), 0.0f, 0.0f);
            pl1->pushBack(p);

            p = ::fwData::Point::New(static_cast<float>(nbPoints - i), 0.0f, 0.0f);
            pl2->pushBack(p);
        }

        // Associate the point lists
        ::fwDataTools::helper::PointList::associate(pl1, ::fwData::TransformationMatrix3D::New(),
                                                    pl2, ::fwData::TransformationMatrix3D::New());

        const ::fwData::PointList::PointListContainer points1 = pl1->getCRefPoints();
        ::fwData::PointList::PointListContainer points2 = pl2->getRefPoints();

        const size_t size = points1.size();

        for(size_t i = 0; i < size; ++i)
        {
            ::fwData::Point::PointCoordArrayType tmp1 = points1[i]->getCRefCoord();
            ::fwData::Point::PointCoordArrayType tmp2 = points2[i]->getCRefCoord();

            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[0], tmp2[0], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[1], tmp2[1], 1e-8);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(tmp1[2], tmp2[2], 1e-8);
        }

    }

}

} //namespace ut
} //namespace fwDataTools
