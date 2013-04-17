/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <set>
#include <boost/assign/list_of.hpp>

#include <fwTools/UUID.hpp>
#include <fwTools/fwID.hpp>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>
#include <fwData/Boolean.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/CompositeMsg.hpp>

#include <fwDataCamp/getObject.hpp>

#include "gui/action/ConfigActionSrvWithKeySendingConfigTemplate.hpp"

namespace gui
{
namespace action
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::gui::action::ConfigActionSrvWithKeySendingConfigTemplate, ::fwData::Object );

//------------------------------------------------------------------------------

ConfigActionSrvWithKeySendingConfigTemplate::ConfigActionSrvWithKeySendingConfigTemplate() throw() :
        m_tooltipConfigTitleKey(""),
        m_iconConfigId(""),
        m_viewConfigTitlePrefixKey (""),
        m_isUnique(false),
        m_fieldAdaptors(::fwData::Composite::New())
{
    m_closableConfig = true;
    //addNewHandledEvent( ::fwComEd::CompositeMsg::ADDED_KEYS );
    //addNewHandledEvent( ::fwComEd::CompositeMsg::REMOVED_KEYS );
}

//------------------------------------------------------------------------------

ConfigActionSrvWithKeySendingConfigTemplate::~ConfigActionSrvWithKeySendingConfigTemplate() throw()
{}

//------------------------------------------------------------------------------

void ConfigActionSrvWithKeySendingConfigTemplate::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    actionServiceStarting();

    bool executable = true;

    ::fwData::Object::sptr obj = this->getObject();
    ::fwData::Composite::sptr composite = ::fwData::Composite::dynamicCast( obj );
    if( composite )
    {
        BOOST_FOREACH( const KeyAdaptorType::value_type& elem, m_keyAdaptors )
        {
            if ( elem.second != "self" )
            {
                executable &= (composite->find(elem.second)!= composite->end());
            }
        }
    }
    else
    {
        BOOST_FOREACH( const KeyAdaptorType::value_type& elem, m_keyAdaptors )
        {
            if ( elem.second != "self" )
            {
                executable &= (obj->getFields().find(elem.second)!= obj->getFields().end());
            }
        }
    }

    this->::fwGui::IActionSrv::setIsExecutable( executable );
}

//------------------------------------------------------------------------------

void ConfigActionSrvWithKeySendingConfigTemplate::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    actionServiceStopping();
}

//------------------------------------------------------------------------------

