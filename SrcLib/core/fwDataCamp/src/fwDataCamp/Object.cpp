/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCamp/UserObject.hpp>

#include <fwData/Object.hpp>

fwCampImplementDataMacro((fwData)(Object))
{
    builder
    .property("fields", &::fwData::Object::m_fields)
    .function("classname", (const std::string& (fwData::Object::*)() const) &::fwData::Object::getClassname)
    .function("is_a", (bool (fwData::Object::*)(const std::string&) const) &::fwData::Object::isA)
    ;
}
