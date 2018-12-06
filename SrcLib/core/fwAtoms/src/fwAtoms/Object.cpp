/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "fwAtoms/Object.hpp"

#include "fwAtoms/registry/macros.hpp"

#include <fwCamp/UserObject.hpp>

fwAtomsRegisterMacro( ::fwAtoms::Object );

namespace fwAtoms
{

//------------------------------------------------------------------------------

void Object::setAttribute(const std::string& key, const Base::sptr& value)
{
    m_attributes[key] = value;
}

//------------------------------------------------------------------------------

Base::sptr Object::getAttribute(const std::string& key) const
{
    AttributesType::const_iterator iterAttr = m_attributes.find(key);
    if (iterAttr != m_attributes.end())
    {
        return iterAttr->second;
    }
    return Base::sptr();
}

//------------------------------------------------------------------------------

void Object::setAttributes(const Object::AttributesType& attrs)
{
    m_attributes = attrs;
}

//------------------------------------------------------------------------------

Base::sptr Object::clone() const
{
    Object::sptr obj = Object::New();
    obj->m_metaInfos = m_metaInfos;

    for(const AttributesType::value_type& elem :  m_attributes)
    {
        if ( elem.second )
        {
            obj->m_attributes.insert( AttributesType::value_type(elem.first, elem.second->clone() ) );
        }
        else
        {
            Base::sptr nullData;
            obj->m_attributes.insert( AttributesType::value_type(elem.first, nullData) );
        }
    }

    return obj;
}

//------------------------------------------------------------------------------

void Object::setMetaInfo(const std::string& key, const std::string& value)
{
    m_metaInfos[key] = value;
}

//------------------------------------------------------------------------------

std::string Object::getMetaInfo(const std::string& key) const
{
    MetaInfosType::const_iterator iterMetaInfos = m_metaInfos.find(key);
    if(iterMetaInfos != m_metaInfos.end())
    {
        return iterMetaInfos->second;
    }
    return "";
}

//------------------------------------------------------------------------------

void Object::setMetaInfos(const MetaInfosType& metaInfos)
{
    m_metaInfos = metaInfos;
}

//------------------------------------------------------------------------------

Object::AttributesType::size_type Object::eraseAttribute(const std::string& key)
{
    return m_attributes.erase(key);
}

//------------------------------------------------------------------------------

void Object::clearAttribute()
{
    m_attributes.clear();
}

//------------------------------------------------------------------------------

Object::MetaInfosType::size_type Object::eraseMetaInfo(const std::string& key)
{
    return m_metaInfos.erase(key);
}

//------------------------------------------------------------------------------

void Object::clearMetaInfo()
{
    m_metaInfos.clear();
}

}  // namespace fwAtoms

