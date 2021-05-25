/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "io/dicom/writer/Series.hpp"

#include "io/dicom/writer/iod/ComprehensiveSRIOD.hpp"
#include "io/dicom/writer/iod/CTMRImageIOD.hpp"
#include "io/dicom/writer/iod/SpatialFiducialsIOD.hpp"
#include "io/dicom/writer/iod/SurfaceSegmentationIOD.hpp"

#include <data/fieldHelper/Image.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>
#include <data/PointList.hpp>
#include <data/Series.hpp>
#include <data/Vector.hpp>

#include <io/base/writer/registry/macros.hpp>

SIGHT_REGISTER_IO_WRITER(::sight::io::dicom::writer::Series);

namespace sight::io::dicom
{

namespace writer
{

//------------------------------------------------------------------------------

Series::Series(io::base::writer::IObjectWriter::Key) :
    m_fiducialsExportMode(SPATIAL_FIDUCIALS)
{
}

//------------------------------------------------------------------------------

Series::~Series()
{
}

//------------------------------------------------------------------------------

void Series::write()
{
    data::Series::csptr series = this->getConcreteObject();
    SIGHT_ASSERT("::sight::data::Series not instanced", series);

    // TODO: Make the user choose this value and implement EnhancedCTImageIOD/EnhancedMRImageIOD
    bool multiFiles = true;

    // Initialization shared object
    SPTR(io::dicom::container::DicomInstance) instance =
        std::make_shared<io::dicom::container::DicomInstance>(series, nullptr, multiFiles);

    // Retrieve series SOPClassUID
    const std::string& sopClassUID = instance->getSOPClassUID();

    if(sopClassUID == ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::CTImageStorage)
       || sopClassUID == ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::MRImageStorage))
    {
        data::ImageSeries::csptr imageSeries = data::ImageSeries::dynamicCast(series);
        SIGHT_ASSERT("::sight::data::ImageSeries not instanced", imageSeries);
        data::Image::sptr image = imageSeries->getImage();
        SIGHT_ASSERT("::sight::data::Image not instanced", image);

        // Write image
        io::dicom::writer::iod::CTMRImageIOD imageIOD(instance, this->getFolder() / "im");
        imageIOD.write(series);

        data::PointList::sptr landmarks =
            image->getField<data::PointList>(data::fieldHelper::Image::m_imageLandmarksId);
        data::Vector::sptr distances =
            image->getField<data::Vector>(data::fieldHelper::Image::m_imageDistancesId);
        if((landmarks && !landmarks->getPoints().empty()) || (distances && !distances->empty()))
        {
            // Write Landmarks and Distances
            if(m_fiducialsExportMode == SPATIAL_FIDUCIALS)
            {
                io::dicom::writer::iod::SpatialFiducialsIOD spatialFiducialsIOD(instance, this->getFolder() / "imSF");
                spatialFiducialsIOD.write(series);
            }
            else
            {
                io::dicom::writer::iod::ComprehensiveSRIOD documentIOD(instance, this->getFolder() / "imSR",
                                                                       m_fiducialsExportMode == COMPREHENSIVE_3D_SR);
                documentIOD.write(series);
            }
        }
    }
    else if(sopClassUID == ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::SurfaceSegmentationStorage))
    {
        SPTR(io::dicom::container::DicomInstance) imageInstance = this->getImageInstance();
        io::dicom::writer::iod::SurfaceSegmentationIOD iod(instance, imageInstance, this->getFolder() / "imSeg");
        iod.write(series);
    }
    else
    {
        SIGHT_WARN("Series type not supported.");
    }

    // Push instance into container
    m_dicomInstanceMap[series->getInstanceUID()] = instance;
}

//------------------------------------------------------------------------------

bool Series::hasDocumentSR(const data::ImageSeries::csptr& imageSeries) const
{
    data::Image::csptr image = imageSeries->getImage();
    SIGHT_ASSERT("Image not instanced", image);

    data::PointList::sptr pl;
    pl = image->getField<data::PointList>(data::fieldHelper::Image::m_imageLandmarksId);

    // Check if image has landmark and distance
    return (pl && pl->getPoints().size() > 0)
           || image->getField(data::fieldHelper::Image::m_imageDistancesId);
}

//------------------------------------------------------------------------------

SPTR(io::dicom::container::DicomInstance) Series::getImageInstance()
{
    SIGHT_ASSERT("You must have created an image instance before trying to access it.", !m_dicomInstanceMap.empty());
    return m_dicomInstanceMap.begin()->second;
}

//------------------------------------------------------------------------------

std::string Series::extension()
{
    return std::string("");
}

//------------------------------------------------------------------------------

} // namespace writer

} // namespace sight::io::dicom
