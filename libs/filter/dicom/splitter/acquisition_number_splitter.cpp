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

#include "filter/dicom/splitter/acquisition_number_splitter.hpp"

#include "filter/dicom/registry/macros.hpp"

#include <dcmtk/dcmdata/dcdeftag.h>

#include <string>

SIGHT_REGISTER_DICOM_FILTER(sight::filter::dicom::splitter::acquisition_number_splitter);

namespace sight::filter::dicom::splitter
{

const std::string acquisition_number_splitter::FILTER_NAME        = "Acquisition number splitter";
const std::string acquisition_number_splitter::FILTER_DESCRIPTION =
    "Split instances according to acquisition number tag.";

//-----------------------------------------------------------------------------

acquisition_number_splitter::acquisition_number_splitter()
{
    this->set_tag(DCM_AcquisitionNumber);
}

//-----------------------------------------------------------------------------

acquisition_number_splitter::~acquisition_number_splitter()
= default;

//-----------------------------------------------------------------------------

std::string acquisition_number_splitter::get_name() const
{
    return acquisition_number_splitter::FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string acquisition_number_splitter::get_description() const
{
    return acquisition_number_splitter::FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool acquisition_number_splitter::is_configuration_required() const
{
    return false;
}

//-----------------------------------------------------------------------------

} // namespace sight::filter::dicom::splitter
