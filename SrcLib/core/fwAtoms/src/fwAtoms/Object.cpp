/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCamp/UserObject.hpp>
#include <fwTools/UUID.hpp>

#include "fwAtoms/Object.hpp"



fwCampImplementMacro((fwAtoms)(Object))
{
    builder.base< ::fwAtoms::Base>()
        .property("metaInfos", &::fwAtoms::Object::getMetaInfos)
        .property("attributes", &::fwAtoms::Object::getAttributes);
}

namespace fwAtoms {

Object::Object()
{
    m_id = ::fwTools::UUID::generateUUID();
}

void Object::addAttribut(const std::string& key, Base::sptr value)
{
    m_attrs[key] = value;
}

void Object::setAttributs( Object::Attributes& attrs)
{
    m_attrs.clear();
    m_attrs.insert(attrs.begin(), attrs.end());
}

void Object::setType(const std::string& type)
{
    m_metaInfos["type"] = type;
}

std::string Object::getType()
{
    return this->getMetaInfo("type");
}

//------------------------------------------------------------------------------

Base::sptr Object::clone()
{
    return this->getSptr();
}


void Object::addMetaInfo(const std::string& key, const std::string& value)
{
    if(!value.empty())
    {
        m_metaInfos[key] = value;
    }

}
std::string Object::getMetaInfo(const std::string& key)
{
    MetaInfos::const_iterator cIt = m_metaInfos.find(key);
    std::string value;

    if(cIt == m_metaInfos.end())
    {
        value = "";
    }
    else
    {
        value = cIt->second;
    }

    return value;
}

Object::MetaInfos& Object::getMetaInfos()
{
   return m_metaInfos;
}

void Object::setMetaInfos(const MetaInfos& metaInfos)
{
    m_metaInfos.clear();
    m_metaInfos.insert(metaInfos.begin(), metaInfos.end());
}

void Object::removeAttribut(const std::string& key)
{
    m_attrs.erase(key);
}


}  // namespace fwAtoms
