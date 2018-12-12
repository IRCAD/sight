/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"
#include "fwData/Plane.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

fwDataRegisterMacro( ::fwData::Plane );

namespace fwData
{

const ::fwCom::Signals::SignalKeyType Plane::s_SELECTED_SIG = "selected";

//------------------------------------------------------------------------------

Plane::Plane (::fwData::Object::Key key) : m_isIntersection(true)
{
    m_vPoints[0] = ::fwData::Point::New();
    m_vPoints[1] = ::fwData::Point::New();
    m_vPoints[2] = ::fwData::Point::New();

    newSignal< SelectedSignalType >(s_SELECTED_SIG);
}

//------------------------------------------------------------------------------

Plane::~Plane ()
{
}

//------------------------------------------------------------------------------

void Plane::shallowCopy(const Object::csptr &_source )
{
    Plane::csptr other = Plane::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
    m_vPoints = other->m_vPoints;
}

//------------------------------------------------------------------------------

void Plane::cachedDeepCopy(const Object::csptr &_source, DeepCopyCacheType &cache)
{
    Plane::csptr other = Plane::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );
    m_vPoints[0] = ::fwData::Object::copy(other->m_vPoints[0], cache);
    m_vPoints[1] = ::fwData::Object::copy(other->m_vPoints[1], cache);
    m_vPoints[2] = ::fwData::Object::copy(other->m_vPoints[2], cache);
}

//------------------------------------------------------------------------------

void Plane::setValue(::fwData::Point::sptr _point1, ::fwData::Point::sptr _point2, ::fwData::Point::sptr _point3)
{
    m_vPoints[0] = _point1;
    m_vPoints[1] = _point2;
    m_vPoints[2] = _point3;
}

//------------------------------------------------------------------------------

} // namespace fwData


