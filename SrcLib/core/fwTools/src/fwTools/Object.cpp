/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/HiResClock.hpp>

#include "fwTools/ClassRegistrar.hpp"
#include "fwTools/Object.hpp"
#include "fwTools/UUID.hpp"

namespace fwTools {


//------------------------------------------------------------------------------
REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwTools::Object, ::fwTools::Object );
//------------------------------------------------------------------------------

const std::string & getLabel(const ::fwTools::Object *obj)
{
	const Field *lobj = dynamic_cast< const fwTools::Field *>( obj );
	assert( lobj );
	return lobj->label();
}

//------------------------------------------------------------------------------

::fwTools::Field::sptr castToField(::fwTools::Object::sptr obj)
{
	assert( fwTools::Field::dynamicCast(obj) );
	return fwTools::Field::dynamicCast(obj);
}

//------------------------------------------------------------------------------

Field::Field()
{
}

//------------------------------------------------------------------------------

Field::Field(const std::string &newLabel ) :  m_label(newLabel)
{
}

//------------------------------------------------------------------------------

Field::~Field()
{
}

//------------------------------------------------------------------------------

std::string & Field::label()
{
	return m_label;
}

//------------------------------------------------------------------------------

const std::string & Field::label() const
{
	return m_label;
}


Object::Object() :
    m_timeStamp ( ::fwCore::TimeStamp::New()  ),
    m_logicStamp( ::fwCore::LogicStamp::New() ),
    m_deleter( IDeleter::New() )
{
	// TODO Auto-generated constructor stub
	assert( m_deleter.get() );
}



Object::~Object()
{
	// TODO Auto-generated destructor stub
	m_deleter->Delete( this ) ;
}

void Object::setDeleter( ::fwTools::IDeleter::sptr _deleter )
{
	m_deleter = _deleter ;
}
::fwTools::IDeleter::sptr Object::getDeleter()
{
	return m_deleter ;
}

//------------------------------------------------------------------------------


std::string Object::className() const
{
	return this->getClassname();
}



std::string Object::getUUID() const
{
	if ( m_cachedSimpleUuid.empty() )
	{
		m_cachedSimpleUuid = UUID::get( Object::constCast(this->getConstSptr()),UUID::SIMPLE );
	}
	return m_cachedSimpleUuid;
}



bool Object::hasUUID() const
{
	return !m_cachedSimpleUuid.empty() || UUID::supervise( shared_from_this() );
}


void Object::setUUID(const std::string &newID)
{
	assert( hasUUID()== false );
	assert( UUID::exist( newID , UUID::SIMPLE) == false );
	UUID::impose( shared_from_this(), newID, UUID::SIMPLE );
}


//------------------------------------------------------------------------------

::fwTools::Field::sptr Object::setField( const FieldID &fieldId )
{
	Field::sptr field = getField(fieldId);

	if ( field.get()==NULL )
	{
		fwTools::Field::NewSptr newfield(fieldId);
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
//		assert( lobj );
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
//		assert( lobj );
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
		assert( field->children().size() < 2 ); // change behavior of Field ?? many element -> one ?
		field->children().clear();
		field->children().push_back( newSubObject );
		return field;
	}
	else
	{
		// field doesn no exist create it then append new element
		fwTools::Field::NewSptr newfield(fieldId);
		newfield->children().push_back(newSubObject);
		m_children.push_back( newfield);
		return newfield;
	}

}



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
		// field doesn no exist create it then append new element
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



















//------------------------------------------------------------------------------



// DEPRECATED
//------------------------------------------------------------------------------
//
//bool Object::getFieldSize( const FieldID& id ) const throw()
//{
//	const ::fwTools::Field::sptr labeledObject =
//		::fwTools::Field::dynamicCast( getField( id ) );
//	return labeledObject && !labeledObject->children().empty();
//}
////------------------------------------------------------------------------------
//
//bool Object::modifyField( const FieldID& id,
//                          const ::fwTools::Object::sptr data ) throw()
//{
//	if ( getFieldSize( id ) )
//	{
//		setFieldSingleElement( id,  data );
//		return true;
//	}
//	return false;
//}



//::fwTools::Field::sptr Object::setFieldSingleElement( const FieldID &fieldId,  ::fwTools::Object::sptr newSubObject )
//{
//	::fwTools::Field::sptr field = getField(fieldId);
//
//	if ( field.get()==NULL )
//	{
//		::fwTools::Field::NewSptr newfield(fieldId);
//		newfield->children().push_back(newSubObject);
//		m_children.push_back( newfield);
//		return newfield;
//	}
//	else
//	{
//		// labeled object already exist set new subObject
//		field->children().clear();
//		field->children().push_back( newSubObject );
//		return field ;
//
//	}
//}

////------------------------------------------------------------------------------
//
//void Object::setFieldElement(std::string _id, ::fwTools::Object::sptr _obj, unsigned int _index )
//{
//	::fwTools::Field::sptr infoOfInterest = this->setField(_id) ;
//	//infoOfInterest
//	while( _index >= infoOfInterest->children().size() )
//	{
//        fwTools::Object::NewSptr newObject;
//		infoOfInterest->children().push_back( newObject ) ;
//	}
//	infoOfInterest->children()[_index] = _obj ;
//}



} //namespace fwTools



