/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "ui/__/layout/frame_manager.hpp"

#include <core/base.hpp>
#include <core/runtime/path.hpp>

#include <data/integer.hpp>
#include <data/string.hpp>

#include <service/macros.hpp>
#include <service/registry.hpp>

#include <ui/__/preferences.hpp>

#include <boost/lexical_cast.hpp>

#include <filesystem>

namespace sight::ui::layout
{

//-----------------------------------------------------------------------------

const frame_manager::registry_key_t frame_manager::REGISTRY_KEY = "sight::ui::layout::frame";

const std::string frame_manager::SOFTWARE_UI         = "SOFTWARE_UI";
const std::string frame_manager::FRAME_STATE_UI      = "FRAME_STATE_UI";
const std::string frame_manager::FRAME_SIZE_W_UI     = "FRAME_SIZE_W_UI";
const std::string frame_manager::FRAME_SIZE_H_UI     = "FRAME_SIZE_H_UI";
const std::string frame_manager::FRAME_POSITION_X_UI = "FRAME_POSITION_X_UI";
const std::string frame_manager::FRAME_POSITION_Y_UI = "FRAME_POSITION_Y_UI";
const std::string frame_manager::FRAME_SCREEN        = "FRAME_SCREEN";

//------------------------------------------------------------------------------

inline static std::string get_frame_key(const std::string& _name)
{
    return std::string(frame_manager::SOFTWARE_UI) + "." + _name;
}

//------------------------------------------------------------------------------

inline static std::string get_frame_state_key(const std::string& _name)
{
    return get_frame_key(_name) + "." + frame_manager::FRAME_STATE_UI;
}

//------------------------------------------------------------------------------

inline static std::string get_frame_w_key(const std::string& _name)
{
    return get_frame_key(_name) + "." + frame_manager::FRAME_SIZE_W_UI;
}

//------------------------------------------------------------------------------

inline static std::string get_frame_h_key(const std::string& _name)
{
    return get_frame_key(_name) + "." + frame_manager::FRAME_SIZE_H_UI;
}

//------------------------------------------------------------------------------

inline static std::string get_frame_x_key(const std::string& _name)
{
    return get_frame_key(_name) + "." + frame_manager::FRAME_POSITION_X_UI;
}

//------------------------------------------------------------------------------

inline static std::string get_frame_y_key(const std::string& _name)
{
    return get_frame_key(_name) + "." + frame_manager::FRAME_POSITION_Y_UI;
}

//-----------------------------------------------------------------------------

inline static std::string get_frame_screen_key(const std::string& _name)
{
    return get_frame_key(_name) + "." + frame_manager::FRAME_SCREEN;
}

//-----------------------------------------------------------------------------

frame_manager::frame_manager()
{
    this->frame_manager::set_close_callback(default_close_callback);
}

//-----------------------------------------------------------------------------

void frame_manager::initialize(const ui::config_t& _configuration)
{
    if(const auto& name = _configuration.get_optional<std::string>("name"); name)
    {
        m_frame_info.m_name = *name;
    }

    if(const auto& version = _configuration.get_optional<std::string>("version"); version)
    {
        m_frame_info.m_version = *version;
    }

    if(const auto& visibility = _configuration.get_optional<bool>("visibility"); visibility)
    {
        m_frame_info.m_visibility = *visibility;
    }

    if(const auto icon = _configuration.get_optional<std::string>("icon"); icon.has_value())
    {
        m_frame_info.m_icon_path = core::runtime::get_module_resource_file_path(icon.value());
        SIGHT_ASSERT(
            "The icon " << *m_frame_info.m_icon_path << " doesn't exist, please ensure that the path is correct",
            std::filesystem::exists(*m_frame_info.m_icon_path)
        );
    }

    if(auto width = _configuration.get_optional<int>("min_size.<xmlattr>.width"); width)
    {
        m_frame_info.m_min_width = *width;
    }
    else if(width = _configuration.get_optional<int>("minSize.<xmlattr>.width"); width)
    {
        FW_DEPRECATED_MSG("'minSize' is deprecated, use 'min_size' instead", "26.0");
        m_frame_info.m_min_width = *width;
    }

    if(auto height = _configuration.get_optional<int>("min_size.<xmlattr>.height"); height)
    {
        m_frame_info.m_min_height = *height;
    }
    else if(height = _configuration.get_optional<int>("minSize.<xmlattr>.height"); height)
    {
        FW_DEPRECATED_MSG("'minSize' is deprecated, use 'min_size' instead", "26.0");
        m_frame_info.m_min_height = *height;
    }

    if(auto width = _configuration.get_optional<int>("max_size.<xmlattr>.width"); width)
    {
        m_frame_info.m_max_width = *width;
    }
    else if(width = _configuration.get_optional<int>("maxSize.<xmlattr>.width"); width)
    {
        FW_DEPRECATED_MSG("'maxSize' is deprecated, use 'min_size' instead", "26.0");
        m_frame_info.m_max_width = *width;
    }

    if(auto height = _configuration.get_optional<int>("max_size.<xmlattr>.height"); height)
    {
        m_frame_info.m_max_height = *height;
    }
    else if(height = _configuration.get_optional<int>("maxSize.<xmlattr>.height"); height)
    {
        FW_DEPRECATED_MSG("'maxSize' is deprecated, use 'min_size' instead", "26.0");
        m_frame_info.m_max_height = *height;
    }

    if(const auto mode = _configuration.get_optional<std::string>("style.<xmlattr>.mode"); mode.has_value())
    {
        if(mode.value() == "DEFAULT")
        {
            m_frame_info.m_style = DEFAULT;
        }
        else if(mode.value() == "STAY_ON_TOP")
        {
            m_frame_info.m_style = stay_on_top;
        }
        else if(mode.value() == "MODAL")
        {
            m_frame_info.m_style = modal;
        }
        else if(mode.value() == "FULLSCREEN")
        {
            m_frame_info.m_style = fullscreen;
        }
        else if(mode.value() == "FRAMELESS")
        {
            m_frame_info.m_style = frameless;
        }
        else
        {
            SIGHT_FATAL(
                "The style " << mode.value()
                << " is unknown, it should be DEFAULT, STAY_ON_TOP, FULLSCREEN, FRAMELESS or MODAL."
            );
        }
    }

    if(const auto& qss_class = _configuration.get_optional<std::string>("style.<xmlattr>.QSSClass"); qss_class)
    {
        m_frame_info.m_qss_class = *qss_class;
    }

    if(const auto& screen = _configuration.get_optional<int>("screen.<xmlattr>.index"); screen)
    {
        m_frame_info.m_configured_screen = *screen;
    }

    this->read_config();
}

//-----------------------------------------------------------------------------

void frame_manager::set_close_callback(CloseCallback _fct)
{
    this->m_close_callback = _fct;
}

//-----------------------------------------------------------------------------

void frame_manager::default_close_callback()
{
    SIGHT_WARN("No specific close callback defined");
}

//-----------------------------------------------------------------------------

void frame_manager::read_config()
{
    try
    {
        ui::preferences preferences;

        const std::string frame_name = m_frame_info.m_name.value_or("");

        using state_t = std::underlying_type_t<frame_state>;

        if(const auto& state = preferences.get_optional<state_t>(get_frame_state_key(frame_name)); state)
        {
            m_frame_info.m_state = static_cast<frame_state>(*state);
        }

        if(const auto& width = preferences.get_optional<int>(get_frame_w_key(frame_name)); width)
        {
            m_frame_info.m_width = *width;
        }

        if(const auto& height = preferences.get_optional<int>(get_frame_h_key(frame_name)); height)
        {
            m_frame_info.m_height = *height;
        }

        if(const auto& x = preferences.get_optional<int>(get_frame_x_key(frame_name)); x)
        {
            m_frame_info.m_x = *x;
        }

        if(const auto& y = preferences.get_optional<int>(get_frame_y_key(frame_name)); y)
        {
            m_frame_info.m_y = *y;
        }

        if(const auto& screen = preferences.get_optional<int>(get_frame_screen_key(frame_name)); screen)
        {
            m_frame_info.m_saved_screen = *screen;
        }
    }
    catch(const ui::preferences_disabled&)
    {
        return;
    }
}

//-----------------------------------------------------------------------------

void frame_manager::write_config() const
{
    try
    {
        ui::preferences preferences;

        const std::string frame_name = m_frame_info.m_name.value_or("");

        if(m_frame_info.m_state)
        {
            preferences.put(
                get_frame_state_key(frame_name),
                std::underlying_type_t<frame_state>(*m_frame_info.m_state)
            );
        }

        if(m_frame_info.m_width)
        {
            preferences.put(get_frame_w_key(frame_name), *m_frame_info.m_width);
        }

        if(m_frame_info.m_height)
        {
            preferences.put(get_frame_h_key(frame_name), *m_frame_info.m_height);
        }

        if(m_frame_info.m_saved_screen)
        {
            preferences.put(get_frame_screen_key(frame_name), *m_frame_info.m_saved_screen);
        }

        if(m_frame_info.m_x)
        {
            preferences.put(get_frame_x_key(frame_name), *m_frame_info.m_x);
        }

        if(m_frame_info.m_y)
        {
            preferences.put(get_frame_y_key(frame_name), *m_frame_info.m_y);
        }

        if(m_frame_info.m_x)
        {
            preferences.put(get_frame_x_key(frame_name), *m_frame_info.m_x);
        }

        if(m_frame_info.m_y)
        {
            preferences.put(get_frame_y_key(frame_name), *m_frame_info.m_y);
        }

        if(m_frame_info.m_configured_screen)
        {
            preferences.put(get_frame_screen_key(frame_name), *m_frame_info.m_configured_screen);
        }
    }
    catch(const ui::preferences_disabled&)
    {
        return;
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::layout
