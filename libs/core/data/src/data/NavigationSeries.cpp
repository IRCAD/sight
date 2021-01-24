/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/NavigationSeries.hpp"

#include <data/Exception.hpp>
#include <data/registry/macros.hpp>

fwDataRegisterMacro( sight::data::NavigationSeries )

namespace sight::data
{

NavigationSeries::NavigationSeries(data::Object::Key _key) :
    Series(_key)
{
}

//------------------------------------------------------------------------------

NavigationSeries::~NavigationSeries()
{
}

//------------------------------------------------------------------------------

void NavigationSeries::shallowCopy(const data::Object::csptr& _source)
{
    NavigationSeries::csptr other = NavigationSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( data::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->data::Series::shallowCopy(_source);

    m_position = other->m_position;
    m_move     = other->m_move;
    m_lookAt   = other->m_lookAt;
}

//------------------------------------------------------------------------------

void NavigationSeries::cachedDeepCopy(const data::Object::csptr& _source, DeepCopyCacheType& cache)
{
    NavigationSeries::csptr other = NavigationSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( data::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->data::Series::cachedDeepCopy(_source, cache);

    m_position = other->m_position;
    m_move     = other->m_move;
    m_lookAt   = other->m_lookAt;
}

//------------------------------------------------------------------------------

void NavigationSeries::addPosition(TimestampType _time, const CoordinateType& _pos)
{
    m_position[_time] = _pos;
}

//------------------------------------------------------------------------------

void NavigationSeries::addMove(TimestampType _time, const CoordinateType& _dir)
{
    m_move[_time] = _dir;
}

//------------------------------------------------------------------------------

void NavigationSeries::addLookAt(TimestampType _time, const CoordinateType& _point)
{
    m_lookAt[_time] = _point;
}

//------------------------------------------------------------------------------

void NavigationSeries::erasePosition(TimestampType _time)
{
    m_position.erase(_time);
}

//------------------------------------------------------------------------------

void NavigationSeries::eraseMove(TimestampType _time)
{
    m_move.erase(_time);
}

//------------------------------------------------------------------------------

void NavigationSeries::eraseLookAt(TimestampType _time)
{
    m_lookAt.erase(_time);
}

//------------------------------------------------------------------------------

void NavigationSeries::clearPosition()
{
    m_position.clear();
}

//------------------------------------------------------------------------------

void NavigationSeries::clearMove()
{
    m_move.clear();
}

//------------------------------------------------------------------------------

void NavigationSeries::clearLookAt()
{
    m_lookAt.clear();
}

//------------------------------------------------------------------------------

const NavigationSeries::ContainerType& NavigationSeries::getPositionMap() const
{
    return m_position;
}

//------------------------------------------------------------------------------

const NavigationSeries::ContainerType& NavigationSeries::getMoveMap() const
{
    return m_move;
}

//------------------------------------------------------------------------------

const NavigationSeries::ContainerType& NavigationSeries::getLookAtMap() const
{
    return m_lookAt;
}

} // namespace sight::data