void ConfigActionSrvWithKeySendingConfigTemplate::configuring() throw(fwTools::Failed)
{
    SLM_ASSERT("Sorry you must have one (and only one) <config id=... /> element.",
               this->getConfigTree().get_child("service").count("config") == 1 );

    const ::fwServices::IService::ConfigType srvconfig = this->getConfigTree().get_child("service");
    const ::fwServices::IService::ConfigType &config = srvconfig.get_child("config");
    const ::fwServices::IService::ConfigType &configAttr = config.get_child("<xmlattr>");

    this->::fwGui::IActionSrv::initialize();


    SLM_ASSERT( "Sorry, missing attribute 'id' in <config> xml element.", configAttr.count("id") == 1 );
    SLM_ASSERT( "Sorry, missing attribute 'title' in <config> xml element.", configAttr.count("title") == 1 );

    m_viewConfigId    = configAttr.get<std::string>("id");
    m_viewConfigTitle = configAttr.get<std::string>("title");

    m_iconConfigId             = configAttr.get<std::string>("icon", m_iconConfigId);
    m_viewConfigTitlePrefixKey = configAttr.get<std::string>("titlePrefixKey", m_viewConfigTitlePrefixKey);
    m_tooltipConfigTitleKey    = configAttr.get<std::string>("tooltipKey", m_tooltipConfigTitleKey);

    std::string unique_str = configAttr.get("unique", "false");
    OSLM_ASSERT("Wrong value for attribute 'unique': "<<unique_str,
                unique_str == ("yes") || unique_str == ("no")|| unique_str == ("true")|| unique_str == ("false"));
    m_isUnique = (unique_str == "true" || unique_str == "yes");

    m_closableConfig = configAttr.get<std::string>("closable", "yes") != "no";

    SLM_ASSERT( "Sorry, the attribute id in <config> xml element is empty.", ! m_viewConfigId.empty() );

    std::string adaptor("");
    std::string pattern("");

    BOOST_FOREACH( const ::fwServices::IService::ConfigType::value_type &v, srvconfig.equal_range("replace") )
    {
        const ::fwServices::IService::ConfigType &attr = v.second.get_child("<xmlattr>");
        adaptor = attr.get("val","");
        pattern = attr.get("pattern","");
        SLM_ASSERT("<replace> tag must have one non-empty attribute 'val'.", !adaptor.empty());
        SLM_ASSERT("<replace> tag must have one non-empty attribute 'pattern'.", !pattern.empty());
        (*m_fieldAdaptors)[pattern] = ::fwData::String::New(adaptor);
    }

    BOOST_FOREACH( const ::fwServices::IService::ConfigType::value_type &v, srvconfig.equal_range("key") )
    {
        const ::fwServices::IService::ConfigType &attr = v.second.get_child("<xmlattr>");
        adaptor = attr.get("id","");
        pattern = attr.get("pattern","");
        SLM_ASSERT("<replace> tag must have one non-empty attribute 'id'.", !adaptor.empty());
        SLM_ASSERT("<replace> tag must have one non-empty attribute 'pattern'.", !pattern.empty());
        m_keyAdaptors[pattern] = adaptor;
    }
}

//------------------------------------------------------------------------------

void ConfigActionSrvWithKeySendingConfigTemplate::updating() throw(::fwTools::Failed)
{
    this->sendConfig();
}

//------------------------------------------------------------------------------

void ConfigActionSrvWithKeySendingConfigTemplate::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    bool executable = true;

    ::fwData::Object::sptr obj = this->getObject();
    ::fwData::Composite::sptr composite = ::fwData::Composite::dynamicCast( obj );
    if( composite )
    {
        if ( _msg->hasEvent( ::fwComEd::CompositeMsg::ADDED_KEYS ) ||
             _msg->hasEvent( ::fwComEd::CompositeMsg::REMOVED_KEYS )  )
        {
            BOOST_FOREACH( const KeyAdaptorType::value_type& elem, m_keyAdaptors )
            {
                if ( elem.second != "self" )
                {
                    executable &= (composite->find(elem.second)!= composite->end());
                }
            }
            this->::fwGui::IActionSrv::setIsExecutable( executable );
        }
    }
    else
    {
        if ( _msg->hasEvent( ::fwServices::ObjectMsg::ADDED_FIELDS ) ||
             _msg->hasEvent( ::fwServices::ObjectMsg::REMOVED_FIELDS ) )
        {
            BOOST_FOREACH( const KeyAdaptorType::value_type& elem, m_keyAdaptors )
            {
                if ( elem.second != "self" )
                {
                    executable &= (composite->find(elem.second)!= composite->end());
                }
            }
            this->::fwGui::IActionSrv::setIsExecutable( executable );
        }
    }
}

//------------------------------------------------------------------------------

void ConfigActionSrvWithKeySendingConfigTemplate::info( std::ostream &_sstream )
{}

//------------------------------------------------------------------------------

