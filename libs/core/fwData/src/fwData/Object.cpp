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

//FIXME :This needs to be include first for GCC
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
//
#include "fwData/factory/new.hpp"
#include "fwData/Object.hpp"

#include <functional>

namespace fwData
{

//------------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType Object::s_MODIFIED_SIG       = "modified";
const ::fwCom::Signals::SignalKeyType Object::s_ADDED_FIELDS_SIG   = "addedFields";
const ::fwCom::Signals::SignalKeyType Object::s_CHANGED_FIELDS_SIG = "changedFields";
const ::fwCom::Signals::SignalKeyType Object::s_REMOVED_FIELDS_SIG = "removedFields";

//------------------------------------------------------------------------------

Object::Object()
{
    newSignal< ModifiedSignalType >(s_MODIFIED_SIG);
    newSignal< AddedFieldsSignalType >(s_ADDED_FIELDS_SIG);
    newSignal< ChangedFieldsSignalType >(s_CHANGED_FIELDS_SIG);
    newSignal< RemovedFieldsSignalType >(s_REMOVED_FIELDS_SIG);

}

//------------------------------------------------------------------------------

Object::~Object()
{
}

//------------------------------------------------------------------------------

::fwData::Object::sptr Object::getField( const FieldNameType& name, ::fwData::Object::sptr defaultValue ) const
{
    ::fwData::Object::sptr object = defaultValue;
    FieldMapType::const_iterator iter = m_fields.find(name);
    if(iter != m_fields.end())
    {
        object = iter->second;
    }
    return object;
}

//------------------------------------------------------------------------------

::fwData::Object::csptr Object::getConstField( const FieldNameType& name ) const
{
    return this->getField(name);
}

//------------------------------------------------------------------------------

const Object::FieldMapType& Object::getFields() const
{
    return m_fields;
}

//------------------------------------------------------------------------------

Object::FieldNameVectorType Object::getFieldNames() const
{
    FieldNameVectorType names;
    std::transform( m_fields.begin(), m_fields.end(),
                    std::back_inserter(names),
                    std::bind(&FieldMapType::value_type::first, std::placeholders::_1) );
    return names;
}

//------------------------------------------------------------------------------

void Object::setField( const FieldNameType& name, ::fwData::Object::sptr obj)
{
    std::pair<FieldMapType::iterator, bool> res = m_fields.insert(FieldMapType::value_type(name, obj));
    if( !res.second )
    {
        res.first->second = obj;
    }
}

//------------------------------------------------------------------------------

void Object::setFields( const FieldMapType& fieldMap )
{
    m_fields = fieldMap;
}

//------------------------------------------------------------------------------

void Object::removeField( const FieldNameType& name )
{
    FieldMapType::const_iterator iter = m_fields.find(name);
    OSLM_ASSERT("Field "<<name<<" not found.", iter != m_fields.end());
    if(iter != m_fields.end())
    {
        m_fields.erase(iter);
    }
}

//-----------------------------------------------------------------------------

void Object::fieldShallowCopy(const ::fwData::Object::csptr& source)
{
    this->setFields(source->getFields());
}

//-----------------------------------------------------------------------------

void Object::deepCopy(const ::fwData::Object::csptr& source)
{
    DeepCopyCacheType cache;
    return this->cachedDeepCopy(source, cache);
}
//-----------------------------------------------------------------------------

void Object::fieldDeepCopy(const ::fwData::Object::csptr& source)
{
    DeepCopyCacheType cache;
    return this->fieldDeepCopy(source, cache);
}

//-----------------------------------------------------------------------------

void Object::fieldDeepCopy(const ::fwData::Object::csptr& source, DeepCopyCacheType& cache)
{
    m_fields.clear();
    const ::fwData::Object::FieldMapType& sourceFields = source->getFields();
    for(const ::fwData::Object::FieldMapType::value_type& elt : sourceFields)
    {
        this->setField(elt.first, ::fwData::Object::copy(elt.second, cache));
    }
}

//-----------------------------------------------------------------------------

void Object::shallowCopy(const ::fwData::Object::csptr& source )
{
    FwCoreNotUsedMacro(source);
    SLM_FATAL("shallowCopy not implemented for : " + this->getClassname() );
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr Object::copy(const ::fwData::Object::csptr& source)
{
    DeepCopyCacheType cache;
    return Object::copy(source, cache);
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr Object::copy(const ::fwData::Object::csptr& source, Object::DeepCopyCacheType& cache)
{
    ::fwData::Object::sptr obj;

    if( source )
    {
        DeepCopyCacheType::const_iterator cacheItem = cache.find(source);

        if (cacheItem == cache.end())
        {
            obj = ::fwData::factory::New(source->getClassname());
            cache.insert( DeepCopyCacheType::value_type(source, obj) );
            obj->cachedDeepCopy(source, cache);
        }
        else
        {
            obj = cacheItem->second;
        }
    }

    return obj;
}

//-----------------------------------------------------------------------------

} // namespace fwData
