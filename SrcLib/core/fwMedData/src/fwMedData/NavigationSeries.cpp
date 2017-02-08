/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMedData/NavigationSeries.hpp"

#include <fwData/Exception.hpp>
#include <fwData/registry/macros.hpp>

fwDataRegisterMacro( ::fwMedData::NavigationSeries );

namespace fwMedData
{

NavigationSeries::NavigationSeries(::fwData::Object::Key key) :
    Series(key)
{
}

//------------------------------------------------------------------------------

NavigationSeries::~NavigationSeries()
{
}

//------------------------------------------------------------------------------

void NavigationSeries::shallowCopy(const ::fwData::Object::csptr& _source)
{
    NavigationSeries::csptr other = NavigationSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->::fwMedData::Series::shallowCopy(_source);

    m_position = other->m_position;
    m_move     = other->m_move;
    m_lookAt   = other->m_lookAt;
}

//------------------------------------------------------------------------------

void NavigationSeries::cachedDeepCopy(const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache)
{
    NavigationSeries::csptr other = NavigationSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->::fwMedData::Series::cachedDeepCopy(_source, cache);

    m_position = other->m_position;
    m_move     = other->m_move;
    m_lookAt   = other->m_lookAt;
}

//------------------------------------------------------------------------------

void NavigationSeries::addPosition(TimestampType time, const CoordinateType& pos)
{
    m_position[time] = pos;
}

//------------------------------------------------------------------------------

void NavigationSeries::addMove(TimestampType time, const CoordinateType& dir)
{
    m_move[time] = dir;
}

//------------------------------------------------------------------------------

void NavigationSeries::addLookAt(TimestampType time, const CoordinateType& point)
{
    m_lookAt[time] = point;
}

//------------------------------------------------------------------------------

void NavigationSeries::erasePosition(TimestampType time)
{
    m_position.erase(time);
}

//------------------------------------------------------------------------------

void NavigationSeries::eraseMove(TimestampType time)
{
    m_move.erase(time);
}

//------------------------------------------------------------------------------

void NavigationSeries::eraseLookAt(TimestampType time)
{
    m_lookAt.erase(time);
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

} // namespace fwMedData

