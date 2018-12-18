/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwMedDataCamp/DicomSeries.hpp"

#include <fwCamp/UserObject.hpp>

#include <fwMedData/DicomSeries.hpp>

//------------------------------------------------------------------------------

fwCampImplementDataMacro((fwMedData)(DicomSeries))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwMedData")
    .base< ::fwMedData::Series>()
    .property("number_of_instances", &::fwMedData::DicomSeries::m_numberOfInstances)
    .property("dicom_container", &::fwMedData::DicomSeries::m_dicomContainer)
    .property("sop_class_uids", &::fwMedData::DicomSeries::m_SOPClassUIDs)
    .property("computed_tag_values", &::fwMedData::DicomSeries::m_computedTagValues)
    .property("first_instance_number", &::fwMedData::DicomSeries::m_firstInstanceNumber)
    ;
}
