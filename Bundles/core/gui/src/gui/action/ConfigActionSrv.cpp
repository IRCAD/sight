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
    m_viewConfigId("")
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

    // Adpat generic configuration
    m_createdConfiguration = this->adaptConfig( this->getServiceObjectConfig() );
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
    m_object = ::fwServices::New( m_createdConfiguration );
    ::fwServices::registerCommunicationChannel(m_object, this->getSptr())->start();
    ::fwServices::start( m_createdConfiguration ) ;
    ::fwServices::update( m_createdConfiguration ) ;
}

//------------------------------------------------------------------------------

void ConfigActionSrv::stopConfig()
{
    SLM_ASSERT("m_object not exist (ConfigActionSrv is in 'style=check' in config?)", m_object);
    ::fwServices::unregisterCommunicationChannel(m_object, this->getSptr());
    ::fwServices::stopAndUnregister( m_createdConfiguration ) ;
    m_object.reset();
}

//------------------------------------------------------------------------------

std::string ConfigActionSrv::completeValue( std::string _str )
{
    std::string newStr = _str;

    ::boost::regex uuidRegex ("(.*)GENERIC_UID(.*)");
    std::stringstream machine_format;
    machine_format << "\\1" << this->getUUID() << "\\2";

    if ( ::boost::regex_match( _str, uuidRegex ) )
    {
        newStr = ::boost::regex_replace( _str, uuidRegex, machine_format.str(), ::boost::match_default | ::boost::format_sed );
    }

    return newStr;
}

//------------------------------------------------------------------------------

::fwRuntime::EConfigurationElement::sptr ConfigActionSrv::adaptConfig( ::fwRuntime::ConfigurationElement::sptr _cfgElem )
{
    SLM_TRACE_FUNC();

    ::fwRuntime::EConfigurationElement::NewSptr result ( _cfgElem->getName() );
    result->setValue( this->completeValue( _cfgElem->getValue() ) );

    typedef std::map<std::string, std::string> MapAttributesType;
    BOOST_FOREACH( MapAttributesType::value_type attribute, _cfgElem->getAttributes() )
    {
        result->setAttributeValue( attribute.first, this->completeValue( attribute.second ) );
    }

    BOOST_FOREACH ( ::fwRuntime::ConfigurationElement::sptr subElem, _cfgElem->getElements())
    {
        result->addConfigurationElement( this->adaptConfig( subElem ) );
    }

    return result;
}
//------------------------------------------------------------------------------

::fwRuntime::ConfigurationElement::sptr ConfigActionSrv::getServiceObjectConfig()
{
    ::fwRuntime::ConfigurationElement::sptr sgConfig = ::fwServices::bundle::findConfigurationForPoint(m_viewConfigId, "::fwServices::ServiceObjectConfig");
    return sgConfig;
}

//------------------------------------------------------------------------------

}
}

