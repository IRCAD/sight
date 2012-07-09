#include <fwCamp/UserObject.hpp>
#include <fwTools/UUID.hpp>

#include "fwMetaData/Object.hpp"



fwCampImplementMacro((fwMetaData)(Object))
{
    builder.base< ::fwMetaData::Base>()
        .property("metaInfos", &::fwMetaData::Object::getMetaInfos)
        .property("attributes", &::fwMetaData::Object::getAttributes);
}

namespace fwMetaData {

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


}  // namespace fwMetaData
