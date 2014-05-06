/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwComEd/Dictionary.hpp>
#include <fwData/Image.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Vector.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/SeriesDB.hpp>

#include "gdcmIO/helper/DicomData.hpp"
#include "gdcmIO/helper/Fiducial.hpp"

namespace gdcmIO
{
namespace helper
{


//------------------------------------------------------------------------------

bool Fiducial::containsLandmarks(SPTR(::fwMedData::SeriesDB) seriesDB)
{
    // Let's find if a series contains distances
    BOOST_FOREACH(const ::fwMedData::Series::sptr& series, seriesDB->getContainer())
    {
        ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
        if(imageSeries)
        {
            ::fwData::Image::sptr image = imageSeries->getImage();
            if(image)
            {
                ::fwData::PointList::sptr pointList =
                        image->getField< ::fwData::PointList >(::fwComEd::Dictionary::m_imageLandmarksId);
                if(pointList && !pointList->getCRefPoints().empty())
                {
                    return true;
                }
            }
        }
    }

    return false;
}

//------------------------------------------------------------------------------

bool Fiducial::containsDistances(SPTR(::fwMedData::SeriesDB) seriesDB)
{
    // Let's find if a series contains distances
    BOOST_FOREACH(const ::fwMedData::Series::sptr& series, seriesDB->getContainer())
    {
        ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
        if(imageSeries)
        {
            ::fwData::Image::sptr image = imageSeries->getImage();
            if(image)
            {
                ::fwData::Vector::sptr distanceVector =
                        image->getField< ::fwData::Vector >(::fwComEd::Dictionary::m_imageDistancesId);
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

bool Fiducial::contains3DDistances(SPTR(::fwMedData::SeriesDB) seriesDB)
{
    // Let's find if a series contains distances
    BOOST_FOREACH(const ::fwMedData::Series::sptr& series, seriesDB->getContainer())
    {
        ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
        if(imageSeries)
        {
            ::fwData::Image::sptr image = imageSeries->getImage();
            if(image)
            {
                ::fwData::Vector::sptr distanceVector =
                        image->getField< ::fwData::Vector >(::fwComEd::Dictionary::m_imageDistancesId);
                if(distanceVector && !distanceVector->empty())
                {
                    BOOST_FOREACH(const ::fwData::Object::sptr& object, distanceVector->getContainer())
                    {
                        ::fwData::PointList::sptr pointList = ::fwData::PointList::dynamicCast(object);
                        if(pointList && pointList->getCRefPoints().size() >= 2)
                        {
                            const ::fwData::Point::sptr point1 = *pointList->getCRefPoints().begin();
                            const ::fwData::Point::sptr point2 = *(++pointList->getCRefPoints().begin());
                            int frameNumber1 = ::gdcmIO::helper::DicomData::convertPointToFrameNumber(image, point1);
                            int frameNumber2 = ::gdcmIO::helper::DicomData::convertPointToFrameNumber(image, point2);
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
} // namespace gdcmIO

