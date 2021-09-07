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

#include "filter/dicom/splitter/AcquisitionNumberSplitter.hpp"

#include "filter/dicom/registry/macros.hpp"

#include <dcmtk/dcmdata/dcdeftag.h>

#include <string>

fwDicomIOFilterRegisterMacro(::sight::filter::dicom::splitter::AcquisitionNumberSplitter);

namespace sight::filter::dicom
{

namespace splitter
{

const std::string AcquisitionNumberSplitter::s_FILTER_NAME        = "Acquisition number splitter";
const std::string AcquisitionNumberSplitter::s_FILTER_DESCRIPTION =
    "Split instances according to acquisition number tag.";

//-----------------------------------------------------------------------------

AcquisitionNumberSplitter::AcquisitionNumberSplitter(filter::dicom::IFilter::Key key) :
    TagValueSplitter(key)
{
    this->setTag(DCM_AcquisitionNumber);
}

//-----------------------------------------------------------------------------

AcquisitionNumberSplitter::~AcquisitionNumberSplitter()
{
}

//-----------------------------------------------------------------------------

std::string AcquisitionNumberSplitter::getName() const
{
    return AcquisitionNumberSplitter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string AcquisitionNumberSplitter::getDescription() const
{
    return AcquisitionNumberSplitter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool AcquisitionNumberSplitter::isConfigurationRequired() const
{
    return false;
}

//-----------------------------------------------------------------------------

} // namespace splitter

} // namespace sight::filter::dicom
