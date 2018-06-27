/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arData/MarkerMap.hpp"

#include <fwData/Exception.hpp>
#include <fwData/registry/macros.hpp>

namespace arData
{

fwDataRegisterMacro( ::arData::MarkerMap )

//------------------------------------------------------------------------------

MarkerMap::MarkerMap(fwData::Object::Key)
{

}

//------------------------------------------------------------------------------

MarkerMap::~MarkerMap()
{

}

//------------------------------------------------------------------------------

void MarkerMap::shallowCopy(const fwData::Object::csptr& _source)
{
    MarkerMap::csptr other = MarkerMap::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );

    m_markers = other->m_markers;

}

//------------------------------------------------------------------------------

void MarkerMap::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& _cache)
{
    MarkerMap::csptr other = MarkerMap::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, _cache );

    m_markers = other->m_markers;
}

//------------------------------------------------------------------------------

const MarkerMap::MarkerType* MarkerMap::getMarker(const KeyType& _id) const
{
    auto it = m_markers.find(_id);
    if(it != m_markers.end())
    {
        return &it->second;
    }

    return nullptr;
}

//------------------------------------------------------------------------------

MarkerMap::MarkerType* MarkerMap::getMarker(const KeyType& _id)
{
    auto it = m_markers.find(_id);
    if(it != m_markers.end())
    {
        return &it->second;
    }

    return nullptr;
}

//------------------------------------------------------------------------------

const MarkerMap::MarkerType& MarkerMap::getMarker(size_t index) const
{
    SLM_ASSERT("Marker map is empty", !m_markers.empty());
    SLM_ASSERT("Index is bigger than map's size", index < m_markers.size() );

    auto it = m_markers.begin();
    std::advance(it, index);
    return it->second;
}

//------------------------------------------------------------------------------

MarkerMap::MarkerType& MarkerMap::getMarker(size_t index)
{
    SLM_ASSERT("Marker map is empty", !m_markers.empty());
    SLM_ASSERT("Index is bigger than map's size", index < m_markers.size() );

    auto it = m_markers.begin();
    std::advance(it, index);
    return it->second;
}

//------------------------------------------------------------------------------

size_t MarkerMap::count() const
{
    return m_markers.size();
}

//------------------------------------------------------------------------------

void MarkerMap::setMarker(const KeyType& _id, const MarkerMap::MarkerType& _marker )
{
    m_markers[_id] = _marker;
}

//------------------------------------------------------------------------------

} // namespace arData
