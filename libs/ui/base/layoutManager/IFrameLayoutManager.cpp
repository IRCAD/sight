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

#include "ui/base/layoutManager/IFrameLayoutManager.hpp"

#include <core/base.hpp>
#include <core/runtime/operations.hpp>

#include <data/Integer.hpp>
#include <data/String.hpp>

#include <service/macros.hpp>
#include <service/registry/ObjectService.hpp>

#include <ui/base/Preferences.hpp>

#include <boost/lexical_cast.hpp>

#include <filesystem>

namespace sight::ui::base
{

namespace layoutManager
{

//-----------------------------------------------------------------------------

const IFrameLayoutManager::RegistryKeyType IFrameLayoutManager::REGISTRY_KEY = "sight::ui::base::FrameLayoutManager";

const std::string IFrameLayoutManager::SOFTWARE_UI         = "SOFTWARE_UI";
const std::string IFrameLayoutManager::FRAME_STATE_UI      = "FRAME_STATE_UI";
const std::string IFrameLayoutManager::FRAME_SIZE_W_UI     = "FRAME_SIZE_W_UI";
const std::string IFrameLayoutManager::FRAME_SIZE_H_UI     = "FRAME_SIZE_H_UI";
const std::string IFrameLayoutManager::FRAME_POSITION_X_UI = "FRAME_POSITION_X_UI";
const std::string IFrameLayoutManager::FRAME_POSITION_Y_UI = "FRAME_POSITION_Y_UI";

//------------------------------------------------------------------------------

inline static std::string get_frame_key(const std::string& name)
{
    return IFrameLayoutManager::SOFTWARE_UI + "." + name;
}

//------------------------------------------------------------------------------

inline static std::string get_frame_state_key(const std::string& name)
{
    return get_frame_key(name) + "." + IFrameLayoutManager::FRAME_STATE_UI;
}

//------------------------------------------------------------------------------

inline static std::string get_frame_w_key(const std::string& name)
{
    return get_frame_key(name) + "." + IFrameLayoutManager::FRAME_SIZE_W_UI;
}

//------------------------------------------------------------------------------

inline static std::string get_frame_h_key(const std::string& name)
{
    return get_frame_key(name) + "." + IFrameLayoutManager::FRAME_SIZE_H_UI;
}

//------------------------------------------------------------------------------

inline static std::string get_frame_x_key(const std::string& name)
{
    return get_frame_key(name) + "." + IFrameLayoutManager::FRAME_POSITION_X_UI;
}

//------------------------------------------------------------------------------

inline static std::string get_frame_y_key(const std::string& name)
{
    return get_frame_key(name) + "." + IFrameLayoutManager::FRAME_POSITION_Y_UI;
}

//-----------------------------------------------------------------------------

IFrameLayoutManager::IFrameLayoutManager()
{
    CloseCallback fct = std::bind(&ui::base::layoutManager::IFrameLayoutManager::defaultCloseCallback, this);
    this->setCloseCallback(fct);
}

//-----------------------------------------------------------------------------

IFrameLayoutManager::~IFrameLayoutManager()
{
}

//-----------------------------------------------------------------------------

void IFrameLayoutManager::initialize(ConfigurationType configuration)
{
    SIGHT_ASSERT(
        "Bad configuration name " << configuration->getName() << ", must be frame",
        configuration->getName() == "frame"
    );

    std::vector<ConfigurationType> name       = configuration->find("name");
    std::vector<ConfigurationType> icon       = configuration->find("icon");
    std::vector<ConfigurationType> minSize    = configuration->find("minSize");
    std::vector<ConfigurationType> styles     = configuration->find("style");
    std::vector<ConfigurationType> visibility = configuration->find("visibility");

    if(!visibility.empty())
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
        SIGHT_ASSERT(
            "The icon " << m_frameInfo.m_iconPath << " doesn't exist, please ensure that the path is correct",
            std::filesystem::exists(m_frameInfo.m_iconPath)
        );
    }

    if(!minSize.empty())
    {
        if(minSize.at(0)->hasAttribute("width"))
        {
            m_frameInfo.m_minSize.first =
                ::boost::lexical_cast<int>(minSize.at(0)->getExistingAttributeValue("width"));
        }

        if(minSize.at(0)->hasAttribute("height"))
        {
            m_frameInfo.m_minSize.second = ::boost::lexical_cast<int>(
                minSize.at(0)->getExistingAttributeValue(
                    "height"
                )
            );
        }
    }

    if(!styles.empty())
    {
        core::runtime::ConfigurationElement::sptr stylesCfgElt = styles.at(0);
        SIGHT_FATAL_IF("<style> node must contain mode attribute", !stylesCfgElt->hasAttribute("mode"));
        const std::string style = stylesCfgElt->getExistingAttributeValue("mode");

        if(style == "DEFAULT")
        {
            m_frameInfo.m_style = DEFAULT;
        }
        else if(style == "STAY_ON_TOP")
        {
            m_frameInfo.m_style = STAY_ON_TOP;
        }
        else if(style == "MODAL")
        {
            m_frameInfo.m_style = MODAL;
        }
        else
        {
            SIGHT_FATAL("The style " << style << " is unknown, it should be DEFAULT, STAY_ON_TOP or MODAL.");
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
    SIGHT_WARN("No specific close callback defined");
}

//-----------------------------------------------------------------------------

void IFrameLayoutManager::readConfig()
{
    try
    {
        ui::base::Preferences preferences;

        m_frameInfo.m_state =
            static_cast<FrameState>(preferences.get(
                                        get_frame_state_key(m_frameInfo.m_name),
                                        static_cast<std::uint8_t>(m_frameInfo.m_state)
            ));
        m_frameInfo.m_size.first     = preferences.get(get_frame_w_key(m_frameInfo.m_name), m_frameInfo.m_size.first);
        m_frameInfo.m_size.second    = preferences.get(get_frame_h_key(m_frameInfo.m_name), m_frameInfo.m_size.second);
        m_frameInfo.m_position.first = preferences.get(
            get_frame_x_key(m_frameInfo.m_name),
            m_frameInfo.m_position.first
        );
        m_frameInfo.m_position.second = preferences.get(
            get_frame_y_key(m_frameInfo.m_name),
            m_frameInfo.m_position.second
        );
    }
    catch(const ui::base::PreferencesDisabled&)
    {
        // Nothing to do..
    }
}

//-----------------------------------------------------------------------------

void IFrameLayoutManager::writeConfig()
{
    try
    {
        ui::base::Preferences preferences;

        if(m_frameInfo.m_state != FrameState::ICONIZED)
        {
            preferences.put(get_frame_state_key(m_frameInfo.m_name), static_cast<std::uint8_t>(m_frameInfo.m_state));
        }

        preferences.put(get_frame_w_key(m_frameInfo.m_name), m_frameInfo.m_size.first);
        preferences.put(get_frame_h_key(m_frameInfo.m_name), m_frameInfo.m_size.second);
        preferences.put(get_frame_x_key(m_frameInfo.m_name), m_frameInfo.m_position.first);
        preferences.put(get_frame_y_key(m_frameInfo.m_name), m_frameInfo.m_position.second);
    }
    catch(const ui::base::PreferencesDisabled&)
    {
        // Nothing to do..
    }
}

//-----------------------------------------------------------------------------

} // namespace layoutManager

} // namespace sight::ui::base
