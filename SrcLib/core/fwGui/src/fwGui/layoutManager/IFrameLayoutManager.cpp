/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/**
 * @file fwGui/IFrameLayoutManager.cpp
 * @brief This file defines the implementation of the base class for managing a frame.
 *
 * @author IRCAD (Research and Development Team).
 * @date 2009-2010
 */

#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include <fwCore/base.hpp>
#include <fwData/Integer.hpp>
#include <fwData/String.hpp>

#include <fwServices/Base.hpp>

#include "fwGui/layoutManager/IFrameLayoutManager.hpp"

namespace fwGui
{

namespace layoutManager
{
//-----------------------------------------------------------------------------

const IFrameLayoutManager::RegistryKeyType IFrameLayoutManager::REGISTRY_KEY =  "::fwGui::FrameLayoutManager";

const std::string IFrameLayoutManager::SOFTWARE_UI         = "SOFTWARE_UI";
const std::string IFrameLayoutManager::FRAME_STATE_UI      = "FRAME_STATE_UI" ;
const std::string IFrameLayoutManager::FRAME_SIZE_W_UI     = "FRAME_SIZE_W_UI" ;
const std::string IFrameLayoutManager::FRAME_SIZE_H_UI     = "FRAME_SIZE_H_UI" ;
const std::string IFrameLayoutManager::FRAME_POSITION_X_UI = "FRAME_POSITION_X_UI" ;
const std::string IFrameLayoutManager::FRAME_POSITION_Y_UI = "FRAME_POSITION_Y_UI" ;

//-----------------------------------------------------------------------------

IFrameLayoutManager::IFrameLayoutManager()
{
    CloseCallback fct = ::boost::bind( &::fwGui::layoutManager::IFrameLayoutManager::defaultCloseCallback, this);
    this->setCloseCallback(fct);
}

//-----------------------------------------------------------------------------

IFrameLayoutManager::~IFrameLayoutManager()
{}

//-----------------------------------------------------------------------------

void IFrameLayoutManager::initialize( ConfigurationType configuration)
{
    OSLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be frame",
            configuration->getName() == "frame");

    std::vector < ConfigurationType > name    = configuration->find("name");
    std::vector < ConfigurationType > icon    = configuration->find("icon");
    std::vector < ConfigurationType > minSize = configuration->find("minSize");
    std::vector < ConfigurationType > styles = configuration->find("style");

    if(!name.empty())
    {
        m_frameInfo.m_name = name.at(0)->getValue();
    }

    if(!icon.empty())
    {
        m_frameInfo.m_iconPath = ::boost::filesystem::path( icon.at(0)->getValue() ) ;
        OSLM_ASSERT("Sorry, icon "<< m_frameInfo.m_iconPath << " doesn't exist", ::boost::filesystem::exists(m_frameInfo.m_iconPath));
    }

    if(!minSize.empty())
    {
        if(minSize.at(0)->hasAttribute("width"))
        {
            m_frameInfo.m_minSize.first = ::boost::lexical_cast<int >(minSize.at(0)->getExistingAttributeValue("width")) ;
        }
        if(minSize.at(0)->hasAttribute("height"))
        {
            m_frameInfo.m_minSize.second = ::boost::lexical_cast<int >(minSize.at(0)->getExistingAttributeValue("height")) ;
        }
    }

