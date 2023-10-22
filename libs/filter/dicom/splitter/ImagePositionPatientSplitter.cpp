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

#include "filter/dicom/splitter/ImagePositionPatientSplitter.hpp"

#include "filter/dicom/exceptions/FilterFailure.hpp"
#include "filter/dicom/registry/macros.hpp"

#include <geometry/data/vector_functions.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

SIGHT_REGISTER_DICOM_FILTER(sight::filter::dicom::splitter::ImagePositionPatientSplitter);

namespace sight::filter::dicom::splitter
{

const std::string ImagePositionPatientSplitter::s_FILTER_NAME        = "Image position patient splitter";
const std::string ImagePositionPatientSplitter::s_FILTER_DESCRIPTION =
    "Split instances by finding gaps in image position continuity. This filter assume that "
    "the instances are <b>already sorted</b> and only gaps between volumes remain.";

//-----------------------------------------------------------------------------

std::string ImagePositionPatientSplitter::getName() const
{
    return ImagePositionPatientSplitter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string ImagePositionPatientSplitter::getDescription() const
{
    return ImagePositionPatientSplitter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

ImagePositionPatientSplitter::dicom_series_container_t ImagePositionPatientSplitter::apply(
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
    for(const auto& item : _series->getDicomContainer())
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
            throw sight::filter::dicom::exceptions::FilterFailure(msg);
        }

        fwVec3d image_position;
        for(unsigned int i = 0 ; i < 3 ; ++i)
        {
            dataset->findAndGetFloat64(DCM_ImagePositionPatient, image_position[i], i);
        }

        fwVec3d image_orientation_u;
        fwVec3d image_orientation_v;
        for(unsigned int i = 0 ; i < 3 ; ++i)
        {
            dataset->findAndGetFloat64(DCM_ImageOrientationPatient, image_orientation_u[i], i);
            dataset->findAndGetFloat64(DCM_ImageOrientationPatient, image_orientation_v[i], i + std::size_t(3));
        }

        //Compute Z direction (cross product)
        const fwVec3d z_vector = geometry::data::cross(image_orientation_u, image_orientation_v);

        //Compute dot product to get the index
        const double index = geometry::data::dot(image_position, z_vector);

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
                current_series->setNumberOfInstances(current_series->getDicomContainer().size());
            }

            instance_number = 0;
            current_series  = std::make_shared<data::dicom_series>();
            current_series->shallow_copy(_series);
            current_series->clearDicomContainer();
        }

        current_series->addBinary(instance_number++, buffer_obj);
        previous_index = index;
    }

    // Push last series created
    result.push_back(current_series);
    current_series->setNumberOfInstances(current_series->getDicomContainer().size());

    if(result.size() > 1)
    {
        _logger->warning("Series has been split according to slice positions.");
    }

    return result;
}

} // namespace sight::filter::dicom::splitter
