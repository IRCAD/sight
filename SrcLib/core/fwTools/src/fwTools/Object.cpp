/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/HiResClock.hpp>

#include "fwTools/ClassRegistrar.hpp"
#include "fwTools/Object.hpp"
#include "fwTools/Field.hpp"
#include "fwTools/UUID.hpp"
#include "fwTools/Factory.hpp"

namespace fwTools
{

//------------------------------------------------------------------------------

Object::Object() :
    m_timeStamp ( ::fwCore::TimeStamp::New()  ),
    m_logicStamp( ::fwCore::LogicStamp::New() ),
    m_uuid( ::fwTools::UUID::New() )
{
    m_OSRKey = ::fwCore::LogicStamp::New();
    m_OSRKey->modified();
}

//------------------------------------------------------------------------------

Object::~Object()
{
    ::fwTools::Factory::uninitData(m_OSRKey);
}

//------------------------------------------------------------------------------

Object &Object::operator=(const Object &_obj)
{
    SLM_FATAL("This operator is forbidden, use shallowCopy or deepCopy instead.");
    m_children =_obj.m_children;
    m_timeStamp =_obj.m_timeStamp;
    m_logicStamp =_obj.m_logicStamp;
    return (*this);
}

//------------------------------------------------------------------------------

std::string Object::className() const
{
    return this->getClassname();
}

//------------------------------------------------------------------------------

::fwTools::Field::sptr Object::setField( const FieldID &fieldId )
{
    Field::sptr field = getField(fieldId);
    if ( field.get()==NULL )
    {
        ::fwTools::Field::NewSptr newfield(fieldId);
        m_children.push_back( newfield );
        return newfield ;
    }
    else
    {
        field->children().clear();
        return field;
    }
}

//------------------------------------------------------------------------------

::fwTools::Field::sptr Object::getField( const FieldID &fieldId )
{
    for ( ChildContainer::iterator f = m_children.begin() ;  f != m_children.end(); ++f )
    {
        ::fwTools::Field::sptr lobj = ::fwTools::Field::dynamicCast( *f );
        if ( lobj && lobj->label() == fieldId )
        {
            return lobj;
        }
    }
    return ::fwTools::Field::sptr();
}

//------------------------------------------------------------------------------

::fwTools::Field::csptr Object::getField( const FieldID &fieldId ) const
{
    for ( ChildContainer::const_iterator f = m_children.begin() ;  f != m_children.end(); ++f )
    {
        Field::sptr lobj = ::fwTools::Field::dynamicCast( *f );
        if ( lobj && lobj->label() == fieldId )
        {
            return lobj;
        }
    }
    return Field::sptr();
}

//------------------------------------------------------------------------------

::fwTools::Field::sptr Object::getField( std::vector< std::string > _path , const FieldID &fieldId )
{
    Field::sptr _result ;
    _result = this->getField(_path[0]) ;
    if( !_result )
    {
        return _result ;
    }
    for(unsigned int i = 1 ; i < _path.size() ; ++i )
    {
        _result = _result->getField(_path[i]) ;
        if( !_result )
        {
            return _result ;
        }
    }
    return _result->getField(fieldId) ;
}

//------------------------------------------------------------------------------

void Object::removeField( const FieldID &fieldId )
{
    for ( ChildContainer::iterator f = m_children.begin() ;  f != m_children.end(); ++f )
    {
        ::fwTools::Field::sptr field = ::fwTools::Field::dynamicCast( *f );

        if ( field && field->label() == fieldId )
        {
            m_children.erase(f);
            // all sub field erased if not pointed
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////
////////////////// management of elements in field /////////////////////////
////////////////////////////////////////////////////////////////////////////


::fwTools::Field::sptr Object::setFieldSingleElement( const FieldID &fieldId, ::fwTools::Object::sptr newSubObject )
{
    ::fwTools::Field::sptr field = getField(fieldId);
    if ( getField(fieldId) )
    {
        // field exist append newSubObject as *unique* element
        OSLM_ASSERT("Field "<<fieldId<<" exist, replace only unique element",
                field->children().size() < 2 ); // change behavior of Field ?? many element -> one ?
        field->children().clear();
        field->children().push_back( newSubObject );
        return field;
    }
    else
    {
        // field doesn't no exist create it then append new element
        ::fwTools::Field::NewSptr newfield(fieldId);
        newfield->children().push_back(newSubObject);
        m_children.push_back( newfield);
        return newfield;
    }
}

//------------------------------------------------------------------------------

::fwTools::Field::sptr Object::addFieldElement( const FieldID &fieldId, ::fwTools::Object::sptr newSubObject )
{
    ::fwTools::Field::sptr field = getField(fieldId);

    if ( getField(fieldId) )
    {
        // field exist append newSubObject as new element
        field->children().push_back( newSubObject );
        return field;
    }
    else
    {
        // field does not exist, create it then append new element
        fwTools::Field::NewSptr newfield(fieldId);
        newfield->children().push_back(newSubObject);
        m_children.push_back( newfield);
        return newfield;
    }
}

//------------------------------------------------------------------------------

void Object::removeFieldElement( const FieldID &fieldId, ::fwTools::Object::sptr subObjectToRemove )
{
    ::fwTools::Field::sptr field = getField(fieldId);
    if ( field)
    {
        ChildContainer::iterator i;
        i = std::find( field->children().begin(),  field->children().end(), subObjectToRemove );
        if ( i != field->children().end() )
        {
            field->children().erase(i);
        }
    }
}

//------------------------------------------------------------------------------

int Object::getFieldSize( const FieldID& id ) const throw()
{
    ::fwTools::Field::csptr field = ::fwTools::Field::dynamicConstCast( getField( id ) );
    return (field ? field->children().size() : 0 );
}

//-----------------------------------------------------------------------------

} //namespace fwTools



