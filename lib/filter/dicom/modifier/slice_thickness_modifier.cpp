/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "filter/dicom/modifier/slice_thickness_modifier.hpp"

#include "filter/dicom/exceptions/filter_failure.hpp"
#include "filter/dicom/registry/macros.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

#include <glm/glm.hpp>

SIGHT_REGISTER_DICOM_FILTER(sight::filter::dicom::modifier::slice_thickness_modifier);

namespace sight::filter::dicom::modifier
{

const std::string slice_thickness_modifier::FILTER_NAME        = "Slice thickness modifier";
const std::string slice_thickness_modifier::FILTER_DESCRIPTION =
    "Compute and modify slice thickness using <i>ImagePositionPatient</i> "
    "and <i>ImageOrientationPatient</i> tags of the two first instances.";

//-----------------------------------------------------------------------------

std::string slice_thickness_modifier::get_name() const
{
    return slice_thickness_modifier::FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string slice_thickness_modifier::get_description() const
{
    return slice_thickness_modifier::FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

slice_thickness_modifier::dicom_series_container_t slice_thickness_modifier::apply(
    const data::dicom_series::sptr& _series,
    const core::log::logger::sptr&
    /*logger*/
) const
{
    dicom_series_container_t result;

    if(_series->get_dicom_container().size() < 2)
    {
        SIGHT_WARN("slice_thickness_modifier is being applied on a series containing only one slice.");
        result.push_back(_series);
        return result;
    }

    // Retrieve the two first instances
    auto first_item = _series->get_dicom_container().begin();

    const core::memory::buffer_object::sptr& first_buffer_obj  = first_item->second;
    const core::memory::buffer_object::sptr& second_buffer_obj = (++first_item)->second;

    // Compute the slice thickness between the 2 first slices.
    const double first_index     = this->get_instance_z_position(first_buffer_obj);
    const double second_index    = this->get_instance_z_position(second_buffer_obj);
    const double slice_thickness = std::abs(second_index - first_index);

    // Check that the computed sliceThickness doesn't match the sliceThickness of the first instance
    const double current_slice_thickness = this->get_slice_thickness(first_buffer_obj);
    const double epsilon                 = 1e-2;

    // If the computed sliceThickness doesn't match the sliceThickness value
    // we add the computed value to the DicomSeries.
    if(std::abs(slice_thickness - current_slice_thickness) > epsilon)
    {
        _series->add_computed_tag_value("SliceThickness", std::to_string(slice_thickness));
    }

    result.push_back(_series);
    return result;
}

//-----------------------------------------------------------------------------

double slice_thickness_modifier::get_instance_z_position(const core::memory::buffer_object::sptr& _buffer_obj) const
{
    DcmFileFormat file_format;
    DcmDataset* dataset = nullptr;

    const std::size_t buff_size = _buffer_obj->size();
    core::memory::buffer_object::lock_t lock(_buffer_obj);
    char* buffer = static_cast<char*>(lock.buffer());

    DcmInputBufferStream is;
    is.setBuffer(buffer, offile_off_t(buff_size));
    is.setEos();

    file_format.transferInit();
    if(!file_format.read(is).good())
    {
        SIGHT_THROW("Unable to read Dicom file '" << _buffer_obj->get_stream_info().fs_file.string() << "'");
    }

    file_format.loadAllDataIntoMemory();
    file_format.transferEnd();

    dataset = file_format.getDataset();

    if(!dataset->tagExists(DCM_ImagePositionPatient) || !dataset->tagExists(DCM_ImageOrientationPatient))
    {
        const std::string msg = "Unable to compute the SliceThickness of the series.";
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

    return index;
}

//-----------------------------------------------------------------------------

double slice_thickness_modifier::get_slice_thickness(const core::memory::buffer_object::sptr& _buffer_obj) const
{
    DcmFileFormat file_format;
    OFCondition status;
    DcmDataset* dataset = nullptr;

    const std::size_t buff_size = _buffer_obj->size();
    core::memory::buffer_object::lock_t lock(_buffer_obj);
    char* buffer = static_cast<char*>(lock.buffer());

    DcmInputBufferStream is;
    is.setBuffer(buffer, offile_off_t(buff_size));
    is.setEos();

    file_format.transferInit();
    if(!file_format.read(is).good())
    {
        SIGHT_THROW("Unable to read Dicom file '" << _buffer_obj->get_stream_info().fs_file.string() << "'");
    }

    file_format.loadAllDataIntoMemory();
    file_format.transferEnd();

    dataset = file_format.getDataset();

    double slice_thickness = 0.;
    dataset->findAndGetFloat64(DCM_SliceThickness, slice_thickness);

    return slice_thickness;
}

} // namespace sight::filter::dicom::modifier
