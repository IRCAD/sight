/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/dicom/helper/Fiducial.hpp"

#include "io/dicom/helper/DicomDataTools.hpp"

#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/PointList.hpp>
#include <data/SeriesSet.hpp>
#include <data/Vector.hpp>

namespace sight::io::dicom::helper
{

//------------------------------------------------------------------------------

bool Fiducial::containsLandmarks(const SPTR(data::SeriesSet)& seriesSet)
{
    // Let's find if a series contains distances
    return std::any_of(
        seriesSet->begin(),
        seriesSet->end(),
        [](const data::Series::sptr& series)
        {
            if(auto imageSeries = std::dynamic_pointer_cast<data::ImageSeries>(series))
            {
                if(data::PointList::sptr pointList = data::helper::MedicalImage::getLandmarks(*imageSeries))
                {
                    return !pointList->getPoints().empty();
                }
            }

            return false;
        });
}

//------------------------------------------------------------------------------

bool Fiducial::containsDistances(const SPTR(data::SeriesSet)& seriesSet)
{
    return std::any_of(
        seriesSet->begin(),
        seriesSet->end(),
        [](const data::Series::sptr& series)
        {
            if(auto imageSeries = std::dynamic_pointer_cast<data::ImageSeries>(series))
            {
                if(auto distanceVector = data::helper::MedicalImage::getDistances(*imageSeries))
                {
                    return !distanceVector->empty();
                }
            }

            return false;
        });
}

//------------------------------------------------------------------------------

bool Fiducial::contains3DDistances(const SPTR(data::SeriesSet)& seriesSet)
{
    // Let's find if a series contains distances
    for(const auto& series : *seriesSet)
    {
        const auto& imageSeries = std::dynamic_pointer_cast<data::ImageSeries>(series);
        if(imageSeries)
        {
            const auto& distanceVector = data::helper::MedicalImage::getDistances(*imageSeries);
            if(distanceVector && !distanceVector->empty())
            {
                for(const auto& object : *distanceVector)
                {
                    const auto& pointList = std::dynamic_pointer_cast<data::PointList>(object);
                    if(pointList && pointList->getPoints().size() >= 2)
                    {
                        const auto& point1             = *pointList->getPoints().begin();
                        const auto& point2             = *(++pointList->getPoints().begin());
                        const std::size_t frameNumber1 =
                            io::dicom::helper::DicomDataTools::convertPointToFrameNumber(imageSeries, point1);
                        const std::size_t frameNumber2 =
                            io::dicom::helper::DicomDataTools::convertPointToFrameNumber(imageSeries, point2);
                        if(frameNumber1 != frameNumber2)
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::helper
