#include <fwServices/Base.hpp>
#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/IEditionService.hpp>

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
        m_viewConfigtitlePrefixKey ("")
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

    if( configElement->hasAttribute("titlePrefixKey") )
    {
        m_viewConfigtitlePrefixKey = configElement->getExistingAttributeValue("titlePrefixKey");
    }

    m_closableConfig = configElement->getAttributeValue("closable") != "no";

    SLM_ASSERT( "Sorry, the attribute id in <config> xml element is empty.", ! m_viewConfigId.empty() );

    std::vector < ConfigurationType > replaceTagsConfig = m_configuration->find("replace");
    //    SLM_ASSERT("::gui::action::ConfigActionSrv must have at least  one tag <replace>", !replaceTagsConfig.empty());
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

void ConfigActionSrvWithKeySendingConfigTemplate::updating() throw(::fwTools::Failed)
{
    //this->::gui::action::ConfigActionSrv::updating();
    sendConfig();
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
{
}

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


    std::string fieldID = "::fwServices::registry::AppConfig";
    std::string viewConfigID = "viewConfigID";
    std::string closableFieldID = "closable";

    ::fwServices::ObjectMsg::sptr  msg  = ::fwServices::ObjectMsg::New();


    std::stringstream ss;
    if (    ! m_viewConfigtitlePrefixKey.empty() &&
            composite->find( m_viewConfigtitlePrefixKey ) != composite->end() )
    {
        ::fwData::String::sptr prefix = ::fwData::String::dynamicCast( (*composite)[m_viewConfigtitlePrefixKey] );
        ss << prefix->getValue() << " - " << m_viewConfigTitle;
    }
    else
    {
        ss << m_viewConfigTitle;
    }
    ::fwData::String::NewSptr title;
    title->value() = ss.str();

    msg->addEvent( "NEW_CONFIGURATION_HELPER", title );
    msg->setFieldSingleElement( fieldID , finalMap );
    msg->setFieldSingleElement( viewConfigID , ::fwData::String::New(m_viewConfigId) );
    msg->setFieldSingleElement( closableFieldID , ::fwData::Boolean::New(m_closableConfig));


    ::fwServices::IEditionService::notify(this->getSptr(), composite, msg);
}

//------------------------------------------------------------------------------

}
}

