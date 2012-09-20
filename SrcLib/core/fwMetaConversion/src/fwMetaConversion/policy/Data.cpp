/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/BufferObject.hpp>

#include <fwMetaData/MetaVisitor.hpp>
#include <fwMetaData/Object.hpp>
#include <fwMetaData/Sequence.hpp>
#include <fwMetaData/Blob.hpp>

#include <fwCamp/factory/new.hpp>

#include "fwMetaConversion/policy/Data.hpp"
#include "fwMetaConversion/policy/DataHelper.hpp"
#include "fwMetaConversion/camp/ValueMapper.hpp"

namespace fwMetaConversion
{
namespace policy
{

//-----------------------------------------------------------------------------

Data::Data(::fwMetaConversion::MetaHelper& helper) : m_helper(helper)
{
}

//-----------------------------------------------------------------------------

void Data::processAttributes(const Attributes & attributes)
{
    Attributes::const_iterator cIt = attributes.begin();
    std::string name;
    ::fwMetaData::Base::sptr attribut;
    const camp::Class & metaclass = camp::classByName(m_object->getClassname());
    for(;cIt != attributes.end(); ++cIt)
    {
        name = cIt->first;
        attribut = cIt->second;

        if(metaclass.hasProperty(name))
        {
            this->processAttribut(name, attribut);
        }
        else
        {
            OSLM_WARN("the property with name :" << name << " is not supported");
        }
    }
}

//-----------------------------------------------------------------------------

void Data::processMetaInfos(const ::fwMetaData::Object::MetaInfos& metaInfos)
{
    std::string type = metaInfos.find("type")->second;
    ::fwData::Object::sptr obj = fwData::factory::New(type);
    m_object = obj;
}

//-----------------------------------------------------------------------------

void Data::processAttribut(const std::string & name, const Attributes::mapped_type & attribut)
{
    const camp::Class & metaclass = camp::classByName(m_object->getClassname());
    const ::camp::Property& p = metaclass.property(name);

    std::string classname = m_object->getClassname();

    OSLM_TRACE("classname : " << classname);
    SPTR(camp::UserObject) userObject = ::fwCamp::factory::New(classname, m_object.get());

    int type= p.type();
    switch(type)
    {
        case(::camp::stringType):
        case(::camp::boolType):
        case(::camp::realType):
        case(::camp::intType):
        case(::camp::enumType):
            m_helper.processSimpleProperty(name, *userObject, attribut);
        break;
        case(::camp::arrayType):
        m_helper.processArrayProperty(name, *userObject, attribut);
        break;
        case(::camp::mappingType):
        m_helper.processMappingProperty(name, *userObject, attribut);
        break;
        case(::camp::userType):
            if(attribut->isObject())
            {
                m_helper.processUserProperty(name, *userObject, attribut);
            }
            else if(attribut->isBlob())
            {
                ::fwMetaData::Blob::sptr blob = fwMetaData::Blob::dynamicCast(attribut);
                ::fwTools::BufferObject::sptr buffer;
                buffer = ::camp_ext::ValueMapper< ::fwMetaData::Blob::sptr>::to(blob);
                p.set(m_object, ::camp::UserObject(buffer));
            }
        break;
    }
}

//-----------------------------------------------------------------------------

}  // namespace policy
}  // namespace fwMetaConversion
