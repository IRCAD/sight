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

#include "ui/__/builder/menubar.hpp"
#include "ui/__/builder/toolbar.hpp"
#include "ui/__/container/widget.hpp"
#include "ui/__/detail/registry/view.hpp"
#include "ui/__/layout/frame_manager.hpp"

#include <service/base.hpp>

namespace sight::ui
{

namespace detail::registry
{

class view;

}

/**
 * @brief   Defines the service interface managing a frame.
 *
 * @section Slots Slots
 *
 * - \b set_visible(bool isVisible) : this slot shows the container (if isVisible = true) or hides it.
 * - \b show() : this slot shows the container.
 * - \b hide() : this slot hides the container.
 *
 * @section XML Example of XML configuration
 *
 * Example of configuration
 * @code{.xml}
   <service uid="mainFrame" type="sight::module::ui::frame"
 * auto_connect="false">
     <window onclose="notify" />
     <gui>
         <frame>
             <name>My app</name>
             <icon>myApp-1.0/icon.ico</icon>
             <min_size width="800" height="600" />
             <style mode="MODAL" />
             <visibility>true</visibility>
         </frame>
         <toolbar />
         <menubar />
     </gui>
     <registry>
         <toolbar sid="toolbar1" />
         <menubar sid="menubar1" />
         <view sid="myView" />
     </registry>
   </service>
   @endcode
 * - \<window onclose="notify" /\> : defines what to do when the frame is closed
 *   - \b exit (by default) : the application is closed. Use it for the main frame.
 *   - \b notify : send signal 'closed'
 *   - \b message : a confirmation dialog appears asking user to confirm closing application
 * - \<frame\> : defines the frame name, icon, size and style.
 *   - \b style : defines frame style (modal, always on top, etc.), not mandatory.
 *     Allowed values are MODAL, STAY_ON_TOP, FULLSCREEN and DEFAULT (default value).
 *   - \b visibility : set the frame visibility when it is created. Possible values are true (default) or false.
 *   - \b min_size : the minimum width and height the application window can have
 *   - \b max_size : the maximum width and height the application window can have
 *   - \b size : the default width and height of the application window. It is constrained by min_size and max_size.
 * - The toolbar section isn't mandatory.
 * - The menubar section isn't mandatory.

 */
class SIGHT_UI_CLASS_API frame : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(frame, service::base);

    /// Get widget defined for progress bar
    SIGHT_UI_API static ui::container::widget::sptr get_progress_widget();

    /**
     * @name Signals
     * @{
     */

    /// Signal emitted when frame is closed and onclose policy is notify
    static const core::com::signals::key_t CLOSED_SIG;
    using closed_signal_t = core::com::signal<void ()>;
    /**
     * @}
     */

    /**
     * @name Slots Keys
     * @{
     */

    /// Slot to show/hide the container
    static const core::com::slots::key_t SET_VISIBLE_SLOT;

    /// Slot to show the container
    static const core::com::slots::key_t SHOW_SLOT;

    /// Slot to hide the container
    static const core::com::slots::key_t HIDE_SLOT;
/**
 * @}
 */

protected:

    SIGHT_UI_API frame();
    SIGHT_UI_API ~frame() override = default;

    /**
     * @brief Initialize frame managers.
     *
     * @see ui::registry::view::initialize(),
     * ui::layout::frame_manager::initialize(),
     *      ui::builder::toolbar::initialize(), ui::builder::menubar::initialize()
     */
    SIGHT_UI_API void initialize();

    /// Creates frame, sub-view, menubar and toolbar containers. Manages sub-view, menubar and toobar services.
    SIGHT_UI_API void create();

    /// Stops sub-view, menubar and toobar services. Destroys frame, sub-view, menubar and toolbar containers.
    SIGHT_UI_API void destroy();

    SIGHT_UI_API static const std::string CLOSE_POLICY_EXIT;
    SIGHT_UI_API static const std::string CLOSE_POLICY_NOTIFY;
    SIGHT_UI_API static const std::string CLOSE_POLICY_MESSAGE;

    /// Static reference on a widget defined for progress bar installation
    SIGHT_UI_API static ui::container::widget::wptr s_progress_widget;

private:

    /// SLOT: show/hide the container
    void set_visible(bool _is_visible);

    /// SLOT: show the container
    void show();

    /// SLOT: hide the container
    void hide();

    static void on_close_exit();
    void on_close_notify();
    static void on_close_message();
    void initialize_layout_manager(const ui::config_t& _layout_config);
    void initialize_menu_bar_builder(const ui::config_t& _menu_bar_config);
    void initialize_tool_bar_builder(const ui::config_t& _tool_bar_config);

    ui::layout::frame_manager::sptr m_frame_layout_manager;
    SPTR(ui::detail::registry::view) m_view_registry;
    ui::builder::menubar::sptr m_menu_bar_builder;
    ui::builder::toolbar::sptr m_tool_bar_builder;

    bool m_has_menu_bar {false};
    bool m_has_tool_bar {false};

    std::string m_close_policy;

    /// Signal emitted when frame is closed and onclose mode is message
    closed_signal_t::sptr m_sig_closed;
};

} // namespace sight::ui
