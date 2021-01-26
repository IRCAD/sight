/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "fwGui/layoutManager/IFrameLayoutManager.hpp"

#include <core/base.hpp>
#include <core/runtime/operations.hpp>

#include <data/Integer.hpp>
#include <data/String.hpp>

#include <fwPreferences/helper.hpp>

#include <services/macros.hpp>
#include <services/registry/ObjectService.hpp>

#include <boost/lexical_cast.hpp>

#include <filesystem>

namespace fwGui
{

namespace layoutManager
{
//-----------------------------------------------------------------------------

const IFrameLayoutManager::RegistryKeyType IFrameLayoutManager::REGISTRY_KEY = "::fwGui::FrameLayoutManager";

const std::string IFrameLayoutManager::SOFTWARE_UI         = "SOFTWARE_UI";
const std::string IFrameLayoutManager::FRAME_STATE_UI      = "FRAME_STATE_UI";
const std::string IFrameLayoutManager::FRAME_SIZE_W_UI     = "FRAME_SIZE_W_UI";
const std::string IFrameLayoutManager::FRAME_SIZE_H_UI     = "FRAME_SIZE_H_UI";
const std::string IFrameLayoutManager::FRAME_POSITION_X_UI = "FRAME_POSITION_X_UI";
const std::string IFrameLayoutManager::FRAME_POSITION_Y_UI = "FRAME_POSITION_Y_UI";

//-----------------------------------------------------------------------------

IFrameLayoutManager::IFrameLayoutManager()
{
    CloseCallback fct = std::bind( &::fwGui::layoutManager::IFrameLayoutManager::defaultCloseCallback, this);
    this->setCloseCallback(fct);
}

//-----------------------------------------------------------------------------

IFrameLayoutManager::~IFrameLayoutManager()
{
}

//-----------------------------------------------------------------------------

void IFrameLayoutManager::initialize( ConfigurationType configuration)
{
    SLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be frame",
               configuration->getName() == "frame");

    std::vector < ConfigurationType > name       = configuration->find("name");
    std::vector < ConfigurationType > icon       = configuration->find("icon");
    std::vector < ConfigurationType > minSize    = configuration->find("minSize");
    std::vector < ConfigurationType > styles     = configuration->find("style");
    std::vector < ConfigurationType > visibility = configuration->find("visibility");

    if (!visibility.empty())
    {
        m_frameInfo.m_visibility = (visibility.at(0)->getValue() == "true");
    }

    if(!name.empty())
    {
        m_frameInfo.m_name = name.at(0)->getValue();
    }

    if(!icon.empty())
    {
        m_frameInfo.m_iconPath = core::runtime::getModuleResourceFilePath(icon.at(0)->getValue());
        SLM_ASSERT("The icon "<< m_frameInfo.m_iconPath << " doesn't exist, please ensure that the path is correct",
                   std::filesystem::exists(m_frameInfo.m_iconPath));
    }

    if(!minSize.empty())
    {
        if(minSize.at(0)->hasAttribute("width"))
        {
            m_frameInfo.m_minSize.first =
                ::boost::lexical_cast<int >(minSize.at(0)->getExistingAttributeValue("width"));
        }
        if(minSize.at(0)->hasAttribute("height"))
        {
            m_frameInfo.m_minSize.second = ::boost::lexical_cast<int >(minSize.at(0)->getExistingAttributeValue(
                                                                           "height"));
        }
    }

