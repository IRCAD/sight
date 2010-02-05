/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Object.hpp>
#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include "fwComEd/CompositeMsg.hpp"
#include "fwComEd/CompositeEditor.hpp"

namespace fwComEd
{

std::string CompositeMsg::MODIFIED_FIELDS = "MODIFIED_FIELDS";

//-------------------------------------------------------------------------

CompositeMsg::CompositeMsg() throw()
{}

//-------------------------------------------------------------------------

CompositeMsg::~CompositeMsg() throw()
{}

//-------------------------------------------------------------------------

void CompositeMsg::addEventModifiedFields( const std::vector< std::string > & _modifiedFields )
{
    ::fwData::Composite::NewSptr dataInfo;

    std::vector< std::string >::const_iterator iter;
    for(iter = _modifiedFields.begin() ; iter != _modifiedFields.end() ; ++iter)
    {
        ::fwData::String::NewSptr strField;
        strField->value() = *iter;
        dataInfo->getRefMap()[strField->getUUID()] = strField;
    }

    this->addEvent(MODIFIED_FIELDS, dataInfo);

}

//-------------------------------------------------------------------------

void CompositeMsg::addEventModifiedFields( const std::vector< std::string > & _modifiedFields, std::vector< ::fwData::Object::sptr > _oldObjects )
{
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

}

//-------------------------------------------------------------------------

std::vector< std::string > CompositeMsg::getEventModifiedFields() const
{
    SLM_ASSERT("sorry, CompositeMsg does not contained MODIFIED_FIELDS event", this->hasEvent(MODIFIED_FIELDS));
    ::fwData::Composite::csptr dataInfo = ::fwData::Composite::dynamicConstCast(this->getDataInfo(MODIFIED_FIELDS));

    std::vector< std::string > modifiedFields;
    ::fwData::Composite::Container::const_iterator iter;
    for(iter = dataInfo->getRefMap().begin() ; iter != dataInfo->getRefMap().end() ; ++iter)
    {
        ::fwData::String::sptr strField = ::fwData::String::dynamicCast(iter->second);
        modifiedFields.push_back(strField->value());
    }
    return modifiedFields;
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

} // namespace op

