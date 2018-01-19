/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
            ::fwData::Image::sptr image = imageSeries->getImage();
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
                            const ::fwData::Point::sptr point1 = *pointList->getPoints().begin();
                            const ::fwData::Point::sptr point2 = *(++pointList->getPoints().begin());
                            const size_t frameNumber1          =
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
