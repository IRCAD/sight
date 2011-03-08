///* ***** BEGIN LICENSE BLOCK *****
// * FW4SPL - Copyright (C) IRCAD, 2009-2010.
// * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
// * published by the Free Software Foundation.
// * ****** END LICENSE BLOCK ****** */
//
//#include <fwCore/base.hpp>
//
//#include <fwTools/ClassFactoryRegistry.hpp>
//#include <fwTools/fwID.hpp>
//
//#include <fwData/PatientDB.hpp>
//
//#include <fwRuntime/helper.hpp>
//#include <fwRuntime/Extension.hpp>
//
//#include <fwServices/ObjectMsg.hpp>
//#include <fwServices/macros.hpp>
//#include <fwServices/Base.hpp>
//
//#include <fwGui/dialog/MessageDialog.hpp>
//
//#include "gui/action/ActionNotifyService.hpp"
//
//namespace gui
//{
//namespace action
//{
//
//REGISTER_SERVICE( ::fwGui::IActionSrv, ::gui::action::ActionNotifyService , ::fwTools::Object ) ;
//
//
//ActionNotifyService::ActionNotifyService() throw() : m_onevent("")
//{}
//
////-----------------------------------------------------------------------------
//
//ActionNotifyService::~ActionNotifyService() throw()
//{}
//
////-----------------------------------------------------------------------------
//
//void ActionNotifyService::info(std::ostream &_sstream )
//{
//    _sstream << "NotifyService Action" << std::endl;
//}
////-----------------------------------------------------------------------------
//
//void ActionNotifyService::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
//{
//    SLM_TRACE_FUNC() ;
//
//    if(_msg->hasEvent(m_onevent))
//    {
//        if(m_isCheckable)
//        {
//            m_isCheck=true;
//            setCheck(m_isCheck);
//        }
//    }
//}
//
////-----------------------------------------------------------------------------
//
//void ActionNotifyService::updating() throw( ::fwTools::Failed )
//{
//    SLM_TRACE("updating ActionNotifyService") ;
//    this->::gui::action::IAction::updating();
//
//    for(size_t i = 0; i < m_uuidServices.size(); i++)
//    {
//        MessageType message =  m_uuidServices.at(i).second;
//        std::string uid =  m_uuidServices.at(i).first;
//        bool obj_exists = ::fwTools::fwID::exist(uid );
//
//        ::fwData::Object::sptr object = ::fwData::Object::dynamicCast( ::fwTools::fwID::getObject(uid) ) ;
//
//        ::fwData::PatientDB::sptr patientDB = ::fwData::PatientDB::dynamicCast( ::fwTools::fwID::getObject(uid) ) ;
//
//        if(patientDB)
//        {
//            if(patientDB->getPatientSize()==0)
//            {
//                SLM_WARN("Please insert a image.");
//                m_isCheck=false;
//                setCheck(m_isCheck);
//
//                return;
//            }
//        }
//
//        // Manage special message
//        if ( message == ADD_OR_REMOVE && m_isCheckable)
//        {
//            if(m_isCheck)
//            {
//                message = ADD;
//            }
//            else
//            {
//                message = REMOVE;
//            }
//        }
//
//        if ( message == ADD_OR_REMOVE && !m_isCheckable)
//        {
//            SLM_FATAL("Sorry, but to use \"ADD_OR_REMOVE\", the action needs to be checkable.");
//        }
//
//
//        if( message != DO_NOTHING)
//        {
//
//            switch ( message )
//            {
//            case ADD :
//            {
//                ::fwServices::ObjectMsg::NewSptr msg;
//                msg->addEvent( "ADD" ) ;
//                ::fwServices::IEditionService::notify(this->getSptr(),  object, msg);
//                break;
//
//            }
//            case REMOVE :
//            {
//                ::fwServices::ObjectMsg::NewSptr msg;
//                msg->addEvent( "REMOVE" ) ;
//                ::fwServices::IEditionService::notify(this->getSptr(), object, msg);
//                break;
//            }
//            default :
//            {
//                SLM_FATAL("Sorry, this message type is not managed");
//                break;
//            }
//            }
//        }
//        else
//        {
//            std::string msgInfo = "Sorry, the service is unavailable.";
//            ::fwGui::dialog::IMessageDialog::Icons icon = ::fwGui::dialog::IMessageDialog::WARNING;
//            ::fwGui::dialog::MessageDialog messageBox;
//            messageBox.setTitle("Warning");
//            messageBox.setMessage( msgInfo );
//            messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
//            messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
//            messageBox.show();
//            OSLM_INFO("Do nothing for Service " << m_uuidServices.at(i).first);
//        }
//    }
//}
//
////-----------------------------------------------------------------------------
//
//void ActionNotifyService::configuring() throw( ::fwTools::Failed )
//{
//    SLM_TRACE("configuring ActionNotifyService") ;
//    this->::gui::action::IAction::configuring() ;
//
//    ::fwRuntime::ConfigurationElementContainer::Iterator iter = this->m_configuration->begin() ;
//    for( ; iter != this->m_configuration->end() ; ++iter )
//    {
//        OSLM_INFO( "ActionNotifyService "  << (*iter)->getName());
//
//        SLM_ASSERT("Attribute event missing", (*iter)->hasAttribute("event")) ;
//        std::string messageType =  (*iter)->getExistingAttributeValue("event") ;
//
//        MessageType message;
//        if ( messageType == "REMOVE" )              { message = REMOVE; }
//        else if ( messageType == "ADD" )            { message = ADD; }
//        else if ( messageType == "ADD_OR_REMOVE" )  { message = ADD_OR_REMOVE; }
//        else
//        {
//            OSLM_FATAL("Sorry this type of \"messageType\":" << messageType <<" is not managed by ActionNotifyService");
//        }
//
//        std::string uuid = (this->getObject< ::fwData::Object >())->getID();
//
//        m_onevent =  (*iter)->getExistingAttributeValue("checkonEvent") ;
//
//        m_uuidServices.push_back( std::make_pair(uuid, message) );
//    }
//}
//
//}
//}
