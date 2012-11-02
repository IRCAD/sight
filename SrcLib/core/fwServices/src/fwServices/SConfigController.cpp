/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/Base.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <fwTools/fwID.hpp>
#include <fwData/Composite.hpp>

#include "fwServices/SConfigController.hpp"

namespace fwServices
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwServices::IController, ::fwServices::SConfigController, ::fwData::Object );

//------------------------------------------------------------------------------

const std::string SConfigController::s_SELF_KEY = "self";

//------------------------------------------------------------------------------

SConfigController::SConfigController() throw()
{}

//------------------------------------------------------------------------------

SConfigController::~SConfigController() throw()
{}

//------------------------------------------------------------------------------

void SConfigController::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->startConfig();
}

//------------------------------------------------------------------------------

void SConfigController::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->stopConfig();
}

//------------------------------------------------------------------------------

void SConfigController::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    std::vector < ConfigurationType > vectConfig = m_configuration->find("config");

    SLM_ASSERT("Sorry you must have one (and only one) <config id=... /> element.", vectConfig.size() == 1 );
    ::fwRuntime::ConfigurationElement::sptr configElement = vectConfig.at(0);

    SLM_ASSERT( "Sorry, missing attribute id in <config> xml element.", configElement->hasAttribute("id") );
    m_viewConfigId = configElement->getExistingAttributeValue("id");

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
        m_fieldAdaptors[pattern] = adaptor;
    }

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

void SConfigController::updating() throw(::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void SConfigController::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void SConfigController::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void SConfigController::addGenericUidToFieldApadtor( )
{
    // Generate generic UID
    std::string genericUidAdaptor = ::fwServices::registry::AppConfig::getUniqueIdentifier( this->getID() );

    // Init manager
    m_fieldAdaptors["GENERIC_UID"] = genericUidAdaptor;
}

//------------------------------------------------------------------------------

void SConfigController::startConfig()
{
    this->addGenericUidToFieldApadtor();
    std::map< std::string, std::string > finalMap;
    finalMap = m_fieldAdaptors;

    ::fwData::Object::sptr object = this->getObject();
    ::fwData::Composite::sptr composite = ::fwData::Composite::dynamicCast( object );
    if( composite )
    {
        std::map< std::string, std::string >::const_iterator itr;
        for(itr = m_keyAdaptors.begin(); itr != m_keyAdaptors.end(); ++itr)
        {
            std::string key = itr->second;
            std::string fwID;
            if( key != s_SELF_KEY )
            {
                fwID = (*composite)[key]->getID() ;
            }
            else
            {
                fwID = composite->getID() ;
            }
            finalMap[itr->first] = fwID;
        }
    }
    else
    {
        std::map< std::string, std::string >::const_iterator itr;
        for(itr = m_keyAdaptors.begin(); itr != m_keyAdaptors.end(); ++itr)
        {
            std::string key = itr->second;
            std::string fwID;
            if( key != s_SELF_KEY )
            {
                fwID = object->getField(key)->getID() ;
            }
            else
            {
                fwID = object->getID() ;
            }
            finalMap[itr->first] = fwID;
        }
    }

    // Init manager
    ::fwRuntime::ConfigurationElement::csptr config =
            ::fwServices::registry::AppConfig::getDefault()->getAdaptedTemplateConfig( m_viewConfigId, finalMap );
    m_configTemplateManager = ::fwServices::AppConfigManager::New();
    m_configTemplateManager->setConfig( config );

    // Launch config
    m_configTemplateManager->launch();
}

//------------------------------------------------------------------------------

void SConfigController::stopConfig()
{
    // Delete manager
    m_configTemplateManager->stopAndDestroy();
    m_configTemplateManager.reset();
}

//------------------------------------------------------------------------------

}

