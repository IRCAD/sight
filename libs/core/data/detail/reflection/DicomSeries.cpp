/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#define CAMP_COMPILATION

#include "data/detail/reflection/DicomSeries.hpp"
#include "data/DicomSeries.hpp"

#include <core/reflection/UserObject.hpp>

//------------------------------------------------------------------------------

SIGHT_IMPLEMENT_DATA_REFLECTION((sight) (data) (DicomSeries))
{
    builder
    .tag("object_version", "2")
    .tag("lib_name", "::sight::data")
    .base< ::sight::data::Series>()
    .property("number_of_instances", &::sight::data::DicomSeries::m_numberOfInstances)
    .property("dicom_container", &::sight::data::DicomSeries::m_dicomContainer)
    .property("sop_class_uids", &::sight::data::DicomSeries::m_SOPClassUIDs)
    .property("computed_tag_values", &::sight::data::DicomSeries::m_computedTagValues)
    .property("first_instance_number", &::sight::data::DicomSeries::m_firstInstanceNumber)
    ;
}
