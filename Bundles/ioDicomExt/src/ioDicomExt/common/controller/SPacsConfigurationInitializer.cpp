/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwServices/macros.hpp>

#include "ioDicomExt/common/controller/SPacsConfigurationInitializer.hpp"

namespace ioDicomExt
{

namespace common
{

namespace controller
{

fwServicesRegisterMacro( ::fwServices::IController , ::ioDicomExt::common::controller::SPacsConfigurationInitializer ,
        ::fwDicomIOExt::data::PacsConfiguration ) ;

//------------------------------------------------------------------------------

SPacsConfigurationInitializer::SPacsConfigurationInitializer() throw()
{
}
//------------------------------------------------------------------------------

SPacsConfigurationInitializer::~SPacsConfigurationInitializer() throw()
{
}

//------------------------------------------------------------------------------

void SPacsConfigurationInitializer::info(std::ostream &_sstream )
{
    _sstream << "SPacsConfigurationInitializer::info" ;
}

//------------------------------------------------------------------------------

void SPacsConfigurationInitializer::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SPacsConfigurationInitializer::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SPacsConfigurationInitializer::configuring() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("config");
    SLM_ASSERT("The service ::ioDicomExt::common::controller::SPacsConfigurationInitializer must have a \"config\" element.",
            config);

    bool success;

    /// Local application title
    ::boost::tie(success, m_localApplicationTitle) = config->getSafeAttributeValue("localApplicationTitle");
    SLM_ASSERT("It should be a \"localApplicationTitle\" tag in the "
            "::ioDicomExt::common::controller::SPacsConfigurationInitializer config element.", success);

    /// Pacs host name
    ::boost::tie(success, m_pacsHostName) = config->getSafeAttributeValue("pacsHostName");
    SLM_ASSERT("It should be a \"pacsHostName\" tag in the "
            "::ioDicomExt::common::controller::SPacsConfigurationInitializer config element.", success);

    /// Pacs application title
    ::boost::tie(success, m_pacsApplicationTitle) = config->getSafeAttributeValue("pacsApplicationTitle");
    SLM_ASSERT("It should be a \"pacsApplicationTitle\" tag in the "
            "::ioDicomExt::common::controller::SPacsConfigurationInitializer config element.", success);

    /// Pacs port
    std::string pacsApplicationPort;
    ::boost::tie(success, pacsApplicationPort) = config->getSafeAttributeValue("pacsApplicationPort");
    SLM_ASSERT("It should be a \"pacsApplicationPort\" tag in the "
            "::ioDicomExt::common::controller::SPacsConfigurationInitializer config element.", success);
    m_pacsApplicationPort = ::boost::lexical_cast< int >(pacsApplicationPort.c_str());

    /// Move application title
    ::boost::tie(success, m_moveApplicationTitle) = config->getSafeAttributeValue("moveApplicationTitle");
    SLM_ASSERT("It should be a \"moveApplicationTitle\" tag in the "
            "::ioDicomExt::common::controller::SPacsConfigurationInitializer config element.", success);

    /// Move application port
    std::string moveApplicationPort;
    ::boost::tie(success, moveApplicationPort) = config->getSafeAttributeValue("moveApplicationPort");
    SLM_ASSERT("It should be a \"moveApplicationPort\" tag in the "
            "::ioDicomExt::common::controller::SPacsConfigurationInitializer config element.", success);
    m_moveApplicationPort = ::boost::lexical_cast< int >(moveApplicationPort.c_str());

    /// Retrieve Method
    std::string retrieveMethod;
    ::boost::tie(success, retrieveMethod) = config->getSafeAttributeValue("retrieveMethod");
    SLM_ASSERT("It should be a \"retrieveMethod\" tag in the "
            "::ioDicomExt::common::controller::SPacsConfigurationInitializer config element.", success);
    m_retrieveMethod = (retrieveMethod == "GET")?
            (::fwDicomIOExt::data::PacsConfiguration::GET_RETRIEVE_METHOD):
            (::fwDicomIOExt::data::PacsConfiguration::MOVE_RETRIEVE_METHOD);


}

//------------------------------------------------------------------------------

void SPacsConfigurationInitializer::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SPacsConfigurationInitializer::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwDicomIOExt::data::PacsConfiguration::sptr pacsConfiguration = this->getObject< ::fwDicomIOExt::data::PacsConfiguration >();

    if(pacsConfiguration)
    {
        pacsConfiguration->setLocalApplicationTitle(m_localApplicationTitle);
        pacsConfiguration->setPacsHostName(m_pacsHostName);
        pacsConfiguration->setPacsApplicationTitle(m_pacsApplicationTitle);
        pacsConfiguration->setPacsApplicationPort(m_pacsApplicationPort);
        pacsConfiguration->setMoveApplicationTitle(m_moveApplicationTitle);
        pacsConfiguration->setMoveApplicationPort(m_moveApplicationPort);
        pacsConfiguration->setRetrieveMethod(m_retrieveMethod);
    }

}



} // namespace controller
} // namespace common
} // namespace ioDicomExt
