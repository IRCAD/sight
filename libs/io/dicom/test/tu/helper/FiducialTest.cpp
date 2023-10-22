/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "FiducialTest.hpp"

#include <data/helper/medical_image.hpp>
#include <data/image_series.hpp>
#include <data/series_set.hpp>

#include <io/dicom/helper/Fiducial.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::helper::ut::FiducialTest);

namespace sight::io::dicom::helper::ut
{

//------------------------------------------------------------------------------

void FiducialTest::containsLandmarksTest()
{
    auto series_set   = std::make_shared<data::series_set>();
    auto image_series = std::make_shared<data::image_series>();
    series_set->push_back(image_series);
    CPPUNIT_ASSERT(!Fiducial::containsLandmarks(series_set));
    auto point_list = std::make_shared<data::point_list>();
    point_list->pushBack(std::make_shared<data::point>(1., 2., 3.));
    data::helper::medical_image::set_landmarks(*image_series, point_list);
    CPPUNIT_ASSERT(Fiducial::containsLandmarks(series_set));
}

//------------------------------------------------------------------------------

void FiducialTest::containsDistancesTest()
{
    auto series_set   = std::make_shared<data::series_set>();
    auto image_series = std::make_shared<data::image_series>();
    series_set->push_back(image_series);
    CPPUNIT_ASSERT(!Fiducial::containsDistances(series_set));
    auto vector     = std::make_shared<data::vector>();
    auto point_list = std::make_shared<data::point_list>();
    point_list->pushBack(std::make_shared<data::point>(1., 2., 3.));
    point_list->pushBack(std::make_shared<data::point>(2., 4., 6.));
    vector->push_back(point_list);
    data::helper::medical_image::set_distances(*image_series, vector);
    CPPUNIT_ASSERT(Fiducial::containsDistances(series_set));
}

//------------------------------------------------------------------------------

void FiducialTest::contains3DDistancesTest()
{
    auto series_set   = std::make_shared<data::series_set>();
    auto image_series = std::make_shared<data::image_series>();
    image_series->resize({3, 5, 7}, core::type::UINT8, data::image::PixelFormat::GRAY_SCALE);
    image_series->setSpacing({1, 1, 1});
    series_set->push_back(image_series);
    CPPUNIT_ASSERT(!Fiducial::contains3DDistances(series_set));
    auto vector     = std::make_shared<data::vector>();
    auto point_list = std::make_shared<data::point_list>();
    point_list->pushBack(std::make_shared<data::point>(1., 2.));
    point_list->pushBack(std::make_shared<data::point>(2., 4.));
    vector->push_back(point_list);
    data::helper::medical_image::set_distances(*image_series, vector);
    CPPUNIT_ASSERT(!Fiducial::contains3DDistances(series_set));
    auto point_list3d = std::make_shared<data::point_list>();
    point_list3d->pushBack(std::make_shared<data::point>(1., 2., 3.));
    point_list3d->pushBack(std::make_shared<data::point>(2., 4., 6.));
    vector->push_back(point_list3d);
    data::helper::medical_image::set_distances(*image_series, vector);
    CPPUNIT_ASSERT(Fiducial::contains3DDistances(series_set));
}

} // namespace sight::io::dicom::helper::ut
