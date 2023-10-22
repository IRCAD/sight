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

#include <data/helper/medical_image.hpp>
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
    bool multi_files = true;

    // Initialization shared object
    SPTR(io::dicom::container::DicomInstance) instance =
        std::make_shared<io::dicom::container::DicomInstance>(series, nullptr, multi_files);

    // Retrieve series sop_classUID
    const std::string& sop_class_uid = instance->getSOPClassUID();

    if(sop_class_uid == gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::CTImageStorage)
       || sop_class_uid == gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::MRImageStorage))
    {
        const auto image_series = std::dynamic_pointer_cast<const data::image_series>(series);
        SIGHT_ASSERT("sight::data::image_series not instanced", image_series);

        // Write image
        io::dicom::writer::iod::CTMRImageIOD image_iod(instance, this->get_folder() / "im");
        image_iod.write(series);

        data::point_list::sptr landmarks = data::helper::medical_image::get_landmarks(*image_series);
        data::vector::sptr distances     = data::helper::medical_image::get_distances(*image_series);

        if((landmarks && !landmarks->getPoints().empty()) || (distances && !distances->empty()))
        {
            // Write Landmarks and Distances
            if(m_fiducialsExportMode == SPATIAL_FIDUCIALS)
            {
                io::dicom::writer::iod::SpatialFiducialsIOD spatial_fiducials_iod(instance,
                                                                                  this->get_folder() / "imSF");
                spatial_fiducials_iod.write(series);
            }
            else
            {
                io::dicom::writer::iod::ComprehensiveSRIOD document_iod(instance, this->get_folder() / "imSR",
                                                                        m_fiducialsExportMode == COMPREHENSIVE_3D_SR);
                document_iod.write(series);
            }
        }
    }
    else if(sop_class_uid == gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::EnhancedUSVolumeStorage))
    {
        auto series_set = std::make_shared<data::series_set>();
        series_set->push_back(std::const_pointer_cast<data::series>(series));

        auto writer = std::make_shared<io::dicom::Writer>();
        writer->set_object(series_set);
        writer->set_folder(get_folder());
        writer->write();
    }
    else if(sop_class_uid == gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::SurfaceSegmentationStorage))
    {
        SPTR(io::dicom::container::DicomInstance) image_instance = this->getImageInstance();
        io::dicom::writer::iod::SurfaceSegmentationIOD iod(instance, image_instance, this->get_folder() / "imSeg");
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

bool series::hasDocumentSR(const data::image_series::csptr& _image_series)
{
    data::point_list::sptr pl = data::helper::medical_image::get_landmarks(*_image_series);
    const auto distances      = data::helper::medical_image::get_distances(*_image_series);

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
