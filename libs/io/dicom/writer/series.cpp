/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/dicom/writer/series.hpp"

#include "io/dicom/Writer.hpp"
#include "io/dicom/writer/iod/ComprehensiveSRIOD.hpp"
#include "io/dicom/writer/iod/CTMRImageIOD.hpp"
#include "io/dicom/writer/iod/SpatialFiducialsIOD.hpp"
#include "io/dicom/writer/iod/SurfaceSegmentationIOD.hpp"

#include <data/helper/MedicalImage.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/model_series.hpp>
#include <data/point_list.hpp>
#include <data/series.hpp>
#include <data/vector.hpp>

#include <io/__/writer/registry/macros.hpp>

SIGHT_REGISTER_IO_WRITER(sight::io::dicom::writer::series);

namespace sight::io::dicom::writer
{

//------------------------------------------------------------------------------

void series::write()
{
    data::series::csptr series = this->getConcreteObject();
    SIGHT_ASSERT("sight::data::series not instanced", series);

    // TODO: Make the user choose this value and implement EnhancedCTImageIOD/EnhancedMRImageIOD
    bool multiFiles = true;

    // Initialization shared object
    SPTR(io::dicom::container::DicomInstance) instance =
        std::make_shared<io::dicom::container::DicomInstance>(series, nullptr, multiFiles);

    // Retrieve series sop_classUID
    const std::string& sopClassUID = instance->getSOPClassUID();

    if(sopClassUID == gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::CTImageStorage)
       || sopClassUID == gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::MRImageStorage))
    {
        const auto imageSeries = std::dynamic_pointer_cast<const data::image_series>(series);
        SIGHT_ASSERT("sight::data::image_series not instanced", imageSeries);

        // Write image
        io::dicom::writer::iod::CTMRImageIOD imageIOD(instance, this->get_folder() / "im");
        imageIOD.write(series);

        data::point_list::sptr landmarks = data::helper::MedicalImage::getLandmarks(*imageSeries);
        data::vector::sptr distances     = data::helper::MedicalImage::getDistances(*imageSeries);

        if((landmarks && !landmarks->getPoints().empty()) || (distances && !distances->empty()))
        {
            // Write Landmarks and Distances
            if(m_fiducialsExportMode == SPATIAL_FIDUCIALS)
            {
                io::dicom::writer::iod::SpatialFiducialsIOD spatialFiducialsIOD(instance, this->get_folder() / "imSF");
                spatialFiducialsIOD.write(series);
            }
            else
            {
                io::dicom::writer::iod::ComprehensiveSRIOD documentIOD(instance, this->get_folder() / "imSR",
                                                                       m_fiducialsExportMode == COMPREHENSIVE_3D_SR);
                documentIOD.write(series);
            }
        }
    }
    else if(sopClassUID == gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::EnhancedUSVolumeStorage))
    {
        auto seriesSet = std::make_shared<data::series_set>();
        seriesSet->push_back(std::const_pointer_cast<data::series>(series));

        auto writer = std::make_shared<io::dicom::Writer>();
        writer->set_object(seriesSet);
        writer->set_folder(get_folder());
        writer->write();
    }
    else if(sopClassUID == gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::SurfaceSegmentationStorage))
    {
        SPTR(io::dicom::container::DicomInstance) imageInstance = this->getImageInstance();
        io::dicom::writer::iod::SurfaceSegmentationIOD iod(instance, imageInstance, this->get_folder() / "imSeg");
        iod.write(series);
    }
    else
    {
        SIGHT_WARN("series type not supported.");
    }

    // Push instance into container
    m_dicomInstanceMap[series->getSeriesInstanceUID()] = instance;
}

//------------------------------------------------------------------------------

bool series::hasDocumentSR(const data::image_series::csptr& imageSeries)
{
    data::point_list::sptr pl = data::helper::MedicalImage::getLandmarks(*imageSeries);
    const auto distances      = data::helper::MedicalImage::getDistances(*imageSeries);

    // Check if image has landmark and distance
    return (pl && !pl->getPoints().empty()) || distances;
}

//------------------------------------------------------------------------------

SPTR(io::dicom::container::DicomInstance) series::getImageInstance()
{
    SIGHT_ASSERT("You must have created an image instance before trying to access it.", !m_dicomInstanceMap.empty());
    return m_dicomInstanceMap.begin()->second;
}

//------------------------------------------------------------------------------

std::string series::extension() const
{
    return {""};
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer
