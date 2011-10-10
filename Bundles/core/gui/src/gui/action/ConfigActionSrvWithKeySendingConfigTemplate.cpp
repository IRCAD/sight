/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <set>
#include <boost/assign/list_of.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwTools/UUID.hpp>
#include <fwTools/fwID.hpp>
#include <fwData/Composite.hpp>
#include <fwData/String.hpp>
#include <fwData/Boolean.hpp>
#include <fwComEd/CompositeMsg.hpp>


#include "gui/action/ConfigActionSrvWithKeySendingConfigTemplate.hpp"

namespace gui
{
namespace action
{

//------------------------------------------------------------------------------

REGISTER_SERVICE( ::fwGui::IActionSrv, ::gui::action::ConfigActionSrvWithKeySendingConfigTemplate, ::fwData::Composite );

//------------------------------------------------------------------------------

ConfigActionSrvWithKeySendingConfigTemplate::ConfigActionSrvWithKeySendingConfigTemplate() throw() :
        m_fieldAdaptors ( ::fwData::Composite::New() ),
        m_viewConfigTitlePrefixKey (""),
        m_iconConfigId(""),
        m_tooltipConfigTitleKey(""),
        m_isUnique(false)
{
    m_closableConfig = true;
    addNewHandledEvent( ::fwComEd::CompositeMsg::ADDED_FIELDS );
    addNewHandledEvent( ::fwComEd::CompositeMsg::REMOVED_FIELDS );
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
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    std::map< std::string, std::string >::const_iterator itr;
    for(itr = m_keyAdaptors.begin(); itr != m_keyAdaptors.end(); ++itr)
    {
        executable &= (composite->find(itr->second)!= composite->end());
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
    SLM_TRACE_FUNC();

    this->::fwGui::IActionSrv::initialize();

    std::vector < ConfigurationType > vectConfig = m_configuration->find("config");

    SLM_ASSERT("Sorry you must have one (and only one) <config id=... /> element.", vectConfig.size() == 1 );
    ::fwRuntime::ConfigurationElement::sptr configElement = vectConfig.at(0);

    SLM_ASSERT( "Sorry, missing attribute id in <config> xml element.", configElement->hasAttribute("id") );
    m_viewConfigId = configElement->getExistingAttributeValue("id");

    SLM_ASSERT( "Sorry, missing attribute title in <config> xml element.", configElement->hasAttribute("title") );
    m_viewConfigTitle = configElement->getExistingAttributeValue("title");

    if(configElement->hasAttribute("icon"))
    {
        m_iconConfigId = configElement->getExistingAttributeValue("icon");
    }
    if( configElement->hasAttribute("titlePrefixKey") )
    {
        m_viewConfigTitlePrefixKey = configElement->getExistingAttributeValue("titlePrefixKey");
    }
    if( configElement->hasAttribute("tooltipKey") )
    {
        m_tooltipConfigTitleKey = configElement->getExistingAttributeValue("tooltipKey");
    }
    if( configElement->hasAttribute("unique") )
    {
        std::string unique_str = configElement->getExistingAttributeValue("unique");
        m_isUnique = (unique_str == "true" || unique_str == "yes");
        const std::set< std::string> uniqueMapValue = ::boost::assign::list_of("yes")("no")("true")("false");
        OSLM_ASSERT("Wrong value for attribute 'unique': "<<unique_str, uniqueMapValue.find(unique_str) != uniqueMapValue.end());
    }

    m_closableConfig = configElement->getAttributeValue("closable") != "no";

    SLM_ASSERT( "Sorry, the attribute id in <config> xml element is empty.", ! m_viewConfigId.empty() );

    std::vector < ConfigurationType > replaceTagsConfig = m_configuration->find("replace");
    std::string adaptor("");
    std::string pattern("");
    BOOST_FOREACH( ConfigurationType replaceItem, replaceTagsConfig)
    {
        SLM_ASSERT("<replace> tag must have one attribut val.", replaceItem->hasAttribute("val"));
        adaptor = replaceItem->getAttributeValue("val");
        SLM_ASSERT("<replace> tag must have one attribut pattern.", replaceItem->hasAttribute("pattern"));
        pattern = replaceItem->getAttributeValue("pattern");
        (*m_fieldAdaptors)[pattern] = ::fwData::String::New(adaptor);
    }

    std::vector < ConfigurationType > keyTagsConfig = m_configuration->find("key");
    if(!keyTagsConfig.empty())
    {
        std::string adaptor("");
        std::string pattern("");
        BOOST_FOREACH( ConfigurationType keyItem, keyTagsConfig)
        {
            SLM_ASSERT("<key> tag must have one attribut val.", keyItem->hasAttribute("id"));
            adaptor = keyItem->getAttributeValue("id");
            SLM_ASSERT("<key> tag must have one attribut pattern.", keyItem->hasAttribute("pattern"));
            pattern = keyItem->getAttributeValue("pattern");
            m_keyAdaptors[pattern] = adaptor;
        }
    }
}

//------------------------------------------------------------------------------

void ConfigActionSrvWithKeySendingConfigTemplate::updating() throw(::fwTools::Failed)
{
    this->sendConfig();
}

//------------------------------------------------------------------------------

void ConfigActionSrvWithKeySendingConfigTemplate::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    //this->::gui::action::ConfigActionSrv::updating(_msg);
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    bool executable = true;
    std::map< std::string, std::string >::const_iterator itr;
    ::fwComEd::CompositeMsg::csptr compositeMsg = ::fwComEd::CompositeMsg::dynamicConstCast (_msg);
    if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::ADDED_FIELDS ) )
    {
        for(itr = m_keyAdaptors.begin(); itr != m_keyAdaptors.end(); ++itr)
        {
            executable &= (composite->find(itr->second)!= composite->end());
        }
    }
    if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::REMOVED_FIELDS ) )
    {
        for(itr = m_keyAdaptors.begin(); itr != m_keyAdaptors.end(); ++itr)
        {
            executable &= (composite->find(itr->second)!= composite->end());
        }
    }
    this->::fwGui::IActionSrv::setIsExecutable( executable );
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


    ::fwData::Composite::NewSptr finalMap;
    finalMap->deepCopy( m_fieldAdaptors );
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    std::map< std::string, std::string >::const_iterator itr;
    for(itr = m_keyAdaptors.begin(); itr != m_keyAdaptors.end(); ++itr)
    {
        std::string key = itr->second;
        if ( key == "self" )
        {
            std::string fwID = composite->getID();
            (*finalMap)[itr->first] = ::fwData::String::New(fwID);
        }
        else
        {
            std::string fwID = (*composite)[key]->getID() ;
            (*finalMap)[itr->first] = ::fwData::String::New(fwID);
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
        std::map< std::string, std::string >::const_iterator itr;
        for(itr = m_keyAdaptors.begin(); itr != m_keyAdaptors.end(); ++itr)
        {
            ::fwData::Composite::ContainerType::iterator key = composite->find(itr->second);
            OSLM_ASSERT("Missing key "<<itr->second<<" in composite", key!= composite->end());
            ::fwTools::Object::sptr obj = key->second;
            tabID += "_" + obj->getID();
        }
    }
    else
    {
        tabID = "TABID_" + ::fwTools::UUID::generateUUID();
    }
    ::fwServices::ObjectMsg::sptr  msg  = ::fwServices::ObjectMsg::New();

    std::stringstream ss;
    if (    ! m_viewConfigTitlePrefixKey.empty() &&
            composite->find( m_viewConfigTitlePrefixKey ) != composite->end() )
    {
        ::fwData::String::sptr prefix = ::fwData::String::dynamicCast( (*composite)[m_viewConfigTitlePrefixKey] );
        ss << prefix->getValue() << " - " << m_viewConfigTitle;
    }
    else
    {
        ss << m_viewConfigTitle;
    }
    if ( ! m_tooltipConfigTitleKey.empty() &&
            composite->find( m_tooltipConfigTitleKey ) != composite->end() )
    {
        ::fwData::String::sptr tooltip = ::fwData::String::dynamicCast( (*composite)[m_tooltipConfigTitleKey] );
        msg->setFieldSingleElement( tooltipFieldID, tooltip );
    }

    ::fwData::String::NewSptr title;
    title->value() = ss.str();
    msg->addEvent( "NEW_CONFIGURATION_HELPER", title );
    msg->setFieldSingleElement( fieldID , finalMap );
    msg->setFieldSingleElement( viewConfigID, ::fwData::String::New(m_viewConfigId) );
    msg->setFieldSingleElement( closableFieldID, ::fwData::Boolean::New(m_closableConfig));
    msg->setFieldSingleElement( tabIDFieldID, ::fwData::String::New(tabID));
    msg->setFieldSingleElement( iconFieldID, ::fwData::String::New(m_iconConfigId) );

    ::fwServices::IEditionService::notify(this->getSptr(), composite, msg);
}

//------------------------------------------------------------------------------

}
}