    if(!styles.empty())
    {
        core::runtime::ConfigurationElement::sptr stylesCfgElt = styles.at(0);
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
        else if (style == "MODAL")
        {
            m_frameInfo.m_style = MODAL;
        }
        else
        {
            SLM_FATAL("The style "<<style<< " is unknown, it should be DEFAULT, STAY_ON_TOP or MODAL.");
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
    data::Composite::sptr prefUI = this->getPreferenceUI();
    if(prefUI)
    {
        if ( prefUI->find( IFrameLayoutManager::FRAME_STATE_UI ) != prefUI->end() )
        {
            data::Integer::sptr state =
                data::Integer::dynamicCast( (*prefUI)[ IFrameLayoutManager::FRAME_STATE_UI ] );
            SLM_ASSERT("UI state not correct", state);
            m_frameInfo.m_state = (FrameState) state->value();
        }
        if ( prefUI->find( IFrameLayoutManager::FRAME_SIZE_W_UI ) != prefUI->end() )
        {
            data::Integer::sptr sizew =
                data::Integer::dynamicCast( (*prefUI)[ IFrameLayoutManager::FRAME_SIZE_W_UI ] );
            SLM_ASSERT("UI sizeW not correct", sizew);
            m_frameInfo.m_size.first = *sizew;
        }
        if ( prefUI->find( IFrameLayoutManager::FRAME_SIZE_H_UI ) != prefUI->end() )
        {
            data::Integer::sptr sizeh =
                data::Integer::dynamicCast( (*prefUI)[ IFrameLayoutManager::FRAME_SIZE_H_UI ] );
            SLM_ASSERT("UI sizeH not correct", sizeh);
            m_frameInfo.m_size.second = *sizeh;
        }
        if ( prefUI->find( IFrameLayoutManager::FRAME_POSITION_X_UI ) != prefUI->end() )
        {
            data::Integer::sptr posx =
                data::Integer::dynamicCast( (*prefUI)[ IFrameLayoutManager::FRAME_POSITION_X_UI ] );
            SLM_ASSERT("UI posX not correct", posx);
            m_frameInfo.m_position.first = *posx;
        }
        if ( prefUI->find( IFrameLayoutManager::FRAME_POSITION_Y_UI ) != prefUI->end() )
        {
            data::Integer::sptr posy =
                data::Integer::dynamicCast( (*prefUI)[ IFrameLayoutManager::FRAME_POSITION_Y_UI ] );
            SLM_ASSERT("UI posY not correct", posy);
            m_frameInfo.m_position.second = *posy;
        }
    }
}

//-----------------------------------------------------------------------------

void IFrameLayoutManager::writeConfig()
{
    data::Composite::sptr prefUI = this->getPreferenceUI();
    if(prefUI)
    {
        if(m_frameInfo.m_state != ICONIZED)
        {
            data::Integer::sptr state = data::Integer::New(m_frameInfo.m_state);
            (*prefUI)[ IFrameLayoutManager::FRAME_STATE_UI ] = state;
        }

        data::Integer::sptr sizew = data::Integer::New(m_frameInfo.m_size.first);
        (*prefUI)[ IFrameLayoutManager::FRAME_SIZE_W_UI ] = sizew;

        data::Integer::sptr sizeh = data::Integer::New(m_frameInfo.m_size.second);
        (*prefUI)[ IFrameLayoutManager::FRAME_SIZE_H_UI ] = sizeh;

        data::Integer::sptr posx = data::Integer::New(m_frameInfo.m_position.first);
        (*prefUI)[ IFrameLayoutManager::FRAME_POSITION_X_UI ] = posx;

        data::Integer::sptr posy = data::Integer::New(m_frameInfo.m_position.second);
        (*prefUI)[ IFrameLayoutManager::FRAME_POSITION_Y_UI ] = posy;
    }
}

//-----------------------------------------------------------------------------

data::Composite::sptr IFrameLayoutManager::getPreferenceUI()
{
    data::Composite::sptr prefUI;
    data::Composite::sptr prefs = ::fwPreferences::getPreferences();

    // Get preferences
    if(prefs)
    {
        data::Composite::sptr framesUI;
        // Retreives software UI pref
        if ( prefs->find( IFrameLayoutManager::SOFTWARE_UI ) == prefs->end() )
        {
            framesUI                                     = data::Composite::New();
            (*prefs)[ IFrameLayoutManager::SOFTWARE_UI ] = framesUI;
        }
        else
        {
            framesUI = data::Composite::dynamicCast( (*prefs)[ IFrameLayoutManager::SOFTWARE_UI ]);
        }
        // Retreives frame UI pref
        if ( framesUI->find( this->m_frameInfo.m_name ) != framesUI->end() )
        {
            prefUI = data::Composite::dynamicCast( (*framesUI)[ this->m_frameInfo.m_name ] );
        }
        else
        {
            prefUI                                   = data::Composite::New();
            (*framesUI)[ this->m_frameInfo.m_name  ] = prefUI;
        }
    }
    return prefUI;
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui
