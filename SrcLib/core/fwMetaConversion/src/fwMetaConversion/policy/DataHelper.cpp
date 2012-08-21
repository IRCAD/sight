#include <fwMetaData/Sequence.hpp>
#include <fwMetaData/Object.hpp>
#include <fwMetaData/Map.hpp>
#include <fwMetaData/MetaVisitor.hpp>

#include "fwMetaConversion/policy/DataHelper.hpp"
#include "fwMetaConversion/policy/Data.hpp"
#include "fwMetaConversion/MetaHelper.hpp"

#include <fwData/camp/mapper.hpp>
#include <fwTools/camp/mapper.hpp>


namespace fwMetaConversion
{
namespace policy
{

DataHelper::DataHelper(::fwMetaConversion::MetaHelper& helper) : m_helper(helper)
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


    ::fwMetaData::Object::sptr metaObject;
    ::fwMetaData::Sequence::sptr sequence;
    ::fwMetaData::Base::sptr element;


    sequence = ::fwMetaData::Sequence::dynamicCast(attribut);
    const ::fwMetaData::Sequence::SequenceType& values = sequence->getValue();

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
                metaObject = ::fwMetaData::Object::dynamicCast(element);
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
    ::fwMetaData::Object::sptr metaObject;
    metaObject  = ::fwMetaData::Object::dynamicCast(attribut);
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
    ::fwMetaData::Object::sptr metaObject;

    ::fwMetaData::Map::sptr map = ::fwMetaData::Map::dynamicCast(attribut);
    ::fwMetaData::Base::sptr value;

    ::fwMetaData::Map::Iterator it = map->begin();
    ::fwMetaData::Base::sptr key;
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
            metaObject = ::fwMetaData::Object::dynamicCast(value);

            if(key->isValue())
            {
                mapProp.set(userObj,key->getString(), m_helper.metaToData(metaObject));
            }
            else
            {
                 mapProp.set( userObj, m_helper.metaToData(::fwMetaData::Object::dynamicCast(key)),
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
} // namespace fwMetaConversion
