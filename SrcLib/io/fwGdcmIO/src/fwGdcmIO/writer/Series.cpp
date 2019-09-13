/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwGdcmIO/writer/Series.hpp"

#include "fwGdcmIO/writer/iod/ComprehensiveSRIOD.hpp"
#include "fwGdcmIO/writer/iod/CTMRImageIOD.hpp"
#include "fwGdcmIO/writer/iod/SpatialFiducialsIOD.hpp"
#include "fwGdcmIO/writer/iod/SurfaceSegmentationIOD.hpp"

#include <fwData/Image.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Vector.hpp>

#include <fwDataIO/writer/registry/macros.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Series.hpp>

fwDataIOWriterRegisterMacro(::fwGdcmIO::writer::Series);

namespace fwGdcmIO
{
namespace writer
{

//------------------------------------------------------------------------------

Series::Series(::fwDataIO::writer::IObjectWriter::Key ) :
    ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >(this),
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
    ::fwMedData::Series::csptr series = this->getConcreteObject();
    SLM_ASSERT("::fwMedData::Series not instanced", series);

    // TODO: Make the user choose this value and implement EnhancedCTImageIOD/EnhancedMRImageIOD
    bool multiFiles = true;

    // Initialization shared object
    SPTR(::fwGdcmIO::container::DicomInstance) instance =
        std::make_shared< ::fwGdcmIO::container::DicomInstance >(series, nullptr, multiFiles);

    // Retrieve series SOPClassUID
    const std::string& sopClassUID = instance->getSOPClassUID();

    if(sopClassUID == ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::CTImageStorage) ||
       sopClassUID == ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::MRImageStorage))
    {
        ::fwMedData::ImageSeries::csptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
        SLM_ASSERT("::fwMedData::ImageSeries not instanced", imageSeries);
        ::fwData::Image::sptr image = imageSeries->getImage();
        SLM_ASSERT("::fwData::Image not instanced", image);

        // Write image
        ::fwGdcmIO::writer::iod::CTMRImageIOD imageIOD(instance, this->getFolder() / "im");
        imageIOD.write(series);

        ::fwData::PointList::sptr landmarks =
            image->getField< ::fwData::PointList >(::fwDataTools::fieldHelper::Image::m_imageLandmarksId);
        ::fwData::Vector::sptr distances =
            image->getField< ::fwData::Vector >(::fwDataTools::fieldHelper::Image::m_imageDistancesId);
        if((landmarks && !landmarks->getPoints().empty()) || (distances && !distances->empty()))
        {
            // Write Landmarks and Distances
            if(m_fiducialsExportMode == SPATIAL_FIDUCIALS)
            {
                ::fwGdcmIO::writer::iod::SpatialFiducialsIOD spatialFiducialsIOD(instance, this->getFolder() / "imSF");
                spatialFiducialsIOD.write(series);
            }
            else
            {
                ::fwGdcmIO::writer::iod::ComprehensiveSRIOD documentIOD(instance, this->getFolder() / "imSR",
                                                                        m_fiducialsExportMode == COMPREHENSIVE_3D_SR);
                documentIOD.write(series);
            }
        }
    }
    else if(sopClassUID == ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::SurfaceSegmentationStorage))
    {
        SPTR(::fwGdcmIO::container::DicomInstance) imageInstance = this->getImageInstance();
        ::fwGdcmIO::writer::iod::SurfaceSegmentationIOD iod(instance, imageInstance, this->getFolder() / "imSeg");
        iod.write(series);
    }
    else
    {
        SLM_WARN("Series type not supported.");
    }

    // Push instance into container
    m_dicomInstanceMap[series->getInstanceUID()] = instance;

}

//------------------------------------------------------------------------------

bool Series::hasDocumentSR(const ::fwMedData::ImageSeries::csptr& imageSeries) const
{
    ::fwData::Image::csptr image = imageSeries->getImage();
    SLM_ASSERT("Image not instanced", image);

    ::fwData::PointList::sptr pl;
    pl = image->getField< ::fwData::PointList >(::fwDataTools::fieldHelper::Image::m_imageLandmarksId);
    // Check if image has landmark and distance
    return ((pl && pl->getPoints().size() > 0) ||
            image->getField(::fwDataTools::fieldHelper::Image::m_imageDistancesId));
}

//------------------------------------------------------------------------------

SPTR(::fwGdcmIO::container::DicomInstance) Series::getImageInstance()
{
    SLM_ASSERT("You must have created an image instance before trying to access it.", !m_dicomInstanceMap.empty());
    return m_dicomInstanceMap.begin()->second;
}

//------------------------------------------------------------------------------

std::string Series::extension()
{
    return std::string("");
}

//------------------------------------------------------------------------------

}// namespace writer
} // namespace fwGdcmIO