void ConfigActionSrvWithKeySendingConfigTemplate::sendConfig()
{
    //AddGenericUidToFieldApadtor();
    // Generate generic UID
    std::string genericUidAdaptor = ::fwServices::registry::AppConfig::getUniqueIdentifier( this->getID(), true);
    // Init manager
    (*m_fieldAdaptors)["GENERIC_UID"] = ::fwData::String::New(genericUidAdaptor);


    ::fwData::Composite::sptr finalMap;
    finalMap = ::fwData::Object::copy( m_fieldAdaptors );

    ::fwData::Object::sptr obj = this->getObject();
    ::fwData::Composite::sptr composite = ::fwData::Composite::dynamicCast( obj );
    if( composite )
    {
        BOOST_FOREACH( const KeyAdaptorType::value_type& elem, m_keyAdaptors )
        {
            std::string key = elem.second;
            if ( key == "self" )
            {
                std::string fwID = composite->getID();
                (*finalMap)[elem.first] = ::fwData::String::New(fwID);
            }
            else
            {
                std::string fwID = (*composite)[key]->getID() ;
                (*finalMap)[elem.first] = ::fwData::String::New(fwID);
            }
        }
    }
    else
    {
        BOOST_FOREACH( const KeyAdaptorType::value_type& elem, m_keyAdaptors )
        {
            std::string key = elem.second;
            if ( key == "self" )
            {
                std::string fwID = obj->getID();
                (*finalMap)[elem.first] = ::fwData::String::New(fwID);
            }
            else
            {
                std::string fwID = obj->getField(key)->getID() ;
                (*finalMap)[elem.first] = ::fwData::String::New(fwID);
            }
        }
    }

    // Init manager
    //::fwRuntime::ConfigurationElement::csptr config =
    //        ::fwServices::registry::AppConfig::getDefault()->getAdaptedTemplateConfig( m_viewConfigId, finalMap );
    //::fwServices::AppConfigManager::sptr configTemplateManager = ::fwServices::AppConfigManager::New();
    //configTemplateManager->setConfig( config );


    std::string fieldID         = "::fwServices::registry::AppConfig";
    std::string viewConfigID    = "viewConfigID";
    std::string closableFieldID = "closable";
    std::string iconFieldID     = "icon";
    std::string tooltipFieldID  = "tooltip";
    std::string tabIDFieldID    = "tabID";
    std::string tabID;
    if(m_isUnique)
    {
        tabID = "TABID";
        if(composite)
        {
            BOOST_FOREACH( const KeyAdaptorType::value_type& elem, m_keyAdaptors )
            {
                std::string key = elem.second;
                if ( key != "self" )
                {
                    tabID += "_" + (*composite)[key]->getID();
                }
            }
        }
        else
        {
            BOOST_FOREACH( const KeyAdaptorType::value_type& elem, m_keyAdaptors )
            {
                std::string key = elem.second;
                if ( key != "self" )
                {
                    tabID += "_" + obj->getField(key)->getID();
                }
            }
        }
    }
    else
    {
        tabID = "TABID_" + ::fwTools::UUID::generateUUID();
    }
    ::fwServices::ObjectMsg::sptr  msg  = ::fwServices::ObjectMsg::New();
    ::fwData::String::NewSptr title;

    std::stringstream ss;
    if (  ! m_viewConfigTitlePrefixKey.empty() )
    {
        ::fwData::String::sptr prefix = ::fwDataCamp::getObject< ::fwData::String >( obj, m_viewConfigTitlePrefixKey );
        if(prefix)
        {
            ss << prefix->getValue() << " - " ;
        }
        ss << m_viewConfigTitle;
    }
    else
    {
        ss << m_viewConfigTitle;
    }
    if ( ! m_tooltipConfigTitleKey.empty() )
    {
        ::fwData::String::sptr tooltip = ::fwDataCamp::getObject< ::fwData::String >( obj, m_tooltipConfigTitleKey );
        if(tooltip)
        {
            title->setField( tooltipFieldID, tooltip );
        }
    }

    title->value() = ss.str();
    msg->addEvent( "NEW_CONFIGURATION_HELPER", title );
    title->setField( fieldID , finalMap );
    title->setField( viewConfigID, ::fwData::String::New(m_viewConfigId) );
    title->setField( closableFieldID, ::fwData::Boolean::New(m_closableConfig));
    title->setField( tabIDFieldID, ::fwData::String::New(tabID));
    title->setField( iconFieldID, ::fwData::String::New(m_iconConfigId) );

    ::fwServices::IEditionService::notify(this->getSptr(), composite, msg);
}

//------------------------------------------------------------------------------

}
}

