/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioPacs/SPacsConfigurationInitializer.hpp"

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <boost/foreach.hpp>

#include <utility>

namespace ioPacs
{

fwServicesRegisterMacro( ::fwServices::IController, ::ioPacs::SPacsConfigurationInitializer,
                         ::fwPacsIO::data::PacsConfiguration );

//------------------------------------------------------------------------------

SPacsConfigurationInitializer::SPacsConfigurationInitializer() throw()
{
}
//------------------------------------------------------------------------------

SPacsConfigurationInitializer::~SPacsConfigurationInitializer() throw()
{
}

//------------------------------------------------------------------------------

void SPacsConfigurationInitializer::info(std::ostream& _sstream )
{
    _sstream << "SPacsConfigurationInitializer::info";
}

//------------------------------------------------------------------------------

void SPacsConfigurationInitializer::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->updating();
}

//------------------------------------------------------------------------------

void SPacsConfigurationInitializer::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->updating();
}

//------------------------------------------------------------------------------

template<typename T, typename CAST_T = T>
struct SetFromConfig
{
    void operator()(::fwData::Composite::sptr config, std::string const& confName, T& attribute)
    {
        if(config)
        {
            ::fwData::Composite::iterator it = config->find(confName);
            if(it != config->end())
            {
                ::fwData::String::sptr obj = ::fwData::String::dynamicCast(it->second);
                attribute = T(::boost::lexical_cast<CAST_T>(obj->value()));
            }
        }
    }
};

//------------------------------------------------------------------------------

std::pair< ::fwServices::IService::sptr, ::fwData::Composite::sptr> getPreferences()
{
    ::fwData::Composite::sptr prefs;

    auto preferencesServicesList = ::fwServices::OSR::getServices("::fwPreferences::IPreferences");

    ::fwServices::IService::sptr prefService;
    if(!preferencesServicesList.empty())
    {
        prefService = *preferencesServicesList.begin();
    }

    if(prefService)
    {
        prefs = prefService->getObject< ::fwData::Composite>();
    }

    return std::make_pair(prefService, prefs);
}

//------------------------------------------------------------------------------

void SPacsConfigurationInitializer::configuring() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("config");
    SLM_ASSERT("The service ::ioPacs::SPacsConfigurationInitializer must have a \"config\" element.",
               config);

    bool success;

    /// Local application title
    ::boost::tie(success, m_localApplicationTitle) = config->getSafeAttributeValue("localApplicationTitle");
    SLM_ASSERT("It should be a \"localApplicationTitle\" tag in the "
               "::ioPacs::SPacsConfigurationInitializer config element.", success);

    /// Pacs host name
    ::boost::tie(success, m_pacsHostName) = config->getSafeAttributeValue("pacsHostName");
    SLM_ASSERT("It should be a \"pacsHostName\" tag in the "
               "::ioPacs::SPacsConfigurationInitializer config element.", success);

    /// Pacs application title
    ::boost::tie(success, m_pacsApplicationTitle) = config->getSafeAttributeValue("pacsApplicationTitle");
    SLM_ASSERT("It should be a \"pacsApplicationTitle\" tag in the "
               "::ioPacs::SPacsConfigurationInitializer config element.", success);

    /// Pacs port
    std::string pacsApplicationPort;
    ::boost::tie(success, pacsApplicationPort) = config->getSafeAttributeValue("pacsApplicationPort");
    SLM_ASSERT("It should be a \"pacsApplicationPort\" tag in the "
               "::ioPacs::SPacsConfigurationInitializer config element.", success);
    m_pacsApplicationPort = ::boost::lexical_cast< unsigned short >(pacsApplicationPort.c_str());

    /// Move application title
    ::boost::tie(success, m_moveApplicationTitle) = config->getSafeAttributeValue("moveApplicationTitle");
    SLM_ASSERT("It should be a \"moveApplicationTitle\" tag in the "
               "::ioPacs::SPacsConfigurationInitializer config element.", success);

    /// Move application port
    std::string moveApplicationPort;
    ::boost::tie(success, moveApplicationPort) = config->getSafeAttributeValue("moveApplicationPort");
    SLM_ASSERT("It should be a \"moveApplicationPort\" tag in the "
               "::ioPacs::SPacsConfigurationInitializer config element.", success);
    m_moveApplicationPort = ::boost::lexical_cast< unsigned short >(moveApplicationPort.c_str());

    /// Retrieve Method
    std::string retrieveMethod;
    ::boost::tie(success, retrieveMethod) = config->getSafeAttributeValue("retrieveMethod");
    SLM_ASSERT("It should be a \"retrieveMethod\" tag in the "
               "::ioPacs::SPacsConfigurationInitializer config element.", success);
    m_retrieveMethod = (retrieveMethod == "GET") ?
                       (::fwPacsIO::data::PacsConfiguration::GET_RETRIEVE_METHOD):
                       (::fwPacsIO::data::PacsConfiguration::MOVE_RETRIEVE_METHOD);

    /// Preference Key
    m_preferenceKey = config->getAttributeValue("preferenceKey");

    ::fwPacsIO::data::PacsConfiguration::sptr pacsConfiguration =
        this->getObject< ::fwPacsIO::data::PacsConfiguration >();

    // Set information from xml and update PacsConfiguration
    if(!m_preferenceKey.empty())
    {
        ::fwData::Composite::sptr prefs = getPreferences().second;
        if(prefs)
        {
            ::fwData::Composite::sptr config = ::fwData::Composite::dynamicCast((*prefs)[m_preferenceKey]);

            SetFromConfig< std::string > setFromConfig;
            SetFromConfig< unsigned short > setFromConfigShort;
            SetFromConfig< ::fwPacsIO::data::PacsConfiguration::RETRIEVE_METHOD, int > setFromConfigEnum;

            setFromConfig(config, "LocalApplicationTitle", m_localApplicationTitle);
            setFromConfig(config, "PacsHostName", m_pacsHostName);
            setFromConfig(config, "PacsApplicationTitle", m_pacsApplicationTitle);
            setFromConfigShort(config, "PacsApplicationPort", m_pacsApplicationPort);
            setFromConfig(config, "MoveApplicationTitle", m_moveApplicationTitle);
            setFromConfigShort(config, "MoveApplicationPort", m_moveApplicationPort);
            setFromConfigEnum(config, "RetrieveMethod", m_retrieveMethod);
        }
    }

    pacsConfiguration->setLocalApplicationTitle(m_localApplicationTitle);
    pacsConfiguration->setPacsHostName(m_pacsHostName);
    pacsConfiguration->setPacsApplicationTitle(m_pacsApplicationTitle);
    pacsConfiguration->setPacsApplicationPort(m_pacsApplicationPort);
    pacsConfiguration->setMoveApplicationTitle(m_moveApplicationTitle);
    pacsConfiguration->setMoveApplicationPort(m_moveApplicationPort);
    pacsConfiguration->setRetrieveMethod(m_retrieveMethod);
}

