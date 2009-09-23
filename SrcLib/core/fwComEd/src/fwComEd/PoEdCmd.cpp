/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

//#include "fwComEd/PoEdCmd.hpp"
//#include "fwComEd/ProcessObjectEditor.hpp"
//#include "fwComEd/PoMsg.hpp"
//#include <fwServices/ObjectServiceRegistry.hpp>
//#include <fwServices/helper.hpp>
//#include <fwServices/ComChannelService.hpp>
//#include <fwData/ProcessObject.hpp>
//#include <fwServices/IEditionService.hpp>
//
//namespace fwComEd
//{
//
//REGISTER_CMD( fwComEd::POEditCmd , ::fwData::ProcessObject );
//
///*************************************************/
//POEditCmd::POEditCmd() 
//{
//	m_inputs.clear();
//	m_outputs.clear();
//}
//
//POEditCmd::~POEditCmd()
//{
//}
//
//void POEditCmd::setInputValue( std::string _id , ::boost::shared_ptr< ::fwData::Object > _object)
//{
//	// save current m_inputs for undoing
//	m_inputs[ _id ] = _object ;
//}
//
//void POEditCmd::setOutputValue( std::string _id , ::boost::shared_ptr< ::fwData::Object > _object)
//{
//	// save current m_outputs for undoing
//	m_outputs[ _id ] = _object ;
//}
//
///// Overrides
//void POEditCmd::apply() 
//{
//	// Precondition
//	::fwData::ProcessObject * processObject = dynamic_cast< ::fwData::ProcessObject *>(m_editedObject) ;
//	fwServices::IEditionService * editor = fwServices::get< fwServices::IEditionService >(m_editedObject) ;	
//	assert( processObject ) ;
//	assert( editor ) ;
//
//	// Prepare notification message
//	 ::boost::shared_ptr< fwComEd::ProcessObjectMsg > poMsg( new fwComEd::ProcessObjectMsg( processObject ) ) ;
//
////	typedef fwServices::ObjectObservationService< fwComEd::ProcessObjectEditor > IOObserverType ;
////	typedef 
//	// Apply modification and update notification message
//	if( !m_inputs.empty() )
//	{
//		for(std::map< std::string , ::boost::shared_ptr< ::fwData::Object > >::iterator iter = m_inputs.begin() ; iter != m_inputs.end() ; ++iter )
//		{
//			// FIXME: unregister observation old input is released...
//			// Install and activate operator to ::fwData::Object observation
////			 ::boost::shared_ptr< fwComEd::ProcessObjectObservationEditorService > obsService( new fwComEd::ProcessObjectObservationEditorService );
////			fwServices::ObjectServiceRegistry::getDefault()->registerService( processObject , obsService );
////			obsService->setObservedObject( iter->second ) ;			
////			obsService->activateObservation();
////			 ::boost::shared_ptr< fwComEd::ProcessObjectObservationEditorService > obsService( new fwComEd::ProcessObjectObservationEditorService );
//			fwServices::registerCommunicationChannel(iter->second,editor)->start();			
////			 ::boost::shared_ptr< fwServices::ComChannelService > obsService( new fwServices::ComChannelService() );			
////			fwServices::OSR::registerService( processObject , obsService );
////			obsService->setDest(editor) ;
////			obsService->setSrc( iter->second ) ;			
////			obsService->start();
//			
//			// Set input
//			processObject->setInputValue( iter->first , iter->second ) ;	
//
//			// Inform the modification message
//			 ::boost::shared_ptr< fwServices::ObjectMsg > objectMsg( new fwServices::ObjectMsg( iter->second ) );
//			poMsg->addIoModif( objectMsg ) ;
//		}
//		m_inputs.clear(); 
//	}
//	if( !m_outputs.empty() )
//	{
//		for(std::map< std::string , ::boost::shared_ptr< ::fwData::Object > >::iterator iter = m_outputs.begin() ; iter != m_outputs.end() ; ++iter )
//		{
//			// Install and activate operator to ::fwData::Object observation
////			 ::boost::shared_ptr< fwServices::ProcessObjectObservationEditorService > obsService( new fwServices::ProcessObjectObservationEditorService );
////			 ::boost::shared_ptr< fwServices::ComChannelService > obsService( new fwServices::ComChannelService() );
//			fwServices::registerCommunicationChannel(iter->second,editor)->start();
////			fwServices::OSR::registerService( processObject , obsService );
////			obsService->setDest(editor) ;
////			obsService->setSrc( iter->second ) ;			
////			obsService->start();
//
//			processObject->setOutputValue( iter->first , iter->second ) ;	
//
//			// Inform the modification message
//			 ::boost::shared_ptr< fwServices::ObjectMsg > objectmsg( new fwServices::ObjectMsg( iter->second ) );
//			pomsg->addIoModif( objectmsg ) ;
//
//		}
//		m_outputs.clear(); 
//	}
//
//	editor->notify( pomsg );
//};
///// Overrides
//void POEditCmd::unapply() 
//{
//	std::cout << "In jb unapply command: test" << std::endl;
//};
//
//
//}
