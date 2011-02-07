#include <fwServices/helper.hpp>
#include <fwServices/bundle/runtime.hpp>
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

ConfigActionSrvWithKeySendingConfigTemplate::ConfigActionSrvWithKeySendingConfigTemplate() throw()
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
    std::string genericUidAdaptor = ::fwServices::ConfigTemplateManager::getUniqueIdentifier( this->getID(), true);
    // Init manager
    m_fieldAdaptors["GENERIC_UID"] = genericUidAdaptor;


    std::map< std::string, std::string > finalMap;
    finalMap = m_fieldAdaptors;
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    std::map< std::string, std::string >::const_iterator itr;
    for(itr = m_keyAdaptors.begin(); itr != m_keyAdaptors.end(); ++itr)
    {
        std::string key = itr->second;
        if ( key == "self" )
        {
            std::string fwID = composite->getID();
            finalMap[itr->first] = fwID;
        }
        else
        {
            std::string fwID = (*composite)[key]->getID() ;
            finalMap[itr->first] = fwID;
        }
    }

    // Init manager
    ::fwServices::ConfigTemplateManager::sptr configTemplateManager;
    configTemplateManager = ::fwServices::ConfigTemplateManager::New();
    configTemplateManager->setConfig( m_viewConfigId, "::fwServices::ServiceObjectConfig" );
    configTemplateManager->setFieldAdaptors( finalMap );


    std::string fieldID = "::fwServices::ServiceObjectConfig";
    std::string closableFieldID = "closable";

    ::fwServices::ObjectMsg::sptr  msg  = ::fwServices::ObjectMsg::New();

    ::fwData::String::NewSptr title;
    title->value() = m_viewConfigTitle;

    msg->addEvent( "NEW_CONFIGURATION_HELPER", title );
    msg->setFieldSingleElement( fieldID ,configTemplateManager);
    msg->setFieldSingleElement( closableFieldID , ::fwData::Boolean::New(m_closableConfig));


    ::fwServices::IEditionService::notify(this->getSptr(), composite, msg);
}

//------------------------------------------------------------------------------

}
}