//------------------------------------------------------------------------------

void SPacsConfigurationInitializer::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwPacsIO::data::PacsConfiguration::sptr pacsConfiguration =
        this->getObject< ::fwPacsIO::data::PacsConfiguration >();

    // Check if the user has changed the Pacs configuration and update the local var
    if(pacsConfiguration->getLocalApplicationTitle   () != m_localApplicationTitle
       || pacsConfiguration->getPacsHostName            () != m_pacsHostName
       || pacsConfiguration->getPacsApplicationTitle    () != m_pacsApplicationTitle
       || pacsConfiguration->getPacsApplicationPort     () != m_pacsApplicationPort
       || pacsConfiguration->getMoveApplicationTitle    () != m_moveApplicationTitle
       || pacsConfiguration->getMoveApplicationPort     () != m_moveApplicationPort
       || pacsConfiguration->getRetrieveMethod          () != m_retrieveMethod
       )
    {
        m_localApplicationTitle = pacsConfiguration->getLocalApplicationTitle   ();
        m_pacsHostName          = pacsConfiguration->getPacsHostName            ();
        m_pacsApplicationTitle  = pacsConfiguration->getPacsApplicationTitle    ();
        m_pacsApplicationPort   = pacsConfiguration->getPacsApplicationPort     ();
        m_moveApplicationTitle  = pacsConfiguration->getMoveApplicationTitle    ();
        m_moveApplicationPort   = pacsConfiguration->getMoveApplicationPort     ();
        m_retrieveMethod        = pacsConfiguration->getRetrieveMethod          ();
    }

    // If a preference key is set, save the local var to the preferences
    if(!m_preferenceKey.empty())
    {
        std::pair< ::fwServices::IService::sptr, ::fwData::Composite::sptr>  prefs_pair = getPreferences();
        ::fwServices::IService::sptr prefService = prefs_pair.first;
        ::fwData::Composite::sptr prefs          = prefs_pair.second;
        if(prefs && (prefs->find(m_preferenceKey) == prefs->end() || !(*prefs)[m_preferenceKey]))
        {
            (*prefs)[m_preferenceKey] = ::fwData::Composite::New();
        }
        if(prefs)
        {
            ::fwData::Composite::sptr config   = ::fwData::Composite::dynamicCast((*prefs)[m_preferenceKey]);
            (*config)["LocalApplicationTitle"] = ::fwData::String::New(m_localApplicationTitle);
            (*config)["PacsHostName"         ] = ::fwData::String::New(m_pacsHostName);
            (*config)["PacsApplicationTitle" ] = ::fwData::String::New(m_pacsApplicationTitle);
            (*config)["PacsApplicationPort"  ] =
                ::fwData::String::New(::boost::lexical_cast<std::string>(m_pacsApplicationPort));
            (*config)["MoveApplicationTitle" ] = ::fwData::String::New(m_moveApplicationTitle);
            (*config)["MoveApplicationPort"  ] =
                ::fwData::String::New(::boost::lexical_cast<std::string>(m_moveApplicationPort));
            (*config)["RetrieveMethod"       ] =
                ::fwData::String::New(::boost::lexical_cast<std::string>(m_retrieveMethod));
        }
        if(prefService)
        {
            prefService->update();
        }
    }
}

} // namespace ioPacs
