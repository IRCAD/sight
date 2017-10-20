/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataTools/helper/Field.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/Exception.hpp>
#include <fwData/Object.hpp>

#include <algorithm>
#include <functional>

namespace fwDataTools
{
namespace helper
{

//-----------------------------------------------------------------------------

Field::Field( ::fwData::Object::sptr object ) :
    m_object( object )
{
}

//-----------------------------------------------------------------------------

Field::~Field()
{
}

//-----------------------------------------------------------------------------

void Field::setField(const fwData::Object::FieldNameType& name, fwData::Object::sptr obj)
{
    this->addOrSwap(name, obj);
}

//-----------------------------------------------------------------------------

void Field::setFields( const fwData::Object::FieldMapType& newFields)
{
    ::fwData::Object::sptr object = m_object.lock();
    SLM_ASSERT("Field helper need a non-null object pointer", object);
    const ::fwData::Object::FieldMapType oldFields = object->getFields();
    this->buildMessage(newFields, oldFields);
    object->setFields(newFields);
}

//-----------------------------------------------------------------------------

void Field::updateFields( const fwData::Object::FieldMapType& fieldMap)
{
    ::fwData::Object::sptr object = m_object.lock();
    SLM_ASSERT("Field helper need a non-null object pointer", object);
    const ::fwData::Object::FieldMapType oldFields = object->getFields();
    this->buildMessage(fieldMap, oldFields);
    object->updateFields(fieldMap);
}

//-----------------------------------------------------------------------------

void Field::removeField(const fwData::Object::FieldNameType& name)
{
    this->remove(name);
}

//-----------------------------------------------------------------------------

void Field::add(const fwData::Object::FieldNameType& _name, fwData::Object::sptr _obj)
{
    ::fwData::Object::sptr object = m_object.lock();
    SLM_ASSERT("Field helper need a non-null object pointer", object);

    ::fwData::Object::sptr field = object->getField(_name);
    FW_RAISE_EXCEPTION_IF(::fwData::Exception("Field already exists"), field);

    m_addedFields[_name] = _obj;

    object->setField(_name, _obj);
}

//-----------------------------------------------------------------------------

void Field::swap(const fwData::Object::FieldNameType& _name, fwData::Object::sptr _obj)
{
    SLM_ASSERT("Field helper need a non-null object pointer", !m_object.expired());
    ::fwData::Object::sptr object = m_object.lock();

    ::fwData::Object::sptr field = object->getField(_name);
    FW_RAISE_EXCEPTION_IF(::fwData::Exception("Field does not exist"), !field);

    m_newChangedFields[_name] = _obj;
    m_oldChangedFields[_name] = field;

    object->setField(_name, _obj);
}

//-----------------------------------------------------------------------------

void Field::addOrSwap(const fwData::Object::FieldNameType& _name, fwData::Object::sptr _obj)
{
    SLM_ASSERT("Field helper need a non-null object pointer", !m_object.expired());
    ::fwData::Object::sptr object = m_object.lock();

    ::fwData::Object::sptr field = object->getField(_name);

    if (!field)
    {
        m_addedFields[_name] = _obj;
    }
    else
    {
        m_newChangedFields[_name] = _obj;
        m_oldChangedFields[_name] = field;
    }
    object->setField(_name, _obj);

}

//-----------------------------------------------------------------------------

void Field::remove(const fwData::Object::FieldNameType& _name)
{
    SLM_ASSERT("Field helper need a non-null object pointer", !m_object.expired());
    ::fwData::Object::sptr object = m_object.lock();
    ::fwData::Object::sptr field  = object->getField(_name);

    FW_RAISE_EXCEPTION_IF(::fwData::Exception("Field does not exist"), !field);

    m_removedFields[_name] = field;
    object->removeField(_name);
}

//-----------------------------------------------------------------------------

void Field::clear()
{
    SLM_ASSERT("Field helper need a non-null object pointer", !m_object.expired());
    ::fwData::Object::sptr object = m_object.lock();

    auto fieldNames = object->getFieldNames();
    for(const auto& name : fieldNames)
    {
        ::fwData::Object::sptr field = object->getField(name);
        m_removedFields[name]        = field;
        object->removeField(name);
    }
}

//-----------------------------------------------------------------------------

void Field::notify()
{
    SLM_ASSERT("Field helper need a non-null object pointer", !m_object.expired());

    if ( !m_addedFields.empty() )
    {
        auto sig = m_object.lock()->signal< ::fwData::Object::AddedFieldsSignalType >(
            ::fwData::Object::s_ADDED_FIELDS_SIG);

        sig->asyncEmit(m_addedFields);
    }
    if ( !m_newChangedFields.empty() && !m_oldChangedFields.empty() )
    {
        auto sig = m_object.lock()->signal< ::fwData::Object::ChangedFieldsSignalType >(
            ::fwData::Object::s_CHANGED_FIELDS_SIG);

        sig->asyncEmit(m_newChangedFields, m_oldChangedFields);
    }
    if ( !m_removedFields.empty() )
    {
        auto sig = m_object.lock()->signal< ::fwData::Object::RemovedFieldsSignalType >(
            ::fwData::Object::s_REMOVED_FIELDS_SIG);

        sig->asyncEmit(m_removedFields);
    }
    OSLM_INFO_IF("No changes were found on the fields of the object '" + m_object.lock()->getID()
                 + "', nothing to notify.",
                 m_addedFields.empty() && m_newChangedFields.empty() && m_removedFields.empty());
}

//-----------------------------------------------------------------------------

void Field::buildMessage(const ::fwData::Object::FieldMapType& oldFields,
                         const ::fwData::Object::FieldMapType& newFields)
{
    ::fwData::Object::FieldNameVectorType oldFieldNames;
    ::fwData::Object::FieldNameVectorType newFieldNames;

    std::transform(
        oldFields.begin(), oldFields.end(),
        std::back_inserter(oldFieldNames),
        std::bind(&::fwData::Object::FieldMapType::value_type::first, std::placeholders::_1)
        );
    std::transform(
        newFields.begin(), newFields.end(),
        std::back_inserter(newFieldNames),
        std::bind(&::fwData::Object::FieldMapType::value_type::first, std::placeholders::_1)
        );

    std::sort(oldFieldNames.begin(), oldFieldNames.end());
    std::sort(newFieldNames.begin(), newFieldNames.end());

    ::fwData::Object::FieldNameVectorType added;   // new - old
    ::fwData::Object::FieldNameVectorType changed; // old & new
    ::fwData::Object::FieldNameVectorType removed; // old - new

    std::set_difference(
        newFieldNames.begin(), newFieldNames.end(),
        oldFieldNames.begin(), oldFieldNames.end(),
        std::back_inserter(added)
        );

    std::set_intersection(
        newFieldNames.begin(), newFieldNames.end(),
        oldFieldNames.begin(), oldFieldNames.end(),
        std::back_inserter(changed)
        );

    std::set_difference(
        oldFieldNames.begin(), oldFieldNames.end(),
        newFieldNames.begin(), newFieldNames.end(),
        std::back_inserter(removed)
        );

    for(const ::fwData::Object::FieldNameVectorType::value_type& fieldName :  added)
    {
        m_addedFields[fieldName] = newFields.find(fieldName)->second;
    }

    for(const ::fwData::Object::FieldNameVectorType::value_type& fieldName :  changed)
    {
        m_newChangedFields[fieldName] = newFields.find(fieldName)->second;
        m_oldChangedFields[fieldName] = oldFields.find(fieldName)->second;
    }

    for(const ::fwData::Object::FieldNameVectorType::value_type& fieldName :  changed)
    {
        m_removedFields[fieldName] = oldFields.find(fieldName)->second;
    }
}

} // namespace helper
} // namespace fwDataTools
