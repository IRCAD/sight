/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCamp/UserObject.hpp>
#include "fwDataCamp/ResectionDB.hpp"

fwCampImplementDataMacro((fwData)(ResectionDB))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwData")
    .base< ::fwData::Object>()
    .property("safe_resection", &::fwData::ResectionDB::m_safeResection)
    .property("resections", &::fwData::ResectionDB::m_resections)
    ;
}
