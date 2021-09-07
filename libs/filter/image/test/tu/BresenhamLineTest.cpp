/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "BresenhamLineTest.hpp"

#include <data/helper/MedicalImage.hpp>

#include <filter/image/BresenhamLine.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(::sight::filter::image::ut::BresenhamLineTest);

namespace sight::filter::image
{

namespace ut
{

//------------------------------------------------------------------------------

void BresenhamLineTest::setUp()
{
}

//------------------------------------------------------------------------------

void BresenhamLineTest::tearDown()
{
}

//------------------------------------------------------------------------------

void BresenhamLineTest::straightLineTest()
{
    {
        const filter::image::BresenhamLine::CoordinatesType START_POINT = {{0, 0, 0}};
        const filter::image::BresenhamLine::CoordinatesType END_POINT   = {{0, 0, 1024}};

        const data::helper::MedicalImage::Orientation SAGITAL =
            data::helper::MedicalImage::X_AXIS;

        const filter::image::BresenhamLine::PathType result =
            filter::image::BresenhamLine::draw(SAGITAL, START_POINT, END_POINT);

        CPPUNIT_ASSERT(!result.empty());
        CPPUNIT_ASSERT_EQUAL(size_t(END_POINT[2] - START_POINT[2] + 1), result.size());

        auto iter = result.begin();
        for(size_t i = START_POINT[2] ; i <= END_POINT[2] ; ++i)
        {
            const auto& pathElt = *iter;

            const filter::image::BresenhamLine::CoordinatesType expectedElt = {{0, 0, i}};

            CPPUNIT_ASSERT(pathElt == expectedElt);
            ++iter;
        }
    }

    {
        const filter::image::BresenhamLine::CoordinatesType START_POINT = {{25, 2057, 3}};
        const filter::image::BresenhamLine::CoordinatesType END_POINT   = {{25, 1313, 3}};

        const data::helper::MedicalImage::Orientation AXIAL =
            data::helper::MedicalImage::Z_AXIS;

        const filter::image::BresenhamLine::PathType result =
            filter::image::BresenhamLine::draw(AXIAL, START_POINT, END_POINT);

        CPPUNIT_ASSERT(!result.empty());
        CPPUNIT_ASSERT_EQUAL(size_t(START_POINT[1] - END_POINT[1] + 1), result.size());

        auto iter = result.begin();
        for(size_t i = START_POINT[1] ; i >= END_POINT[1] ; --i)
        {
            const auto& pathElt = *iter;

            const filter::image::BresenhamLine::CoordinatesType expectedElt = {{25, i, 3}};

            CPPUNIT_ASSERT(pathElt == expectedElt);
            ++iter;
        }
    }

    {
        const filter::image::BresenhamLine::CoordinatesType START_POINT = {{90, 67, 68}};
        const filter::image::BresenhamLine::CoordinatesType END_POINT   = {{90, 67, 54}};

        const data::helper::MedicalImage::Orientation FRONTAL =
            data::helper::MedicalImage::Y_AXIS;

        const filter::image::BresenhamLine::PathType result =
            filter::image::BresenhamLine::draw(FRONTAL, START_POINT, END_POINT);

        CPPUNIT_ASSERT(!result.empty());
        CPPUNIT_ASSERT_EQUAL(size_t(START_POINT[2] - END_POINT[2] + 1), result.size());

        auto iter = result.begin();
        for(size_t i = START_POINT[2] ; i >= END_POINT[2] ; --i)
        {
            const auto& pathElt = *iter;

            const filter::image::BresenhamLine::CoordinatesType expectedElt = {{90, 67, i}};

            CPPUNIT_ASSERT(pathElt == expectedElt);
            ++iter;
        }
    }
    {
        const filter::image::BresenhamLine::CoordinatesType START_POINT = {{90, 67, 68}};

        const data::helper::MedicalImage::Orientation FRONTAL =
            data::helper::MedicalImage::Y_AXIS;

        const filter::image::BresenhamLine::PathType result =
            filter::image::BresenhamLine::draw(FRONTAL, START_POINT, START_POINT);

        CPPUNIT_ASSERT(!result.empty());
        CPPUNIT_ASSERT_EQUAL(size_t(1), result.size());

        CPPUNIT_ASSERT(START_POINT == result[0]);
    }
}

//------------------------------------------------------------------------------

void BresenhamLineTest::obliqueLineTest()
{
    {
        const filter::image::BresenhamLine::CoordinatesType START_POINT = {{1, 1, 42}};
        const filter::image::BresenhamLine::CoordinatesType END_POINT   = {{11, 5, 42}};

        const data::helper::MedicalImage::Orientation AXIAL =
            data::helper::MedicalImage::Z_AXIS;

        const filter::image::BresenhamLine::PathType result =
            filter::image::BresenhamLine::draw(AXIAL, START_POINT, END_POINT);

        CPPUNIT_ASSERT(!result.empty());

        const BresenhamLine::PathType expectedPath = {
            {{1, 1, 42}},
            {{2, 1, 42}},
            {{3, 2, 42}},
            {{4, 2, 42}},
            {{5, 3, 42}},
            {{6, 3, 42}},
            {{7, 3, 42}},
            {{8, 4, 42}},
            {{9, 4, 42}},
            {{10, 5, 42}},
            {{11, 5, 42}}
        };

        CPPUNIT_ASSERT_EQUAL(expectedPath.size(), result.size());

        auto iter = result.begin();
        for(const auto& expectedPathElt : expectedPath)
        {
            const auto& resElt = *iter;
            CPPUNIT_ASSERT(expectedPathElt == resElt);
            ++iter;
        }
    }

    {
        const filter::image::BresenhamLine::CoordinatesType START_POINT = {{1, 4, 1}};
        const filter::image::BresenhamLine::CoordinatesType END_POINT   = {{1, 2, 5}};

        const data::helper::MedicalImage::Orientation SAGITAL =
            data::helper::MedicalImage::X_AXIS;

        const filter::image::BresenhamLine::PathType result =
            filter::image::BresenhamLine::draw(SAGITAL, START_POINT, END_POINT);

        CPPUNIT_ASSERT(!result.empty());

        const BresenhamLine::PathType expectedPath = {
            {{1, 4, 1}},
            {{1, 4, 2}},
            {{1, 3, 3}},
            {{1, 3, 4}},
            {{1, 2, 5}},
        };

        CPPUNIT_ASSERT_EQUAL(expectedPath.size(), result.size());

        auto iter = result.begin();
        for(const auto& expectedPathElt : expectedPath)
        {
            const auto& resElt = *iter;
            CPPUNIT_ASSERT(expectedPathElt == resElt);
            ++iter;
        }
    }
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::filter::image
