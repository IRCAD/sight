#include <fwServices/helper.hpp>
#include <fwServices/bundle/runtime.hpp>
#include <fwTools/fwID.hpp>
#include <fwData/Composite.hpp>

#include "gui/action/ConfigActionSrvWithKey.hpp"

namespace gui
{
namespace action
{

//------------------------------------------------------------------------------

REGISTER_SERVICE( ::fwGui::IActionSrv, ::gui::action::ConfigActionSrvWithKey, ::fwTools::Object );

//------------------------------------------------------------------------------

ConfigActionSrvWithKey::ConfigActionSrvWithKey() throw()
{

}

//------------------------------------------------------------------------------

ConfigActionSrvWithKey::~ConfigActionSrvWithKey() throw()
{}

//------------------------------------------------------------------------------

void ConfigActionSrvWithKey::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    this->::gui::action::ConfigActionSrv::starting();

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
}

//------------------------------------------------------------------------------

void ConfigActionSrvWithKey::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void ConfigActionSrvWithKey::startConfig()
{
    std::map< std::string, std::string > finalMap;
    finalMap = m_fieldAdaptors;
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    std::map< std::string, std::string >::const_iterator itr;
    for(itr = m_keyAdaptors.begin(); itr != m_keyAdaptors.end(); ++itr)
    {
        for(    ::fwData::Composite::Container::iterator  objectId = composite->getRefMap().begin();
                objectId != composite->getRefMap().end();
                ++objectId )
        {
            if(itr->first == objectId->first)
            {
                finalMap[itr->second] = itr->second;
            }
        }
    }
    // Init manager
    m_configTemplateManager = ::fwServices::ConfigTemplateManager::New();
    m_configTemplateManager->setConfig( m_viewConfigId, "::fwServices::ServiceObjectConfig" );
    m_configTemplateManager->setFieldAdaptors( finalMap );

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

