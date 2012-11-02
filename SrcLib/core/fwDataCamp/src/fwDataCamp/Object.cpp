/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Object.hpp>

#include <fwCamp/UserObject.hpp>

fwCampImplementDataMacro((fwData)(Object))
{
    builder
        .property("fields", &::fwData::Object::m_fields)
        .function("classname", (std::string (fwData::Object::*)() const) &::fwData::Object::className)
        ;
}
