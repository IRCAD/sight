/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwDataCamp/Version.hpp>

#include <fwAtoms/Object.hpp>
#include <fwAtoms/AtomVisitor.hpp>

#include "fwAtomConversion/policy/Data.hpp"
#include "fwAtomConversion/DataVisitor.hpp"
#include "fwAtomConversion/AtomHelper.hpp"
#include "fwAtomConversion/custom/factory/new.hpp"
#include "fwAtomConversion/custom/Mapper.hpp"


namespace fwAtomConversion
{


fwAtoms::Object::sptr AtomHelper::fromFwData(fwData::Object::sptr object)
{
    fwAtoms::Object::sptr metaObject = this->dataToMeta(object);
    this->clearMetaCache();
    this->clearCache();
    return metaObject;
}

//-----------------------------------------------------------------------------

fwAtoms::Object::sptr AtomHelper::dataToMeta(fwData::Object::sptr object)
{
    const camp::Class& metaclass = camp::classByName(object->getClassname());
    fwAtoms::Object::sptr metaObj;

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
        SPTR(custom::Mapper) mapper = custom::factory::New(object->getClassname());
        if(mapper)
        {
            metaObj = mapper->toMeta(object, *this);
        }
        else
        {
            ::fwAtomConversion::DataVisitor visitor(object, *this);
            metaclass.visit(visitor);
            metaObj = visitor.getAtomObject();
        }
        metaObj->setId(id);
        m_metaCache[id] =  metaObj;
    }
    return metaObj;
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr AtomHelper::toFwData(fwAtoms::Object::sptr object)
{
    ::fwData::Object::sptr dataObject = this->metaToData(object);
    this->clearMetaCache();
    this->clearCache();
    return dataObject;
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr AtomHelper::metaToData(fwAtoms::Object::sptr object)
{
    ::fwData::Object::sptr dataObject;
    std::string metaId = object->getId();
    Cache::const_iterator it = m_cache.find(metaId);

    if(it == m_cache.end())
    {
        SPTR(custom::Mapper) mapper = custom::factory::New(object->getType());
        if(mapper)
        {
            dataObject = mapper->fromMeta(object, *this);
        }
        else
        {
            const camp::Class& metaclass = camp::classByName(object->getClassname());
            ::fwAtomConversion::policy::Data policy(*this);
            ::fwAtoms::AtomVisitor visitor(policy, object);
            metaclass.visit(visitor);
            dataObject = policy.getObject();
        }
        OSLM_ASSERT("Error when converting metaData to fwData", (dataObject.get()) != NULL);
        this->m_cache[metaId] = dataObject;
    }
    else
    {
        dataObject = it->second;
    }
    return dataObject;
}

//-----------------------------------------------------------------------------

AtomHelper::AtomHelper()
{
    m_nbProcessObject = 0;
    FW_RAISE_IF("fwDataCamp version not correspond.", ::fwDataCamp::Version::s_CURRENT_VERSION != 1 );
}

//-----------------------------------------------------------------------------

AtomHelper::~AtomHelper()
{}

//-----------------------------------------------------------------------------

void AtomHelper::clearCache()
{
    this->m_cache.clear();
}

//-----------------------------------------------------------------------------

void AtomHelper::clearMetaCache()
{
    this->m_metaCache.clear();
}

//-----------------------------------------------------------------------------

unsigned int AtomHelper::getNbProcessObject()
{
    return m_nbProcessObject;
}

} // namespace fwAtomConversion
