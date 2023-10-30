/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

frame_manager::frame_manager()
{
    this->frame_manager::set_close_callback(default_close_callback);
}

//-----------------------------------------------------------------------------

void frame_manager::initialize(const ui::config_t& _configuration)
{
    m_frame_info.m_name       = _configuration.get<std::string>("name", m_frame_info.m_name);
    m_frame_info.m_version    = _configuration.get<std::string>("version", "");
    m_frame_info.m_visibility = _configuration.get<bool>("visibility", m_frame_info.m_visibility);

    if(const auto icon = _configuration.get_optional<std::string>("icon"); icon.has_value())
    {
        m_frame_info.m_icon_path = core::runtime::get_module_resource_file_path(icon.value());
        SIGHT_ASSERT(
            "The icon " << m_frame_info.m_icon_path << " doesn't exist, please ensure that the path is correct",
            std::filesystem::exists(m_frame_info.m_icon_path)
        );
    }

    m_frame_info.m_min_size.first  = _configuration.get<int>("minSize.<xmlattr>.width", m_frame_info.m_min_size.first);
    m_frame_info.m_min_size.second =
        _configuration.get<int>("minSize.<xmlattr>.height", m_frame_info.m_min_size.second);
    m_frame_info.m_max_size.first  = _configuration.get<int>("maxSize.<xmlattr>.width", m_frame_info.m_max_size.first);
    m_frame_info.m_max_size.second = _configuration.get<int>(
        "maxSize.<xmlattr>.height",
        m_frame_info.m_max_size.second
    );

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
        else
        {
            SIGHT_FATAL("The style " << mode.value() << " is unknown, it should be DEFAULT, STAY_ON_TOP or MODAL.");
        }
    }

    if(const auto qss_class = _configuration.get_optional<std::string>(
           "style.<xmlattr>.QSSClass"
    ); qss_class.has_value())
    {
        m_frame_info.m_qss_class = qss_class.get();
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

        m_frame_info.m_state =
            static_cast<frame_state>(preferences.get(
                                         get_frame_state_key(m_frame_info.m_name),
                                         static_cast<std::uint8_t>(m_frame_info.m_state)
            ));
        m_frame_info.m_size.first =
            preferences.get(get_frame_w_key(m_frame_info.m_name), m_frame_info.m_size.first);
        m_frame_info.m_size.second =
            preferences.get(get_frame_h_key(m_frame_info.m_name), m_frame_info.m_size.second);
        m_frame_info.m_position.first = preferences.get(
            get_frame_x_key(m_frame_info.m_name),
            m_frame_info.m_position.first
        );
        m_frame_info.m_position.second = preferences.get(
            get_frame_y_key(m_frame_info.m_name),
            m_frame_info.m_position.second
        );
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

        if(m_frame_info.m_state != frame_state::iconized)
        {
            preferences.put(get_frame_state_key(m_frame_info.m_name), static_cast<std::uint8_t>(m_frame_info.m_state));
        }

        preferences.put(get_frame_w_key(m_frame_info.m_name), m_frame_info.m_size.first);
        preferences.put(get_frame_h_key(m_frame_info.m_name), m_frame_info.m_size.second);
        preferences.put(get_frame_x_key(m_frame_info.m_name), m_frame_info.m_position.first);
        preferences.put(get_frame_y_key(m_frame_info.m_name), m_frame_info.m_position.second);
    }
    catch(const ui::preferences_disabled&)
    {
        return;
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::layout
