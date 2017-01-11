/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCamp/UserObject.hpp>

#include <fwData/Reconstruction.hpp>

#include <fwMedData/ModelSeries.hpp>

//------------------------------------------------------------------------------

fwCampImplementDataMacro((fwMedData)(ModelSeries))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwMedData")
    .base< ::fwMedData::Series>()
    .property("reconstruction_db", &::fwMedData::ModelSeries::m_reconstructionDB)
    ;
}
