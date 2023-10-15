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

#include "filter/dicom/composite/CTImageStorageDefaultComposite.hpp"

#include "filter/dicom/modifier/SliceThicknessModifier.hpp"
#include "filter/dicom/registry/macros.hpp"
#include "filter/dicom/sorter/ImagePositionPatientSorter.hpp"
#include "filter/dicom/sorter/InstanceNumberSorter.hpp"
#include "filter/dicom/splitter/AcquisitionNumberSplitter.hpp"
#include "filter/dicom/splitter/ImageDimensionsSplitter.hpp"
#include "filter/dicom/splitter/ImagePositionPatientSplitter.hpp"
#include "filter/dicom/splitter/ImageTypeSplitter.hpp"
#include "filter/dicom/splitter/sop_class_uid_splitter.hpp"
#include "filter/dicom/splitter/TemporalPositionSplitter.hpp"

SIGHT_REGISTER_DICOM_FILTER(sight::filter::dicom::composite::CTImageStorageDefaultComposite);

namespace sight::filter::dicom::composite
{

const std::string CTImageStorageDefaultComposite::s_FILTER_NAME        = "CTImageStorage default composite";
const std::string CTImageStorageDefaultComposite::s_FILTER_DESCRIPTION =
    "CTImageStorage default composite.";

//-----------------------------------------------------------------------------

CTImageStorageDefaultComposite::CTImageStorageDefaultComposite()
{
    // Split according to image type
    this->addChild(std::make_shared<sight::filter::dicom::splitter::ImageTypeSplitter>());
    // Split according to acquisition number
    this->addChild(std::make_shared<sight::filter::dicom::splitter::AcquisitionNumberSplitter>());
    // Split according to temporal position (MR)
    this->addChild(std::make_shared<sight::filter::dicom::splitter::TemporalPositionSplitter>());
    // Split according to image dimensions
    this->addChild(std::make_shared<sight::filter::dicom::splitter::ImageDimensionsSplitter>());
    // Sort according to instance number
    this->addChild(std::make_shared<sight::filter::dicom::sorter::InstanceNumberSorter>());
    // Sort according to image position
    this->addChild(std::make_shared<sight::filter::dicom::sorter::ImagePositionPatientSorter>());
    // Split if gaps are found in image positions
    this->addChild(std::make_shared<sight::filter::dicom::splitter::ImagePositionPatientSplitter>());
    // Sort according to image position to get correct orientation
    this->addChild(std::make_shared<sight::filter::dicom::sorter::ImagePositionPatientSorter>());
    // Correct slice thickness if needed
    this->addChild(std::make_shared<sight::filter::dicom::modifier::SliceThicknessModifier>());
}

//-----------------------------------------------------------------------------

CTImageStorageDefaultComposite::~CTImageStorageDefaultComposite()
= default;

//-----------------------------------------------------------------------------

std::string CTImageStorageDefaultComposite::getName() const
{
    return CTImageStorageDefaultComposite::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string CTImageStorageDefaultComposite::getDescription() const
{
    return CTImageStorageDefaultComposite::s_FILTER_DESCRIPTION;
}

} // namespace sight::filter::dicom::composite