    if(!styles.empty())
    {
        ::fwRuntime::ConfigurationElement::sptr stylesCfgElt = styles.at(0);
        SLM_FATAL_IF("<style> node must contain mode attribute", !stylesCfgElt->hasAttribute("mode") );
        const std::string style = stylesCfgElt->getExistingAttributeValue("mode");

        if (style == "DEFAULT")
        {
            m_frameInfo.m_style = DEFAULT;
        }
        else if (style == "STAY_ON_TOP")
        {
            m_frameInfo.m_style = STAY_ON_TOP;
        }
        else
        {
            OSLM_FATAL("Sorry, style "<<style<< " is unknown.")
        }
    }
    this->readConfig();
}

//-----------------------------------------------------------------------------

void IFrameLayoutManager::setCloseCallback(CloseCallback fct)
{
    this->m_closeCallback = fct;
}

//-----------------------------------------------------------------------------

void IFrameLayoutManager::defaultCloseCallback()
{
    SLM_WARN("No specific close callback defined");
}

//-----------------------------------------------------------------------------

void IFrameLayoutManager::readConfig()
{
    ::fwData::Composite::sptr prefUI = this->getPreferenceUI();
    if(prefUI)
    {
        if ( prefUI->find( IFrameLayoutManager::FRAME_STATE_UI ) != prefUI->end() )
        {
            ::fwData::Integer::sptr state = ::fwData::Integer::dynamicCast( (*prefUI)[ IFrameLayoutManager::FRAME_STATE_UI ] );
            SLM_ASSERT("UI state not correct", state);
            m_frameInfo.m_state = (FrameState) state->value();
        }
        if ( prefUI->find( IFrameLayoutManager::FRAME_SIZE_W_UI ) != prefUI->end() )
        {
            ::fwData::Integer::sptr sizew = ::fwData::Integer::dynamicCast( (*prefUI)[ IFrameLayoutManager::FRAME_SIZE_W_UI ] );
            SLM_ASSERT("UI sizeW not correct", sizew);
            m_frameInfo.m_size.first = *sizew;
        }
        if ( prefUI->find( IFrameLayoutManager::FRAME_SIZE_H_UI ) != prefUI->end() )
        {
            ::fwData::Integer::sptr sizeh = ::fwData::Integer::dynamicCast( (*prefUI)[ IFrameLayoutManager::FRAME_SIZE_H_UI ] );
            SLM_ASSERT("UI sizeH not correct", sizeh);
            m_frameInfo.m_size.second = *sizeh;
        }
        if ( prefUI->find( IFrameLayoutManager::FRAME_POSITION_X_UI ) != prefUI->end() )
        {
            ::fwData::Integer::sptr posx = ::fwData::Integer::dynamicCast( (*prefUI)[ IFrameLayoutManager::FRAME_POSITION_X_UI ] );
            SLM_ASSERT("UI posX not correct", posx);
            m_frameInfo.m_position.first = *posx;
        }
        if ( prefUI->find( IFrameLayoutManager::FRAME_POSITION_Y_UI ) != prefUI->end() )
        {
            ::fwData::Integer::sptr posy = ::fwData::Integer::dynamicCast( (*prefUI)[ IFrameLayoutManager::FRAME_POSITION_Y_UI ] );
            SLM_ASSERT("UI posY not correct", posy);
            m_frameInfo.m_position.second = *posy;
        }
    }
}

//-----------------------------------------------------------------------------

void IFrameLayoutManager::writeConfig()
{
    ::fwData::Composite::sptr prefUI = this->getPreferenceUI();
    if(prefUI)
    {
        if(m_frameInfo.m_state != ICONIZED)
        {
            ::fwData::Integer::NewSptr state(m_frameInfo.m_state);
            (*prefUI)[ IFrameLayoutManager::FRAME_STATE_UI ] = state;
        }

        ::fwData::Integer::NewSptr sizew(m_frameInfo.m_size.first);
        (*prefUI)[ IFrameLayoutManager::FRAME_SIZE_W_UI ] = sizew;

        ::fwData::Integer::NewSptr sizeh(m_frameInfo.m_size.second);
        (*prefUI)[ IFrameLayoutManager::FRAME_SIZE_H_UI ] = sizeh;

        ::fwData::Integer::NewSptr posx(m_frameInfo.m_position.first);
        (*prefUI)[ IFrameLayoutManager::FRAME_POSITION_X_UI ] = posx;

        ::fwData::Integer::NewSptr posy(m_frameInfo.m_position.second);
        (*prefUI)[ IFrameLayoutManager::FRAME_POSITION_Y_UI ] = posy;
    }
}

//-----------------------------------------------------------------------------

::fwData::Composite::sptr IFrameLayoutManager::getPreferenceUI()
{
    ::fwData::Composite::sptr prefUI;

    // Get preferences
    std::vector< ::fwServices::IService::sptr > preferencesServicesList = ::fwServices::OSR::getServices("::preferences::IPreferencesService");
    if(!preferencesServicesList.empty())
    {
        ::fwServices::IService::sptr prefService = preferencesServicesList[0];
        ::fwData::Composite::sptr prefs = prefService->getObject< ::fwData::Composite >();

        ::fwData::Composite::sptr framesUI;
        // Retreives software UI pref
        if ( prefs->find( IFrameLayoutManager::SOFTWARE_UI ) == prefs->end() )
        {
            framesUI = ::fwData::Composite::New();
            (*prefs)[ IFrameLayoutManager::SOFTWARE_UI ] = framesUI;
        }
        else
        {
            framesUI = ::fwData::Composite::dynamicCast( (*prefs)[ IFrameLayoutManager::SOFTWARE_UI ]);
        }
        // Retreives frame UI pref
        if ( framesUI->find( this->m_frameInfo.m_name ) != framesUI->end() )
        {
            prefUI = ::fwData::Composite::dynamicCast( (*framesUI)[ this->m_frameInfo.m_name ] );
        }
        else
        {
            prefUI = ::fwData::Composite::New();
            (*framesUI)[ this->m_frameInfo.m_name  ] = prefUI;
        }
    }
    return prefUI;
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui



