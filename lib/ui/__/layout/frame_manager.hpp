/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include <sight/ui/__/config.hpp>

#include "ui/__/container/widget.hpp"
#include "ui/__/object.hpp"

#include <filesystem>
#include <functional>
#include <list>

namespace sight::ui::layout
{

/**
 * @brief   Defines an interface for managing a frame.
 */
class SIGHT_UI_CLASS_API frame_manager : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(frame_manager, ui::object);

    /// Defines all possible style for a frame
    enum style
    {
        DEFAULT,
        stay_on_top,
        modal,
        fullscreen,
        frameless
    };

    using registry_key_t = std::string;

    static const std::string SOFTWARE_UI;
    static const std::string FRAME_STATE_UI;
    static const std::string FRAME_SIZE_W_UI;
    static const std::string FRAME_SIZE_H_UI;
    static const std::string FRAME_POSITION_X_UI;
    static const std::string FRAME_POSITION_Y_UI;

    enum class frame_state : std::uint8_t
    {
        normal      = 0, ///< the normal state
        iconized    = 1, ///< the minimized state
        maximized   = 2, ///< the maximied state
        full_screen = 4  ///< the full screen state
    };

    class frame_info
    {
    public:

        frame_info() = default;

        /// Frame name.
        std::string m_name;

        /// Optional version number, displayed in the title if specified.
        std::string m_version;

        /// Frame icon.
        std::filesystem::path m_icon_path;

        /// Frame minimum size (min width and min height)
        std::pair<int, int> m_min_size {-1, -1};

        /// Frame maximum size (max width and max height)
        std::pair<int, int> m_max_size {-1, -1};

        /// Frame style
        style m_style {DEFAULT};

        /// Frame size
        std::pair<int, int> m_size {-1, -1};

        /// Frame position
        std::pair<int, int> m_position {-1, -1};

        /// Frame state (maximize, minized, full screen)
        frame_state m_state {frame_state::normal};

        /// Frame visibility
        bool m_visibility {true};

        std::string m_qss_class;

        /// Wanted Screen index (-1 means unset)
        int m_screen {-1};
    };

    /// Constructor. Do nothing.
    SIGHT_UI_API frame_manager();

    /// Destructor. Do nothing.
    SIGHT_UI_API ~frame_manager() override = default;

    /**
     * @brief Configure the layout before creation.
     */
    SIGHT_UI_API virtual void initialize(const ui::config_t& _configuration);

    SIGHT_UI_API static const registry_key_t REGISTRY_KEY;

    /**
     * @brief Instantiate frame.
     */
    SIGHT_UI_API virtual void create_frame() = 0;

    /**
     * @brief Destroy frame.
     */
    SIGHT_UI_API virtual void destroy_frame() = 0;

    /// Return the frame container
    virtual ui::container::widget::sptr get_frame()
    {
        return m_frame;
    }

    /// Return the first container into the frame
    virtual ui::container::widget::sptr get_container()
    {
        return m_container;
    }

    using CloseCallback = std::function<void ()>;
    SIGHT_UI_API virtual void set_close_callback(CloseCallback _fct);

protected:

    /**
     * @brief Configuration definition.
     * @{ */
    const frame_info& get_frame_info() const
    {
        return m_frame_info;
    }

    //------------------------------------------------------------------------------

    frame_info& get_frame_info()
    {
        return m_frame_info;
    }

    //------------------------------------------------------------------------------

    void set_frame_info(const frame_info& _frame_info)
    {
        m_frame_info = _frame_info;
    }

    /**  @} */

    ui::container::widget::sptr m_frame;
    ui::container::widget::sptr m_container;
    CloseCallback m_close_callback;

    SIGHT_UI_API void read_config();

    SIGHT_UI_API void write_config() const;

private:

    static void default_close_callback();

    /// Save frame configuration definition
    frame_info m_frame_info;
};

} // namespace sight::ui::layout
