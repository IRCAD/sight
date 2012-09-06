/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/String.hpp>

#include "fwDataCamp/Factory.hpp"

fwCampImplementDataMacro((fwData)(String))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object >()
        .property("value", (std::string fwData::String::*) &fwData::String::m_value)
        ;
}
