/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/__/builder/menubar.hpp"
#include "ui/__/builder/toolbar.hpp"
#include "ui/__/config.hpp"
#include "ui/__/container/widget.hpp"
#include "ui/__/detail/registry/View.hpp"
#include "ui/__/layout/frame_manager.hpp"

#include <service/base.hpp>

namespace sight::ui
{

namespace detail::registry
{

class View;

}

/**
 * @brief   Defines the service interface managing a frame.
 *
 * @section Slots Slots
 *
 * - \b setVisible(bool isVisible) : this slot shows the container (if isVisible = true) or hides it.
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
             <name>My App</name>
             <icon>myApp-1.0/icon.ico</icon>
             <minSize width="800" height="600" />
             <style mode="MODAL" />
             <visibility>true</visibility>
         </frame>
         <toolBar />
         <menuBar />
     </gui>
     <registry>
         <toolBar sid="toolbar1" start="true" />
         <menuBar sid="menubar1" start="true" />
         <view sid="myView" start="true" />
     </registry>
   </service>
   @endcode
 * - \<window onclose="notify" /\> : defines what to do when the frame is closed
 *   - \b exit (by default) : the application is closed. Use it for the main frame.
 *   - \b notify : send signal 'closed'
 *   - \b message : a confirmation dialog appears asking user to confirm closing application
 * - \<frame\> : defines the frame name, icon, size and style.
 *   - \b style : defines frame style (modal, always on top, etc.), not mandatory.
 *     Allowed values are MODAL, STAY_ON_TOP and DEFAULT (default value).
 *   - \b visibility : set the frame visibility when it is created. Possible values are true (default) or false.
 *   - \b minSize : the minimum width and height the application window can have
 *   - \b maxSize : the maximum width and height the application window can have
 * - The toolBar section isn't mandatory.
 * - The menuBar section isn't mandatory.

 */
class UI_CLASS_API frame : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(frame, service::base);

    /// Get widget defined for progress bar
    UI_API static ui::container::widget::sptr getProgressWidget();

    /**
     * @name Signals
     * @{
     */

    /// Signal emitted when frame is closed and onclose policy is notify
    static const core::com::signals::key_t CLOSED_SIG;
    typedef core::com::signal<void ()> ClosedSignalType;
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

    UI_API frame();
    UI_API ~frame() override = default;

    /**
     * @brief Initialize frame managers.
     *
     * @see ui::registry::View::initialize(),
     * ui::layout::frame_manager::initialize(),
     *      ui::builder::toolbar::initialize(), ui::builder::menubar::initialize()
     */
    UI_API void initialize();

    /// Creates frame, sub-view, menubar and toolbar containers. Manages sub-view, menubar and toobar services.
    UI_API void create();

    /// Stops sub-view, menubar and toobar services. Destroys frame, sub-view, menubar and toolbar containers.
    UI_API void destroy();

    UI_API static const std::string CLOSE_POLICY_EXIT;
    UI_API static const std::string CLOSE_POLICY_NOTIFY;
    UI_API static const std::string CLOSE_POLICY_MESSAGE;

    /// Static reference on a widget defined for progress bar installation
    UI_API static ui::container::widget::wptr m_progressWidget;

private:

    /// SLOT: show/hide the container
    void setVisible(bool isVisible);

    /// SLOT: show the container
    void show();

    /// SLOT: hide the container
    void hide();

    static void onCloseExit();
    void onCloseNotify();
    static void onCloseMessage();
    void initializeLayoutManager(const ui::config_t& layoutConfig);
    void initializeMenuBarBuilder(const ui::config_t& menuBarConfig);
    void initializeToolBarBuilder(const ui::config_t& toolBarConfig);

    ui::layout::frame_manager::sptr m_frameLayoutManager;
    SPTR(ui::detail::registry::View) m_viewRegistry;
    ui::builder::menubar::sptr m_menuBarBuilder;
    ui::builder::toolbar::sptr m_toolBarBuilder;

    bool m_hasMenuBar {false};
    bool m_hasToolBar {false};

    std::string m_closePolicy;

    /// Signal emitted when frame is closed and onclose mode is message
    ClosedSignalType::sptr m_sigClosed;
};

} // namespace sight::ui
