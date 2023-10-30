/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "filter/dicom/composite/ct_image_storage_default_composite.hpp"

#include "filter/dicom/modifier/slice_thickness_modifier.hpp"
#include "filter/dicom/registry/macros.hpp"
#include "filter/dicom/sorter/image_position_patient_sorter.hpp"
#include "filter/dicom/sorter/instance_number_sorter.hpp"
#include "filter/dicom/splitter/acquisition_number_splitter.hpp"
#include "filter/dicom/splitter/image_dimensions_splitter.hpp"
#include "filter/dicom/splitter/image_position_patient_splitter.hpp"
#include "filter/dicom/splitter/image_type_splitter.hpp"
#include "filter/dicom/splitter/sop_class_uid_splitter.hpp"
#include "filter/dicom/splitter/temporal_position_splitter.hpp"

SIGHT_REGISTER_DICOM_FILTER(sight::filter::dicom::composite::ct_image_storage_default_composite);

namespace sight::filter::dicom::composite
{

const std::string ct_image_storage_default_composite::FILTER_NAME        = "CTImageStorage default composite";
const std::string ct_image_storage_default_composite::FILTER_DESCRIPTION =
    "CTImageStorage default composite.";

//-----------------------------------------------------------------------------

ct_image_storage_default_composite::ct_image_storage_default_composite()
{
    // Split according to image type
    this->add_child(std::make_shared<sight::filter::dicom::splitter::image_type_splitter>());
    // Split according to acquisition number
    this->add_child(std::make_shared<sight::filter::dicom::splitter::acquisition_number_splitter>());
    // Split according to temporal position (MR)
    this->add_child(std::make_shared<sight::filter::dicom::splitter::temporal_position_splitter>());
    // Split according to image dimensions
    this->add_child(std::make_shared<sight::filter::dicom::splitter::image_dimensions_splitter>());
    // Sort according to instance number
    this->add_child(std::make_shared<sight::filter::dicom::sorter::instance_number_sorter>());
    // Sort according to image position
    this->add_child(std::make_shared<sight::filter::dicom::sorter::image_position_patient_sorter>());
    // Split if gaps are found in image positions
    this->add_child(std::make_shared<sight::filter::dicom::splitter::image_position_patient_splitter>());
    // Sort according to image position to get correct orientation
    this->add_child(std::make_shared<sight::filter::dicom::sorter::image_position_patient_sorter>());
    // Correct slice thickness if needed
    this->add_child(std::make_shared<sight::filter::dicom::modifier::slice_thickness_modifier>());
}

//-----------------------------------------------------------------------------

ct_image_storage_default_composite::~ct_image_storage_default_composite()
= default;

//-----------------------------------------------------------------------------

std::string ct_image_storage_default_composite::get_name() const
{
    return ct_image_storage_default_composite::FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string ct_image_storage_default_composite::get_description() const
{
    return ct_image_storage_default_composite::FILTER_DESCRIPTION;
}

} // namespace sight::filter::dicom::composite
