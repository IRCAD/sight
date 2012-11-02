/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/shared_ptr.hpp>
#include <boost/utility/enable_if.hpp>


#include <fwCamp/factory/new.hpp>
#include <fwCamp/Mapper/ValueMapper.hpp>

#include <fwData/camp/mapper.hpp>
#include <fwData/Array.hpp>

#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/Blob.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/String.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Base.hpp>

#include <fwTools/UUID.hpp>
#include <fwTools/BufferObject.hpp>

#include "fwAtomConversion/DataVisitor.hpp"

#include "fwAtomConversion/camp/ValueMapper.hpp"


namespace fwAtomConversion
{

//-----------------------------------------------------------------------------

DataVisitor::DataVisitor(::fwData::Object::sptr data, ::fwAtomConversion::AtomHelper& helper) : m_helper(helper)
{
    std::string type = data->getClassname();
    const camp::Class& metaclass = camp::classByName(type);

    m_obj = ::fwCamp::factory::New(type, data.get());

    m_metaObject = ::fwAtoms::Object::New();
    m_metaObject->setType(type);


    std::size_t tagCount = metaclass.tagCount();
    if( tagCount > 0)
    {
        std::string key;
        std::string value;
        for (std::size_t i = 0; i < tagCount; i++)
        {
             key = metaclass.tagId(i).to< std::string>();
             value = metaclass.tag(key).to< std::string>();
             m_metaObject->addMetaInfo(key, value);
        }
    }
}

//-----------------------------------------------------------------------------

DataVisitor::~DataVisitor()
{}

//-----------------------------------------------------------------------------

void DataVisitor::visit(const camp::SimpleProperty& property)
{
    SLM_TRACE_FUNC();

    const std::string& name(property.name());
    const ::camp::Value& value(m_obj->get(name));
    ::fwAtoms::Base::sptr base = this->matchCampType(property.type(), value);
    m_metaObject->addAttribut(name,base);
}

//-----------------------------------------------------------------------------

void DataVisitor::visit(const camp::EnumProperty& property)
{
    SLM_TRACE_FUNC();
    const std::string& name(property.name());
    const camp::Enum& metaEnum = property.getEnum();
    camp::Value enumProperty= m_obj->get(name);
    camp::Value stringValue = metaEnum.name(enumProperty.to<int>());

    ::fwAtoms::Base::sptr base = this->matchCampType(camp::stringType,stringValue);
    m_metaObject->addAttribut(name,base);
}

//-----------------------------------------------------------------------------

void DataVisitor::visit(const camp::MapProperty& property)
{
    ::fwAtoms::Map::sptr map = ::fwAtoms::Map::New();

    std::pair< camp::Value, camp::Value > value;
    camp::Value first;
    camp::Value second;

    ::fwAtoms::Base::sptr currentMetaKey;
    ::fwAtoms::Base::sptr currentMetaValue;

    for (std::size_t var = 0; var < property.getSize(*m_obj); ++var)
    {
        value = property.getElement(*m_obj, var);
        first = value.first;
        second = value.second;

        currentMetaKey = this->matchCampType(first.type(), first);
        currentMetaValue = this->matchCampType(second.type(), second);

        map->insert(currentMetaKey, currentMetaValue);
    }
    m_metaObject->addAttribut(property.name(),map);
}

//-----------------------------------------------------------------------------

void DataVisitor::visit(const camp::ArrayProperty& property)
{
    SLM_TRACE_FUNC();

    unsigned int size = property.size(*m_obj);

    ::fwAtoms::Sequence::sptr sequence = ::fwAtoms::Sequence::New();

    camp::Value elemValue;

    for (unsigned int i = 0; i < size; ++i)
    {
        elemValue = property.get(*m_obj,i);
        sequence->append(this->matchCampType(elemValue.type(), elemValue));
    }
    m_metaObject->addAttribut(property.name(), sequence);
}

//-----------------------------------------------------------------------------

void DataVisitor::visit(const camp::UserProperty& property)
{
    SLM_TRACE_FUNC();
    const ::camp::Value& value(property.get(*m_obj));

    ::fwAtoms::Base::sptr baseObj;

    baseObj = this->processObject(value);
    if(baseObj.get() != NULL)
    {
        m_metaObject->addAttribut(property.name(), baseObj);
    }
}

//-----------------------------------------------------------------------------

void DataVisitor::visit(const camp::Function& function)
{
    SLM_WARN("function visitation not implemented");
}

//-----------------------------------------------------------------------------

::fwAtoms::Base::sptr DataVisitor::processObject(const ::camp::Value& value)
{
    ::fwAtoms::Base::sptr baseObj;

    ::fwData::Object::sptr object = value.to< ::fwData::Object::sptr>();

    if(object.get()  != NULL)
    {
        baseObj = m_helper.dataToMeta(object);
    }
    else
    {
        const camp::UserObject campObject = value.to< ::camp::UserObject>();
        const ::camp::Class& metaclass = campObject.getClass();

        if(metaclass.hasTag("buffer"))
        {
            baseObj = value.to< ::fwAtoms::Blob::sptr>();
        }
    }
    return baseObj;
}

//-----------------------------------------------------------------------------

::fwAtoms::Base::sptr DataVisitor::matchCampType(camp::Type type,
        const camp::Value& value)
{
    ::fwAtoms::Base::sptr base;
    switch(type)
    {
    case camp::enumType:
    case camp::stringType:
        base = ::fwAtoms::String::New(value.to<std::string>());
        break;
    case camp::intType :
        base = ::fwAtoms::Numeric::New(value.to< std::string>());
        break;
    case camp::realType :
        base = ::fwAtoms::Numeric::New(value.to< std::string>());
        break;
    case camp::boolType :
        base = ::fwAtoms::Boolean::New(value.to< bool>());
        break;
    case camp::userType:
        base = this->processObject(value);
        break;
    default:
        OSLM_ERROR("the type is unknown; type id : " << type);
        break;
    }

    return base;
}

//-----------------------------------------------------------------------------

::fwAtoms::Object::sptr DataVisitor::getAtomObject()
{
    return this->m_metaObject;
}

} // fwAtomConversion
