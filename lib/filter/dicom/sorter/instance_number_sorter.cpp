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

#include "filter/dicom/sorter/instance_number_sorter.hpp"

#include "filter/dicom/exceptions/filter_failure.hpp"
#include "filter/dicom/registry/macros.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

SIGHT_REGISTER_DICOM_FILTER(sight::filter::dicom::sorter::instance_number_sorter);

namespace sight::filter::dicom::sorter
{

const std::string instance_number_sorter::FILTER_NAME        = "Instance number sorter";
const std::string instance_number_sorter::FILTER_DESCRIPTION =
    "Sort instances using <i>InstanceNumber</i> tag.";

//-----------------------------------------------------------------------------

instance_number_sorter::instance_number_sorter()
{
    this->set_tag(DCM_InstanceNumber);
}

//-----------------------------------------------------------------------------

instance_number_sorter::~instance_number_sorter()
= default;

//-----------------------------------------------------------------------------

std::string instance_number_sorter::get_name() const
{
    return instance_number_sorter::FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string instance_number_sorter::get_description() const
{
    return instance_number_sorter::FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool instance_number_sorter::is_configuration_required() const
{
    return false;
}

} // namespace sight::filter::dicom::sorter
