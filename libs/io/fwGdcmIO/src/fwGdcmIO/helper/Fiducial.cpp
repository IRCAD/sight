/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwGdcmIO/helper/Fiducial.hpp"

#include "fwGdcmIO/helper/DicomDataTools.hpp"

#include <fwData/Image.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Vector.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/SeriesDB.hpp>

namespace fwGdcmIO
{
namespace helper
{

//------------------------------------------------------------------------------

bool Fiducial::containsLandmarks(const SPTR(::fwMedData::SeriesDB)& seriesDB)
{
    // Let's find if a series contains distances
    for(const ::fwMedData::Series::sptr& series : seriesDB->getContainer())
    {
        ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
        if(imageSeries)
        {
            ::fwData::Image::sptr image = imageSeries->getImage();
            if(image)
            {
                ::fwData::PointList::sptr pointList =
                    image->getField< ::fwData::PointList >(::fwDataTools::fieldHelper::Image::m_imageLandmarksId);
                if(pointList && !pointList->getPoints().empty())
                {
                    return true;
                }
            }
        }
    }

    return false;
}

//------------------------------------------------------------------------------

bool Fiducial::containsDistances(const SPTR(::fwMedData::SeriesDB)& seriesDB)
{
    // Let's find if a series contains distances
    for(const ::fwMedData::Series::sptr& series : seriesDB->getContainer())
    {
        ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
        if(imageSeries)
        {
            ::fwData::Image::sptr image = imageSeries->getImage();
            if(image)
            {
                ::fwData::Vector::sptr distanceVector =
                    image->getField< ::fwData::Vector >(::fwDataTools::fieldHelper::Image::m_imageDistancesId);
                if(distanceVector && !distanceVector->empty())
                {
                    return true;
                }
            }
        }
    }

    return false;
}

//------------------------------------------------------------------------------

bool Fiducial::contains3DDistances(const SPTR(::fwMedData::SeriesDB)& seriesDB)
{
    // Let's find if a series contains distances
    for(const ::fwMedData::Series::sptr& series : seriesDB->getContainer())
    {
        ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
        if(imageSeries)
        {
            ::fwData::Image::csptr image = imageSeries->getImage();
            if(image)
            {
                ::fwData::Vector::sptr distanceVector =
                    image->getField< ::fwData::Vector >(::fwDataTools::fieldHelper::Image::m_imageDistancesId);
                if(distanceVector && !distanceVector->empty())
                {
                    for(const ::fwData::Object::sptr& object : distanceVector->getContainer())
                    {
                        ::fwData::PointList::sptr pointList = ::fwData::PointList::dynamicCast(object);
                        if(pointList && pointList->getPoints().size() >= 2)
                        {
                            const ::fwData::Point::csptr point1 = *pointList->getPoints().begin();
                            const ::fwData::Point::csptr point2 = *(++pointList->getPoints().begin());
                            const size_t frameNumber1           =
                                ::fwGdcmIO::helper::DicomDataTools::convertPointToFrameNumber(image, point1);
                            const size_t frameNumber2 =
                                ::fwGdcmIO::helper::DicomDataTools::convertPointToFrameNumber(image, point2);
                            if(frameNumber1 != frameNumber2)
                            {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}

//------------------------------------------------------------------------------

} // namespace helper
} // namespace fwGdcmIO
