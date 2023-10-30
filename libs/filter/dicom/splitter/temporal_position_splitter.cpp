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

#include "filter/dicom/splitter/temporal_position_splitter.hpp"

#include "filter/dicom/registry/macros.hpp"

#include <dcmtk/dcmdata/dcdeftag.h>

SIGHT_REGISTER_DICOM_FILTER(sight::filter::dicom::splitter::temporal_position_splitter);

namespace sight::filter::dicom::splitter
{

const std::string temporal_position_splitter::FILTER_NAME        = "Temporal position splitter";
const std::string temporal_position_splitter::FILTER_DESCRIPTION =
    "Split instances according to temporal position identifier tag.";

//-----------------------------------------------------------------------------

temporal_position_splitter::temporal_position_splitter()
{
    this->set_tag(DCM_TemporalPositionIdentifier);
}

//-----------------------------------------------------------------------------

temporal_position_splitter::~temporal_position_splitter()
= default;

//-----------------------------------------------------------------------------

std::string temporal_position_splitter::get_name() const
{
    return temporal_position_splitter::FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string temporal_position_splitter::get_description() const
{
    return temporal_position_splitter::FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool temporal_position_splitter::is_configuration_required() const
{
    return false;
}

//-----------------------------------------------------------------------------

} // namespace sight::filter::dicom::splitter
