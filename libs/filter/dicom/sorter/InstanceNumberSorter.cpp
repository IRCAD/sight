/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "filter/dicom/sorter/InstanceNumberSorter.hpp"

#include "filter/dicom/exceptions/FilterFailure.hpp"
#include "filter/dicom/registry/macros.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

SIGHT_REGISTER_DICOM_FILTER(sight::filter::dicom::sorter::InstanceNumberSorter);

namespace sight::filter::dicom::sorter
{

const std::string InstanceNumberSorter::s_FILTER_NAME        = "Instance number sorter";
const std::string InstanceNumberSorter::s_FILTER_DESCRIPTION =
    "Sort instances using <i>InstanceNumber</i> tag.";

//-----------------------------------------------------------------------------

InstanceNumberSorter::InstanceNumberSorter(filter::dicom::IFilter::Key key) :
    filter::dicom::sorter::TagValueSorter(key)
{
    this->setTag(DCM_InstanceNumber);
}

//-----------------------------------------------------------------------------

InstanceNumberSorter::~InstanceNumberSorter()
= default;

//-----------------------------------------------------------------------------

std::string InstanceNumberSorter::getName() const
{
    return InstanceNumberSorter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string InstanceNumberSorter::getDescription() const
{
    return InstanceNumberSorter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool InstanceNumberSorter::isConfigurationRequired() const
{
    return false;
}

} // namespace sight::filter::dicom::sorter
