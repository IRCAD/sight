/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/spyLog.hpp>

#include <fwData/Object.hpp>
#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include "fwComEd/CompositeMsg.hpp"
#include "fwComEd/CompositeEditor.hpp"

namespace fwComEd
{

std::string CompositeMsg::MODIFIED_FIELDS = "MODIFIED_FIELDS";

std::string CompositeMsg::ADDED_FIELDS = "ADDED_FIELDS";
std::string CompositeMsg::REMOVED_FIELDS = "REMOVED_FIELDS";
std::string CompositeMsg::SWAPPED_FIELDS = "SWAPPED_FIELDS";

//-------------------------------------------------------------------------

CompositeMsg::CompositeMsg() throw()
: m_removedFields ( new ::fwData::Composite() ),
m_addedFields ( new ::fwData::Composite() ),
m_swappedOldFields ( new ::fwData::Composite() ),
m_swappedNewFields ( new ::fwData::Composite() )
{}

//-------------------------------------------------------------------------

CompositeMsg::~CompositeMsg() throw()
{}

//-------------------------------------------------------------------------

void CompositeMsg::addEventModifiedFields( const std::vector< std::string > & _modifiedFields )
{
    /*
    ::fwData::Composite::NewSptr dataInfo;

    std::vector< std::string >::const_iterator iter;
    for(iter = _modifiedFields.begin() ; iter != _modifiedFields.end() ; ++iter)
    {
        ::fwData::String::NewSptr strField;
        strField->value() = *iter;
        dataInfo->getRefMap()[strField->getUUID()] = strField;
    }

    this->addEvent(MODIFIED_FIELDS, dataInfo);
*/
    m_modifiedFields = _modifiedFields;
    this->addEvent(MODIFIED_FIELDS);

}

//-------------------------------------------------------------------------

void CompositeMsg::addEventModifiedField( std::string _modifiedField )
{

    if( ! this->hasEvent( MODIFIED_FIELDS ) )
    {
        this->addEvent( MODIFIED_FIELDS );
    }

    m_modifiedFields.push_back(_modifiedField);
}

//-------------------------------------------------------------------------

void CompositeMsg::addEventModifiedFields( const std::vector< std::string > & _modifiedFields, std::vector< ::fwData::Object::sptr > _oldObjects )
{
    m_modifiedFields = _modifiedFields;
    m_modifiedObjects = _oldObjects;
    this->addEvent(MODIFIED_FIELDS);

    /*
    ::fwData::Composite::NewSptr dataInfo;

    std::vector< std::string >::const_iterator iter;
    std::vector< ::fwData::Object::sptr >::iterator iterOldObj = _oldObjects.begin();
    for(iter = _modifiedFields.begin() ; iter != _modifiedFields.end() ; ++iter)
    {
        ::fwData::String::NewSptr strField;
        strField->value() = *iter;
        strField->setFieldSingleElement( "OLD_OBJECT", *iterOldObj );
        dataInfo->getRefMap()[strField->getUUID()] = strField;
        ++iterOldObj;
    }

    this->addEvent(MODIFIED_FIELDS, dataInfo);
     */
}

//-------------------------------------------------------------------------

std::vector< std::string > CompositeMsg::getEventModifiedFields() const
{
    SLM_ASSERT("sorry, CompositeMsg does not contained MODIFIED_FIELDS event", this->hasEvent(MODIFIED_FIELDS));

    return m_modifiedFields;

    /*
    ::fwData::Composite::csptr dataInfo = ::fwData::Composite::dynamicConstCast(this->getDataInfo(MODIFIED_FIELDS));

    std::vector< std::string > modifiedFields;
    ::fwData::Composite::Container::const_iterator iter;
    for(iter = dataInfo->getRefMap().begin() ; iter != dataInfo->getRefMap().end() ; ++iter)
    {
        ::fwData::String::sptr strField = ::fwData::String::dynamicCast(iter->second);
        modifiedFields.push_back(strField->value());
    }
    return modifiedFields;
    */
}

//-------------------------------------------------------------------------

//
///***************************************/
//CompositeMsg::CompositeMsg( ::boost::shared_ptr< ::fwData::Composite> _obj ) throw()
//: ObjectMsg(_obj)
//{}
//CompositeMsg::~CompositeMsg() throw()
//{}
//
//void CompositeMsg::info(std::ostream &_sstream ) const
//{
//  _sstream << std::endl << "Composite modification message : " << std::endl;
//  _sstream << "-----------------------------------" << std::endl;
//  _sstream << "Generic part: " << std::endl;
//  this->ObjectMsg::info(_sstream) ;
//  _sstream << "-----------------------------------" << std::endl;
//  _sstream << "Specific part (I/O): " << std::endl;
//  std::map< std::string , ::boost::shared_ptr< const ObjectMsg > >::const_iterator iter ;
//  for( iter = m_ModificationMap.begin() ; iter != m_ModificationMap.end() ; ++iter )
//  {
//      _sstream << "Element " << iter->first << " modified : " << std::endl;
//      _sstream << *(iter->second) << std::endl ;
//  }
//
//  _sstream << "-----------------------------------" << std::endl;
//}
//
//void CompositeMsg::addModif( ::boost::shared_ptr< const ObjectMsg > _message , ModifProp _prop )
//{
//  ::fwTools::Object::csptr  modifiedObj = _message->m_modifiedObject ;
//  ::fwData::Composite::Container::const_iterator iter;
//  ::fwData::Composite::csptr cObject = ::boost::dynamic_pointer_cast< const ::fwData::Composite  >(m_modifiedObject) ;
//
//  for( iter = cObject->getRefMap().begin() ; iter != cObject->getRefMap().end() ; ++iter )
//  {
//      if( iter->second == modifiedObj )
//      {
//          m_ModificationMap[iter->first]  = _message ;
//          m_ModifProp[iter->first]        = _prop ;
//          return ;
//      }
//  }
//  assert( false ) ; //object associated with the modification _message is not an item of the Composite!!!
//}
//
//std::vector< ::boost::shared_ptr< const fwServices::ObjectMsg > > CompositeMsg::getMsg() const
//{
//  std::vector< ::boost::shared_ptr< const ObjectMsg > > container ;
//  std::map< std::string , ::boost::shared_ptr< const ObjectMsg > >::iterator iter ;
//  for( iter = ( const_cast<CompositeMsg *>( this ) )->m_ModificationMap.begin() ; iter != ( const_cast<CompositeMsg *>( this ) )->m_ModificationMap.end() ; ++iter )
//  {
//      container.push_back( iter->second ) ;
//  }
//  return container ;
//}
//
//std::vector< std::string > CompositeMsg::getModifIds() const
//{
//  std::vector< std::string > result ;
//  std::map< std::string , ::boost::shared_ptr< const ObjectMsg > >::const_iterator iter ;
//  for( iter = m_ModificationMap.begin() ; iter != m_ModificationMap.end() ; ++iter )
//  {
//      result.push_back( iter->first ) ;
//  }
//  return result ;
//}
//
//::boost::shared_ptr< const fwServices::ObjectMsg > CompositeMsg::getMsg(std::string _id) const
//{
//  assert( m_ModificationMap.find( _id ) != m_ModificationMap.end() ) ;
//  ::boost::shared_ptr< const ObjectMsg > msg = ( const_cast<CompositeMsg *>( this ) )->m_ModificationMap[_id] ;
//  return msg ;
//}
//
//fwServices::ObjectMsg::ModifProp CompositeMsg::getProp(std::string _fieldId )
//{
//  assert( m_ModifProp.find( _fieldId ) != m_ModifProp.end() ) ;
//  return m_ModifProp[_fieldId] ;
//}


//=============================================================================

void CompositeMsg::addAddedField( std::string _compositeKey, ::fwData::Object::sptr _pNewObject )
{
    if( ! this->hasEvent( ADDED_FIELDS ) )
    {
        this->addEvent( ADDED_FIELDS, m_addedFields );
    }

    SLM_ASSERT("This composite key is already register", m_addedFields->getRefMap().find(_compositeKey) == m_addedFields->getRefMap().end() );

    m_addedFields->getRefMap()[ _compositeKey ] = _pNewObject;
    addEventModifiedField( _compositeKey );
}

//-----------------------------------------------------------------------------

::fwData::Composite::sptr CompositeMsg::getAddedFields() const
{
    return m_addedFields;
}

//-----------------------------------------------------------------------------

void CompositeMsg::addRemovedField( std::string _compositeKey, ::fwData::Object::sptr _pOldObject )
{
    if( ! this->hasEvent( REMOVED_FIELDS ) )
    {
        this->addEvent( REMOVED_FIELDS, m_removedFields );
    }

    SLM_ASSERT("This composite key is already register", m_removedFields->getRefMap().find(_compositeKey) == m_removedFields->getRefMap().end() );

    m_removedFields->getRefMap()[ _compositeKey ] = _pOldObject;
    addEventModifiedField( _compositeKey );
}

//-----------------------------------------------------------------------------

::fwData::Composite::sptr CompositeMsg::getRemovedFields() const
{
    return m_removedFields;
}

//-----------------------------------------------------------------------------

void CompositeMsg::addSwappedField( std::string _compositeKey, ::fwData::Object::sptr _pOldObject, ::fwData::Object::sptr _pNewObject )
{
    if( ! this->hasEvent( SWAPPED_FIELDS ) )
    {
        this->addEvent( SWAPPED_FIELDS, m_swappedOldFields );
    }

    SLM_ASSERT("This composite key is already register", m_swappedOldFields->getRefMap().find(_compositeKey) == m_swappedOldFields->getRefMap().end() );


    m_swappedOldFields->getRefMap()[ _compositeKey ] = _pOldObject;
    m_swappedNewFields->getRefMap()[ _compositeKey ] = _pNewObject;

    addEventModifiedField( _compositeKey );
}

//-----------------------------------------------------------------------------

::fwData::Composite::sptr CompositeMsg::getSwappedOldFields() const
{
    return m_swappedOldFields;
}

//-----------------------------------------------------------------------------

::fwData::Composite::sptr CompositeMsg::getSwappedNewFields() const
{
    return m_swappedNewFields;
}

//-----------------------------------------------------------------------------

} // namespace op

