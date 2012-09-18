/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwDataCamp/Version.hpp>

#include <fwMetaData/Object.hpp>
#include <fwMetaData/MetaVisitor.hpp>

#include "fwMetaConversion/policy/Data.hpp"
#include "fwMetaConversion/DataVisitor.hpp"
#include "fwMetaConversion/MetaHelper.hpp"
#include "fwMetaConversion/custom/Factory.hpp"
#include "fwMetaConversion/custom/Mapper.hpp"


namespace fwMetaConversion
{


fwMetaData::Object::sptr MetaHelper::fromFwData(fwData::Object::sptr object)
{
    fwMetaData::Object::sptr metaObject = this->dataToMeta(object);
    this->clearMetaCache();
    this->clearCache();
    return metaObject;
}

//-----------------------------------------------------------------------------

fwMetaData::Object::sptr MetaHelper::dataToMeta(fwData::Object::sptr object)
{

    const camp::Class& metaclass = camp::classByName(object->getClassname());
    fwMetaData::Object::sptr metaObj;

    ::fwTools::UUID::UUIDType id = ::fwTools::UUID::get(object);
    MetaCache::const_iterator cIt = m_metaCache.find(id);


    //Check if the object is a metaObject represented the fwData already exist.
    if( cIt != m_metaCache.end())
    {

        metaObj =  cIt->second;
    }
    else
    {
        m_nbProcessObject++;
        if(custom::ObjectFactory::get(metaclass.name()))
        {
            custom::Mapper* m = custom::ObjectFactory::get(object->getClassname());
            metaObj = m->toMeta(object, *this);
        }
        else
        {
            ::fwMetaConversion::DataVisitor visitor(object, *this);
            metaclass.visit(visitor);
            metaObj = visitor.getMetaObject();
        }
        metaObj->setId(id);
        m_metaCache[id] =  metaObj;
    }
    return metaObj;
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr MetaHelper::toFwData(fwMetaData::Object::sptr object)
{
    ::fwData::Object::sptr dataObject = this->metaToData(object);
    this->clearMetaCache();
    this->clearCache();
    return dataObject;
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr MetaHelper::metaToData(fwMetaData::Object::sptr object)
{
    ::fwData::Object::sptr dataObject;
    std::string metaId = object->getId();
    Cache::const_iterator it = this->m_cache.find(metaId);


    if(it == this->m_cache.end())
    {
        const camp::Class& metaclass = camp::classByName(object->getClassname());

        if(custom::ObjectFactory::get(object->getType()))
        {
            custom::Mapper* m = custom::ObjectFactory::get(object->getType());
            dataObject = m->fromMeta(object, *this);
        }
        else
        {
            ::fwMetaConversion::policy::Data policy(*this);
            ::fwMetaData::MetaVisitor visitor(policy, object);
            metaclass.visit(visitor);
            dataObject = policy.getObject();
        }
        OSLM_ASSERT("Error when converting metaData to fwData",
                        (dataObject.get()) != NULL);
        this->m_cache[metaId] = dataObject;
    }
    else
    {
        dataObject = it->second;
    }
    return dataObject;
}

//-----------------------------------------------------------------------------

MetaHelper::MetaHelper()
{
    m_nbProcessObject = 0;
    FW_RAISE_IF("fwDataCamp version not correspond.", ::fwDataCamp::Version::s_CURRENT_VERSION != 1 );
}

//-----------------------------------------------------------------------------

MetaHelper::~MetaHelper()
{}

//-----------------------------------------------------------------------------

void MetaHelper::clearCache()
{
    this->m_cache.clear();
}

//-----------------------------------------------------------------------------

void MetaHelper::clearMetaCache()
{
    this->m_metaCache.clear();
}

//-----------------------------------------------------------------------------

unsigned int MetaHelper::getNbProcessObject()
{
    return m_nbProcessObject;
}

} // namespace fwMetaConversion
