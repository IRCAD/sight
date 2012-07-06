/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/HiResClock.hpp>

#include "fwTools/ClassRegistrar.hpp"
#include "fwTools/Object.hpp"
#include "fwTools/UUID.hpp"
#include "fwTools/Factory.hpp"

namespace fwTools
{

//------------------------------------------------------------------------------

Object::Object() :
    m_timeStamp ( ::fwCore::TimeStamp::New()  ),
    m_logicStamp( ::fwCore::LogicStamp::New() ),
    m_uuid( ::fwTools::UUID::New() )
{
    m_OSRKey = ::fwCore::LogicStamp::New();
    m_OSRKey->modified();
}

//------------------------------------------------------------------------------

Object::~Object()
{
    ::fwTools::Factory::uninitData(m_OSRKey);
}

//------------------------------------------------------------------------------

Object &Object::operator=(const Object &_obj)
{
    SLM_FATAL("This operator is forbidden, use shallowCopy or deepCopy instead.");
    m_timeStamp =_obj.m_timeStamp;
    m_logicStamp =_obj.m_logicStamp;
    return (*this);
}

//------------------------------------------------------------------------------

std::string Object::className() const
{
    return this->getClassname();
}

//-----------------------------------------------------------------------------

} //namespace fwTools



