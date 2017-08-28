/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwTools/Object.hpp"

#include "fwTools/UUID.hpp"

#include <fwCore/HiResClock.hpp>

namespace fwTools
{

//------------------------------------------------------------------------------

Object::Object() :
    m_timeStamp( ::fwCore::TimeStamp::New()  ),
    m_uuid( ::fwTools::UUID::New() )
{
}

//------------------------------------------------------------------------------

Object::~Object()
{
}

//------------------------------------------------------------------------------

Object& Object::operator=(const Object& _obj)
{
    SLM_FATAL("This operator is forbidden, use shallowCopy or deepCopy instead.");
    m_timeStamp = _obj.m_timeStamp;
    return (*this);
}

//------------------------------------------------------------------------------

std::string Object::className() const
{
    return this->getClassname();
}

//-----------------------------------------------------------------------------

} //namespace fwTools

