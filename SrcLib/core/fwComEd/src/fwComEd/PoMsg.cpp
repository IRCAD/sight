/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include "fwComEd/PoMsg.hpp"
#include "fwComEd/ProcessObjectEditor.hpp"
#include <fwData/Object.hpp>
#include <fwData/ProcessObject.hpp>

namespace fwComEd
{
//
///***************************************/
//ProcessObjectMsg::ProcessObjectMsg( ::boost::shared_ptr< ::fwData::ProcessObject> _obj ) throw()
//: ObjectMsg(_obj)
//{}
//ProcessObjectMsg::~ProcessObjectMsg() throw()
//{}
//
//void ProcessObjectMsg::info(std::ostream &_sstream ) const
//{
//	_sstream << std::endl << "ProcessObject modification message : " << std::endl;
//	_sstream << "-----------------------------------" << std::endl;
//	_sstream << "Generic part: " << std::endl;
//	this->ObjectMsg::info(_sstream) ;
//	_sstream << "-----------------------------------" << std::endl;
//	_sstream << "Specific part (I/O): " << std::endl;
//	std::map< std::string , ::boost::shared_ptr< const ObjectMsg > >::const_iterator iter ;
//	for( iter = m_inputModificationMap.begin() ; iter != m_inputModificationMap.end() ; ++iter )
//	{
//		_sstream << "Input " << iter->first << " modified : " << std::endl;
//		_sstream << *(iter->second) << std::endl ;
//	}
//
//	_sstream << "-----------------------------------" << std::endl;
//
//	for( iter = m_outputModificationMap.begin() ; iter != m_outputModificationMap.end() ; ++iter )
//	{
//		_sstream << "-----------------------------------" << std::endl;
//		_sstream << "Output " << iter->first << " modified : " << std::endl;
//		_sstream << *(iter->second) << std::endl ;
//	}
//	_sstream << "-----------------------------------" << std::endl;
//}
//
//void ProcessObjectMsg::addIoModif( ::boost::shared_ptr< const ObjectMsg > _message , ModifProp _prop )
//{
//	::fwTools::Object::csptr  modifiedObj = _message->m_modifiedObject ;
//	::fwTools::Object::ChildContainer::const_iterator iter;
//	::fwData::ProcessObject::csptr poThisObject = boost::dynamic_pointer_cast< const ::fwData::ProcessObject >( m_modifiedObject );
//	/// INPUTS
//	for( iter = poThisObject->getInputs()->children().begin() ; iter != poThisObject->getInputs()->children().end() ; ++iter )
//	{
//		if( castToField(*iter)->children().at(0) == modifiedObj )
//		{
//			m_inputModificationMap[castToField(*iter)->label()] 	= _message ;
//			m_inputModifProp[castToField(*iter)->label()] 		= _prop ;
//			return ;
//		}
//	}
//	/// OUTPUTS
//	for(iter = poThisObject->getOutputs()->children().begin() ; iter != poThisObject->getOutputs()->children().end() ; ++iter )
//	{
//		if( castToField(*iter)->children().at(0) == modifiedObj )
//		{
//			m_outputModificationMap[castToField(*iter)->label()] = _message ;
//			m_outputModifProp[castToField(*iter)->label()] 		= _prop ;
//			return ;
//		}
//	}
//	assert( false ) ; //object associated with the modification _message is not an input nor an output of the processObject!!!
//}
//
//std::vector< ::boost::shared_ptr< const fwServices::ObjectMsg > > ProcessObjectMsg::getIOMsg() const
//{
//	std::vector< ::boost::shared_ptr< const ObjectMsg > > container ;
//	std::map< std::string , ::boost::shared_ptr< const ObjectMsg > >::iterator iter ;
//	for( iter = ( const_cast<ProcessObjectMsg *>( this ) )->m_inputModificationMap.begin() ; iter != ( const_cast<ProcessObjectMsg *>( this ) )->m_inputModificationMap.end() ; ++iter )
//	{
//		container.push_back( iter->second ) ;
//	}
//	for( iter = ( const_cast<ProcessObjectMsg *>( this ) )->m_outputModificationMap.begin() ; iter != ( const_cast<ProcessObjectMsg *>( this ) )->m_outputModificationMap.end() ; ++iter )
//	{
//		container.push_back( iter->second ) ;
//	}
//	return container ;
//}
//
//std::vector< std::string > ProcessObjectMsg::getInputModifIds() const
//{
//	std::vector< std::string > result ;
//	std::map< std::string , ::boost::shared_ptr< const ObjectMsg > >::const_iterator iter ;
//	for( iter = m_inputModificationMap.begin() ; iter != m_inputModificationMap.end() ; ++iter )
//	{
//		result.push_back( iter->first ) ;
//	}
//	return result ;
//}
//
//::boost::shared_ptr< const fwServices::ObjectMsg > ProcessObjectMsg::getInputMsg(std::string _id) const
//{
//	assert( m_inputModificationMap.find( _id ) != m_inputModificationMap.end() ) ;
//	::boost::shared_ptr< const ObjectMsg > msg = ( const_cast<ProcessObjectMsg *>( this ) )->m_inputModificationMap[_id] ;
//	return msg ;
//}
//
//fwServices::ObjectMsg::ModifProp ProcessObjectMsg::getInputProp(std::string _fieldId )
//{
//	assert( m_inputModifProp.find( _fieldId ) != m_inputModifProp.end() ) ;
//	return m_inputModifProp[_fieldId] ;
//}
//
//std::vector< std::string > ProcessObjectMsg::getOutputModifIds() const
//{
//	std::vector< std::string > result ;
//	std::map< std::string , ::boost::shared_ptr< const ObjectMsg > >::const_iterator iter ;
//	for( iter = m_outputModificationMap.begin() ; iter != m_outputModificationMap.end() ; ++iter )
//	{
//		result.push_back( iter->first ) ;
//	}
//	return result ;
//}
//
//::boost::shared_ptr< const fwServices::ObjectMsg > ProcessObjectMsg::getOutputMsg(std::string _id) const
//{
//	assert( m_outputModificationMap.find( _id ) != m_outputModificationMap.end() ) ;
//	::boost::shared_ptr< const ObjectMsg > msg = ( const_cast<ProcessObjectMsg *>( this ) )->m_outputModificationMap[_id] ;
//	return msg ;
//}
//
//fwServices::ObjectMsg::ModifProp ProcessObjectMsg::getOutputProp(std::string _fieldId )
//{
//	assert( m_outputModifProp.find( _fieldId ) != m_outputModifProp.end() ) ;
//	return m_outputModifProp[_fieldId] ;
//}

} // namespace op

