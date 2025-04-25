/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "filter/dicom/splitter/image_position_patient_splitter.hpp"

#include "filter/dicom/exceptions/filter_failure.hpp"
#include "filter/dicom/registry/macros.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

#include <glm/glm.hpp>

SIGHT_REGISTER_DICOM_FILTER(sight::filter::dicom::splitter::image_position_patient_splitter);

namespace sight::filter::dicom::splitter
{

const std::string image_position_patient_splitter::FILTER_NAME        = "Image position patient splitter";
const std::string image_position_patient_splitter::FILTER_DESCRIPTION =
    "Split instances by finding gaps in image position continuity. This filter assume that "
    "the instances are <b>already sorted</b> and only gaps between volumes remain.";

//-----------------------------------------------------------------------------

std::string image_position_patient_splitter::get_name() const
{
    return image_position_patient_splitter::FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string image_position_patient_splitter::get_description() const
{
    return image_position_patient_splitter::FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

image_position_patient_splitter::dicom_series_container_t image_position_patient_splitter::apply(
    const data::dicom_series::sptr& _series,
    const core::log::logger::sptr& _logger
)
const
{
    dicom_series_container_t result;

    OFCondition status;
    double previous_index         = 0.;
    unsigned int instance_number  = 0;
    double spacing_between_slices = 0.;
    const double epsilon          = 1e-2; // Value used to find a gap
    data::dicom_series::sptr current_series;
    for(const auto& item : _series->get_dicom_container())
    {
        const core::memory::buffer_object::sptr buffer_obj = item.second;
        const std::size_t buff_size                        = buffer_obj->size();
        core::memory::buffer_object::lock_t lock(buffer_obj);
        char* buffer = static_cast<char*>(lock.buffer());

        DcmInputBufferStream is;
        is.setBuffer(buffer, offile_off_t(buff_size));
        is.setEos();

        DcmFileFormat file_format;
        file_format.transferInit();
        if(!file_format.read(is).good())
        {
            SIGHT_THROW(
                "Unable to read Dicom file '" << buffer_obj->get_stream_info().fs_file.string() << "' "
                << "(slice: '" << item.first << "')"
            );
        }

        file_format.loadAllDataIntoMemory();
        file_format.transferEnd();

        DcmDataset* dataset = file_format.getDataset();

        if(!dataset->tagExists(DCM_ImagePositionPatient) || !dataset->tagExists(DCM_ImageOrientationPatient))
        {
            const std::string msg =
                "Unable to split the series using ImagePositionPatient and ImageOrientationPatient. "
                "Tag(s) missing.";
            throw sight::filter::dicom::exceptions::filter_failure(msg);
        }

        glm::dvec3 image_position;
        for(unsigned int i = 0 ; i < 3 ; ++i)
        {
            dataset->findAndGetFloat64(DCM_ImagePositionPatient, image_position[int(i)], i);
        }

        glm::dvec3 image_orientation_u;
        glm::dvec3 image_orientation_v;
        for(unsigned int i = 0 ; i < 3 ; ++i)
        {
            dataset->findAndGetFloat64(DCM_ImageOrientationPatient, image_orientation_u[int(i)], i);
            dataset->findAndGetFloat64(DCM_ImageOrientationPatient, image_orientation_v[int(i)], i + std::size_t(3));
        }

        //Compute Z direction (cross product)
        const glm::dvec3 z_vector = glm::cross(image_orientation_u, image_orientation_v);

        //Compute dot product to get the index
        const double index = glm::dot(image_position, z_vector);

        //Compute spacing
        const double spacing = index - previous_index;
        if(current_series && fabs(spacing_between_slices) < epsilon)
        {
            spacing_between_slices = spacing;
        }

        // First frame or volume detected: We create a new Series
        if(!current_series || (fabs(spacing - spacing_between_slices) > epsilon))
        {
            if(current_series)
            {
                result.push_back(current_series);
                current_series->set_number_of_instances(current_series->get_dicom_container().size());
            }

            instance_number = 0;
            current_series  = std::make_shared<data::dicom_series>();
            current_series->shallow_copy(_series);
            current_series->clear_dicom_container();
        }

        current_series->add_binary(instance_number++, buffer_obj);
        previous_index = index;
    }

    // Push last series created
    result.push_back(current_series);
    current_series->set_number_of_instances(current_series->get_dicom_container().size());

    if(result.size() > 1)
    {
        _logger->warning("Series has been split according to slice positions.");
    }

    return result;
}

} // namespace sight::filter::dicom::splitter
