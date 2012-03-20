/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/Base.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <fwTools/fwID.hpp>
#include <fwData/Composite.hpp>
#include <fwComEd/CompositeMsg.hpp>

#include "gui/action/ConfigActionSrvWithKey.hpp"

namespace gui
{
namespace action
{

//------------------------------------------------------------------------------

REGISTER_SERVICE( ::fwGui::IActionSrv, ::gui::action::ConfigActionSrvWithKey, ::fwData::Composite );

//------------------------------------------------------------------------------

ConfigActionSrvWithKey::ConfigActionSrvWithKey() throw()
{
    addNewHandledEvent( ::fwComEd::CompositeMsg::ADDED_KEYS );
    addNewHandledEvent( ::fwComEd::CompositeMsg::REMOVED_KEYS );
}

//------------------------------------------------------------------------------

ConfigActionSrvWithKey::~ConfigActionSrvWithKey() throw()
{}

//------------------------------------------------------------------------------

void ConfigActionSrvWithKey::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    this->::gui::action::ConfigActionSrv::starting();
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

void ConfigActionSrvWithKey::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->::gui::action::ConfigActionSrv::stopping();
}

//------------------------------------------------------------------------------

void ConfigActionSrvWithKey::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->::gui::action::ConfigActionSrv::configuring();

    std::vector < ConfigurationType > keyTagsConfig = m_configuration->find("key");
    SLM_ASSERT("::gui::action::ConfigActionSrv must have at least  one tag <key>", !keyTagsConfig.empty());
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

void ConfigActionSrvWithKey::updating() throw(::fwTools::Failed)
{
    this->::gui::action::ConfigActionSrv::updating();
}

//------------------------------------------------------------------------------

void ConfigActionSrvWithKey::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    this->::gui::action::ConfigActionSrv::updating(_msg);
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    bool executable = true;
    std::map< std::string, std::string >::const_iterator itr;
    ::fwComEd::CompositeMsg::csptr compositeMsg = ::fwComEd::CompositeMsg::dynamicConstCast (_msg);
    if(compositeMsg)
    {
        if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::ADDED_KEYS ) )
        {
            for(itr = m_keyAdaptors.begin(); itr != m_keyAdaptors.end(); ++itr)
            {
                executable &= (composite->find(itr->second)!= composite->end());
            }
        }
        if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::REMOVED_KEYS ) )
        {
            for(itr = m_keyAdaptors.begin(); itr != m_keyAdaptors.end(); ++itr)
            {
                executable &= (composite->find(itr->second)!= composite->end());
            }
        }
        this->::fwGui::IActionSrv::setIsExecutable( executable );
    }
}

//------------------------------------------------------------------------------

void ConfigActionSrvWithKey::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void ConfigActionSrvWithKey::startConfig()
{
    AddGenericUidToFieldApadtor();
    std::map< std::string, std::string > finalMap;
    finalMap = m_fieldAdaptors;
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    std::map< std::string, std::string >::const_iterator itr;
    for(itr = m_keyAdaptors.begin(); itr != m_keyAdaptors.end(); ++itr)
    {
        std::string key = itr->second;
        std::string fwID = (*composite)[key]->getID() ;
        finalMap[itr->first] = fwID;
    }

    // Init manager
    ::fwRuntime::ConfigurationElement::csptr config =
            ::fwServices::registry::AppConfig::getDefault()->getAdaptedTemplateConfig( m_viewConfigId, finalMap );
    m_configTemplateManager = ::fwServices::AppConfigManager::New();
    m_configTemplateManager->setConfig( config );

    // Launch config
    m_configTemplateManager->launch();

    // Add com channel
    ::fwServices::registerCommunicationChannel( m_configTemplateManager->getConfigRoot(), this->getSptr() )->start();

    m_configIsRunning = true;

}

//------------------------------------------------------------------------------

void ConfigActionSrvWithKey::stopConfig()
{
    if( m_configIsRunning )
    {
        // Remove com channel
        ::fwServices::unregisterCommunicationChannel( m_configTemplateManager->getConfigRoot(), this->getSptr() );

        // Delete manager
        m_configTemplateManager->stopAndDestroy();
        m_configTemplateManager.reset();
    }
    m_configIsRunning = false;
}

//------------------------------------------------------------------------------

}
}

