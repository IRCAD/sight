/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <boost/bind.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/Factory.hpp"
#include "fwData/Object.hpp"

// ACH HACK, Force registration in factory
REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwTools::Object, ::fwTools::Object );
REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwTools::Field,  ::fwTools::Field );

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Object, ::fwData::Object);

namespace fwData
{

//------------------------------------------------------------------------------

Object::Object()
{}

//------------------------------------------------------------------------------

Object::~Object()
{}

//------------------------------------------------------------------------------

::fwData::Object::sptr Object::getField_NEWAPI( const FieldNameType & name ) const
{
    ::fwData::Object::sptr object;
    FieldMapType::const_iterator iter = m_fields.find(name);
    if(iter != m_fields.end())
    {
        object = iter->second;
    }
    return object;
}

//------------------------------------------------------------------------------

::fwData::Object::csptr Object::getConstField_NEWAPI( const FieldNameType & name ) const
{
    return this->getField_NEWAPI(name);
}

//------------------------------------------------------------------------------

const Object::FieldMapType& Object::getFields_NEWAPI() const
{
    return m_fields;
}

//------------------------------------------------------------------------------

Object::FieldNameVectorType Object::getFieldNames_NEWAPI() const
{
    FieldNameVectorType names;
    std::transform( m_fields.begin(), m_fields.end(),
            std::back_inserter(names),
            ::boost::bind(& FieldMapType::value_type::first, _1) );
    return names;
}

//------------------------------------------------------------------------------

void Object::setField_NEWAPI( const FieldNameType & name, ::fwData::Object::sptr obj)
{
    std::pair<FieldMapType::iterator, bool> res = m_fields.insert(FieldMapType::value_type(name, obj));
    if( !res.second )
    {
        res.first->second = obj;
    }
}

//------------------------------------------------------------------------------

void Object::setFields_NEWAPI( const FieldMapType & fieldMap )
{
    m_fields = fieldMap;
}

//------------------------------------------------------------------------------

void Object::updateFields_NEWAPI( const FieldMapType & fieldMap )
{
    m_fields.insert(fieldMap.begin(), fieldMap.end());
}


//-----------------------------------------------------------------------------

void Object::fieldShallowCopy( ::fwData::Object::csptr source )
{
    this->setFields_NEWAPI(source->getFields_NEWAPI());
}

//-----------------------------------------------------------------------------

void Object::fieldDeepCopy( ::fwData::Object::csptr source )
{
    m_fields.clear();
    ::fwData::Object::FieldMapType sourceFields = source->getFields_NEWAPI();
    BOOST_FOREACH(FieldMapType::value_type elt, sourceFields)
    {
        ::fwData::Object::sptr newObject = ::fwData::Factory::New( elt.second->getClassname() );
        newObject->deepCopy(elt.second);
        this->setField_NEWAPI(elt.first, newObject);
    }
}

//-----------------------------------------------------------------------------

void Object::shallowCopy( ::fwData::Object::csptr source )
{
    OSLM_FATAL("shallowCopy not implemented for : " << this->getClassname() );
}

//-----------------------------------------------------------------------------

void Object::deepCopy( ::fwData::Object::csptr source )
{
    OSLM_FATAL("deepCopy not implemented for : " << this->getClassname() );
}

} // namespace fwData
