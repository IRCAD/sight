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

#include <data/helper/MedicalImage.hpp>
#include <data/ImageSeries.hpp>
#include <data/SeriesSet.hpp>

#include <io/dicom/helper/Fiducial.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::helper::ut::FiducialTest);

namespace sight::io::dicom::helper::ut
{

//------------------------------------------------------------------------------

void FiducialTest::containsLandmarksTest()
{
    auto seriesSet   = std::make_shared<data::SeriesSet>();
    auto imageSeries = std::make_shared<data::ImageSeries>();
    seriesSet->push_back(imageSeries);
    CPPUNIT_ASSERT(!Fiducial::containsLandmarks(seriesSet));
    auto pointList = std::make_shared<data::PointList>();
    pointList->pushBack(std::make_shared<data::Point>(1., 2., 3.));
    data::helper::MedicalImage::setLandmarks(*imageSeries, pointList);
    CPPUNIT_ASSERT(Fiducial::containsLandmarks(seriesSet));
}

//------------------------------------------------------------------------------

void FiducialTest::containsDistancesTest()
{
    auto seriesSet   = std::make_shared<data::SeriesSet>();
    auto imageSeries = std::make_shared<data::ImageSeries>();
    seriesSet->push_back(imageSeries);
    CPPUNIT_ASSERT(!Fiducial::containsDistances(seriesSet));
    auto vector    = std::make_shared<data::Vector>();
    auto pointList = std::make_shared<data::PointList>();
    pointList->pushBack(std::make_shared<data::Point>(1., 2., 3.));
    pointList->pushBack(std::make_shared<data::Point>(2., 4., 6.));
    vector->push_back(pointList);
    data::helper::MedicalImage::setDistances(*imageSeries, vector);
    CPPUNIT_ASSERT(Fiducial::containsDistances(seriesSet));
}

//------------------------------------------------------------------------------

void FiducialTest::contains3DDistancesTest()
{
    auto seriesSet   = std::make_shared<data::SeriesSet>();
    auto imageSeries = std::make_shared<data::ImageSeries>();
    imageSeries->resize({3, 5, 7}, core::type::UINT8, data::Image::PixelFormat::GRAY_SCALE);
    imageSeries->setSpacing({1, 1, 1});
    seriesSet->push_back(imageSeries);
    CPPUNIT_ASSERT(!Fiducial::contains3DDistances(seriesSet));
    auto vector    = std::make_shared<data::Vector>();
    auto pointList = std::make_shared<data::PointList>();
    pointList->pushBack(std::make_shared<data::Point>(1., 2.));
    pointList->pushBack(std::make_shared<data::Point>(2., 4.));
    vector->push_back(pointList);
    data::helper::MedicalImage::setDistances(*imageSeries, vector);
    CPPUNIT_ASSERT(!Fiducial::contains3DDistances(seriesSet));
    auto pointList3d = std::make_shared<data::PointList>();
    pointList3d->pushBack(std::make_shared<data::Point>(1., 2., 3.));
    pointList3d->pushBack(std::make_shared<data::Point>(2., 4., 6.));
    vector->push_back(pointList3d);
    data::helper::MedicalImage::setDistances(*imageSeries, vector);
    CPPUNIT_ASSERT(Fiducial::contains3DDistances(seriesSet));
}

} // namespace sight::io::dicom::helper::ut
