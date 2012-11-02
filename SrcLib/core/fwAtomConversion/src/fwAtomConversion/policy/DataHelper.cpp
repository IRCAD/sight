/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/AtomVisitor.hpp>

#include "fwAtomConversion/policy/DataHelper.hpp"
#include "fwAtomConversion/policy/Data.hpp"
#include "fwAtomConversion/AtomHelper.hpp"

#include <fwData/camp/mapper.hpp>
#include <fwTools/camp/mapper.hpp>


namespace fwAtomConversion
{
namespace policy
{

DataHelper::DataHelper(::fwAtomConversion::AtomHelper& helper) : m_helper(helper)
{
}

//-----------------------------------------------------------------------------

void DataHelper::processSimpleProperty(const std::string & property, camp::UserObject & userObj, const Attribut & attribut)
{
    userObj.set(property, attribut->getString());
}

//-----------------------------------------------------------------------------

void DataHelper::processArrayProperty(const std::string& property, camp::UserObject & userObj, const Attribut & attribut)
{
    ::fwAtoms::Object::sptr metaObject;
    ::fwAtoms::Sequence::sptr sequence;
    ::fwAtoms::Base::sptr element;


    sequence = ::fwAtoms::Sequence::dynamicCast(attribut);
    const ::fwAtoms::Sequence::SequenceType& values = sequence->getValue();

    const ::camp::Class& metaclass = userObj.getClass();
    const ::camp::Property& prop = metaclass.property(property);
    const ::camp::ArrayProperty& arrayProp = dynamic_cast< const ::camp::ArrayProperty&>(prop);

    unsigned int size = sequence->getValue().size();
    for(unsigned int i = 0; i < size; i++)
    {
        element = values[i];
        if(element.get() != NULL)
        {
            if(element->isValue())
            {
                if(arrayProp.dynamic())
                {
                    arrayProp.insert(userObj, i,  element->getString());
                }
                else
                {
                    arrayProp.set(userObj, i,  element->getString());
                }
            }
            else if(element->isObject())
            {
                metaObject = ::fwAtoms::Object::dynamicCast(element);
                if(arrayProp.dynamic())
                {
                    arrayProp.insert(userObj,i, m_helper.metaToData(metaObject));
                }
                else
                {
                    arrayProp.set(userObj,i, m_helper.metaToData(metaObject));
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void DataHelper::processUserProperty(const std::string & property, camp::UserObject & userObj, const Attribut & attribut)
{
    ::fwAtoms::Object::sptr metaObject;
    metaObject  = ::fwAtoms::Object::dynamicCast(attribut);
    if(metaObject.get() != NULL && !metaObject->getType().empty())
    {
        ::fwData::Object::sptr object = m_helper.metaToData(metaObject);
        userObj.set(property, object);
    }
    else if (metaObject.get() != NULL)
    {
        OSLM_FATAL("Object could not be null : " << property);
    }
    else
    {
        OSLM_FATAL("Object type could not be empty: " << property);
    }
}

//-----------------------------------------------------------------------------

void DataHelper::processMappingProperty(const std::string & property, camp::UserObject & userObj, const Attribut & attribut)
{
    ::fwAtoms::Object::sptr metaObject;

    ::fwAtoms::Map::sptr map = ::fwAtoms::Map::dynamicCast(attribut);
    ::fwAtoms::Base::sptr value;

    ::fwAtoms::Map::Iterator it = map->begin();
    ::fwAtoms::Base::sptr key;
    camp::Value campValue;

    const ::camp::Class& metaclass = userObj.getClass();
    const ::camp::Property& prop = metaclass.property(property);
    const ::camp::MapProperty& mapProp = dynamic_cast< const ::camp::MapProperty&>(prop);

    for(; it != map->end(); ++it)
    {
        key = it->first;
        value = it->second;
        if(value->isObject())
        {
            metaObject = ::fwAtoms::Object::dynamicCast(value);

            if(key->isValue())
            {
                mapProp.set(userObj,key->getString(), m_helper.metaToData(metaObject));
            }
            else
            {
                 mapProp.set( userObj, m_helper.metaToData(::fwAtoms::Object::dynamicCast(key)),
                              m_helper.metaToData(metaObject));
            }
        }
        else
        {
            mapProp.set(userObj, key->getString(), value->getString());
        }
    }
}

//-----------------------------------------------------------------------------

DataHelper::~DataHelper()
{
}

} // namespace policy
} // namespace fwAtomConversion
