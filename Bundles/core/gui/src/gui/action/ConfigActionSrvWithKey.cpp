/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
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

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::gui::action::ConfigActionSrvWithKey, ::fwData::Object );

//------------------------------------------------------------------------------

ConfigActionSrvWithKey::ConfigActionSrvWithKey() throw()
{
    //addNewHandledEvent( ::fwComEd::CompositeMsg::ADDED_KEYS );
    //addNewHandledEvent( ::fwComEd::CompositeMsg::REMOVED_KEYS );
}

//------------------------------------------------------------------------------

ConfigActionSrvWithKey::~ConfigActionSrvWithKey() throw()
{
}

//------------------------------------------------------------------------------

void ConfigActionSrvWithKey::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    this->::gui::action::ConfigActionSrv::starting();

    bool executable = true;

    ::fwData::Object::sptr obj          = this->getObject();
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
            pattern                = keyItem->getAttributeValue("pattern");
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

void ConfigActionSrvWithKey::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    this->::gui::action::ConfigActionSrv::receiving(_msg);

    bool executable = true;

    ::fwData::Object::sptr obj          = this->getObject();
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

void ConfigActionSrvWithKey::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void ConfigActionSrvWithKey::startConfig()
{
    AddGenericUidToFieldApadtor();
    std::map< std::string, std::string > finalMap;
    finalMap = m_fieldAdaptors;

    ::fwData::Object::sptr obj          = this->getObject();
    ::fwData::Composite::sptr composite = ::fwData::Composite::dynamicCast( obj );
    if( composite )
    {
        BOOST_FOREACH( const KeyAdaptorType::value_type& elem, m_keyAdaptors )
        {
            std::string key = elem.second;
            if ( key == "self" )
            {
                finalMap[elem.first] = composite->getID();
            }
            else
            {
                finalMap[elem.first] = (*composite)[key]->getID();
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
                finalMap[elem.first] = obj->getID();
            }
            else
            {
                finalMap[elem.first] = obj->getField(key)->getID();
            }
        }
    }

    // Init manager
    ::fwRuntime::ConfigurationElement::csptr config =
        ::fwServices::registry::AppConfig::getDefault()->getAdaptedTemplateConfig( m_viewConfigId, finalMap );
    m_configTemplateManager = ::fwServices::AppConfigManager::New();
    m_configTemplateManager->setConfig( config );

    // Launch config
    m_configTemplateManager->launch();

    // Add connection
    this->connectToConfigRoot();

    m_configIsRunning = true;

}

//------------------------------------------------------------------------------

void ConfigActionSrvWithKey::stopConfig()
{
    if( m_configIsRunning )
    {
        // Remove com channel
        this->disconnectToConfigRoot();

        // Delete manager
        m_configTemplateManager->stopAndDestroy();
        m_configTemplateManager.reset();
    }
    m_configIsRunning = false;
}

//------------------------------------------------------------------------------

}
}

