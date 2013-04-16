/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <boost/bind.hpp>

#include "fwData/factory/new.hpp"
#include "fwData/Object.hpp"


namespace fwData
{

//------------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType Object::s_OBJECT_MODIFIED_SIG = "objectModified";

//------------------------------------------------------------------------------

Object::Object()
{
    // Init
    m_sigObjectModified = ObjectModifiedSignalType::New();

    // Register
    m_signals( s_OBJECT_MODIFIED_SIG,  m_sigObjectModified);

#ifdef COM_LOG
    ::fwCom::HasSignals::m_signals.setID();
#endif
}

//------------------------------------------------------------------------------

Object::~Object()
{}

//------------------------------------------------------------------------------

::fwData::Object::sptr Object::getField( const FieldNameType & name, ::fwData::Object::sptr defaultValue ) const
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

::fwData::Object::csptr Object::getConstField( const FieldNameType & name ) const
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
            ::boost::bind(& FieldMapType::value_type::first, _1) );
    return names;
}

//------------------------------------------------------------------------------

void Object::setField( const FieldNameType & name, ::fwData::Object::sptr obj)
{
    std::pair<FieldMapType::iterator, bool> res = m_fields.insert(FieldMapType::value_type(name, obj));
    if( !res.second )
    {
        res.first->second = obj;
    }
}

//------------------------------------------------------------------------------

void Object::setFields( const FieldMapType & fieldMap )
{
    m_fields = fieldMap;
}

//------------------------------------------------------------------------------

void Object::removeField( const FieldNameType & name )
{
    FieldMapType::const_iterator iter = m_fields.find(name);
    OSLM_ASSERT("Field "<<name<<" not found.", iter != m_fields.end());
    if(iter != m_fields.end())
    {
        m_fields.erase(iter);
    }
}

//------------------------------------------------------------------------------

void Object::updateFields( const FieldMapType & fieldMap )
{
    m_fields.insert(fieldMap.begin(), fieldMap.end());
}


//-----------------------------------------------------------------------------

void Object::fieldShallowCopy(const ::fwData::Object::csptr &source )
{
    this->setFields(source->getFields());
}

//-----------------------------------------------------------------------------

void Object::fieldDeepCopy(const ::fwData::Object::csptr &source )
{
    m_fields.clear();
    ::fwData::Object::FieldMapType sourceFields = source->getFields();
    BOOST_FOREACH(::fwData::Object::FieldMapType::value_type elt, sourceFields)
    {
        this->setField(elt.first, ::fwData::Object::copy(elt.second));
    }
}

//-----------------------------------------------------------------------------

void Object::shallowCopy(const ::fwData::Object::csptr &source )
{
    FwCoreNotUsedMacro(source);
    OSLM_FATAL("shallowCopy not implemented for : " << this->getClassname() );
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr Object::copy(const ::fwData::Object::csptr &source)
{
    ::fwData::Object::sptr newObj;
    if( source )
    {
        newObj = ::fwData::factory::New(source->className());
        newObj->deepCopy(source);
    }
    return newObj;
}

//-----------------------------------------------------------------------------

#ifdef COM_LOG
void Object::setID( ::fwTools::fwID::IDType newID )
{
    if( ! this->hasID() ||
        this->getID( ::fwTools::fwID::MUST_EXIST ) != newID )
    {
        this->::fwTools::fwID::setID( newID );
    }

    std::string lightID = this->getLightID( ::fwTools::fwID::MUST_EXIST );

    ::fwCom::HasSignals::m_signals.setID( lightID + "::" );
}
#endif

} // namespace fwData
