/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCamp/UserObject.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>

//------------------------------------------------------------------------------

fwCampImplementDataMacro((fwMedData)(SeriesDB))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwMedData")
    .base< ::fwData::Object>()
    .property("values", &::fwMedData::SeriesDB::m_container)
    ;
}
