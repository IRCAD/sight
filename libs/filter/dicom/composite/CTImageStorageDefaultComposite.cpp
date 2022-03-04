/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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
#include "filter/dicom/splitter/SOPClassUIDSplitter.hpp"
#include "filter/dicom/splitter/TemporalPositionSplitter.hpp"

fwDicomIOFilterRegisterMacro(sight::filter::dicom::composite::CTImageStorageDefaultComposite);

namespace sight::filter::dicom
{

namespace composite
{

const std::string CTImageStorageDefaultComposite::s_FILTER_NAME        = "CTImageStorage default composite";
const std::string CTImageStorageDefaultComposite::s_FILTER_DESCRIPTION =
    "CTImageStorage default composite.";

//-----------------------------------------------------------------------------

CTImageStorageDefaultComposite::CTImageStorageDefaultComposite(filter::dicom::IFilter::Key) :
    IComposite()
{
    // Split according to image type
    this->addChild(filter::dicom::splitter::ImageTypeSplitter::New());
    // Split according to acquisition number
    this->addChild(filter::dicom::splitter::AcquisitionNumberSplitter::New());
    // Split according to temporal position (MR)
    this->addChild(filter::dicom::splitter::TemporalPositionSplitter::New());
    // Split according to image dimensions
    this->addChild(filter::dicom::splitter::ImageDimensionsSplitter::New());
    // Sort according to instance number
    this->addChild(filter::dicom::sorter::InstanceNumberSorter::New());
    // Sort according to image position
    this->addChild(filter::dicom::sorter::ImagePositionPatientSorter::New());
    // Split if gaps are found in image positions
    this->addChild(filter::dicom::splitter::ImagePositionPatientSplitter::New());
    // Sort according to image position to get correct orientation
    this->addChild(filter::dicom::sorter::ImagePositionPatientSorter::New());
    // Correct slice thickness if needed
    this->addChild(filter::dicom::modifier::SliceThicknessModifier::New());
}

//-----------------------------------------------------------------------------

CTImageStorageDefaultComposite::~CTImageStorageDefaultComposite()
{
}

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

} // namespace composite

} // namespace sight::filter::dicom
