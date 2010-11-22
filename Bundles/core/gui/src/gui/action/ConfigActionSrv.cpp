#include <boost/regex.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/bundle/runtime.hpp>

#include "gui/action/ConfigActionSrv.hpp"

namespace gui
{
namespace action
{

//------------------------------------------------------------------------------

REGISTER_SERVICE( ::fwGui::IActionSrv, ::gui::action::ConfigActionSrv, ::fwTools::Object );

//------------------------------------------------------------------------------

ConfigActionSrv::ConfigActionSrv() throw() :
    m_viewConfigId(""),
    m_configIsRunning(false)
{
    addNewHandledEvent("WINDOW_CLOSED");
}

//------------------------------------------------------------------------------

ConfigActionSrv::~ConfigActionSrv() throw()
{}

//------------------------------------------------------------------------------

void ConfigActionSrv::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void ConfigActionSrv::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    bool configIsRunning = this->::fwGui::IActionSrv::getIsActive();

    if ( configIsRunning )
    {
        this->stopConfig();
    }

    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

void ConfigActionSrv::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    this->::fwGui::IActionSrv::initialize();

    std::vector < ConfigurationType > vectConfig = m_configuration->find("config");

    SLM_ASSERT("Sorry you must have one (and only one) <config id=... /> element.", vectConfig.size() == 1 );
    ::fwRuntime::ConfigurationElement::sptr configElement = vectConfig.at(0);

    SLM_ASSERT( "Sorry, missing attribute id in <config> xml element.", configElement->hasAttribute("id") );
    m_viewConfigId = configElement->getExistingAttributeValue("id");

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
}

//------------------------------------------------------------------------------

void ConfigActionSrv::updating() throw(::fwTools::Failed)
{
    bool needRunningConfig = this->::fwGui::IActionSrv::getIsActive();

    if ( needRunningConfig )
    {
        this->startConfig();
    }
    else
    {
        this->stopConfig();
    }
}

//------------------------------------------------------------------------------

void ConfigActionSrv::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    if ( _msg->hasEvent("WINDOW_CLOSED") )
    {
        this->::fwGui::IActionSrv::setIsActive( false );
        this->stopConfig();
    }
}

//------------------------------------------------------------------------------

void ConfigActionSrv::info( std::ostream &_sstream )
{}

//------------------------------------------------------------------------------

void ConfigActionSrv::startConfig()
{
    // Generate generic UID
    std::string genericUidAdaptor = ::fwServices::ConfigTemplateManager::getUniqueIdentifier( this->getID() );

    // Init manager
    m_fieldAdaptors["GENERIC_UID"] = genericUidAdaptor;
    m_configTemplateManager = ::fwServices::ConfigTemplateManager::New();
    m_configTemplateManager->setConfig( m_viewConfigId, "::fwServices::ServiceObjectConfig" );
    m_configTemplateManager->setFieldAdaptors( m_fieldAdaptors );

    // Launch config
    m_configTemplateManager->launch();

    // Add com channel
    ::fwServices::registerCommunicationChannel( m_configTemplateManager->getConfigRoot(), this->getSptr() )->start();

    m_configIsRunning = true;
}

//------------------------------------------------------------------------------

void ConfigActionSrv::stopConfig()
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

