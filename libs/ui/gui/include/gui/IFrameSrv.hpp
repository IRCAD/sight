/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "gui/builder/IMenuBarBuilder.hpp"
#include "gui/builder/IToolBarBuilder.hpp"
#include "gui/config.hpp"
#include "gui/container/fwContainer.hpp"
#include "gui/layoutManager/IFrameLayoutManager.hpp"
#include "gui/registrar/ViewRegistrar.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>

#include <services/IService.hpp>

namespace sight::gui
{

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
   <service uid="mainFrame" type="::gui::IFrameSrv" impl="::modules::gui::frame::SDefaultFrame" autoConnect="no">
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
         <toolBar sid="toolbar1" start="yes" />
         <menuBar sid="menubar1" start="yes" />
         <view sid="myView" start="yes" />
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
 * - The toolBar section isn't mandatory.
 * - The menuBar section isn't mandatory.

 */
class GUI_CLASS_API IFrameSrv : public services::IService
{

public:

    fwCoreServiceMacro(IFrameSrv, services::IService)

    /// Get widget defined for progress bar
    GUI_API static gui::container::fwContainer::sptr getProgressWidget();

    /**
     * @name Signals
     * @{
     */
    /// Signal emitted when frame is closed and onclose policy is notify
    static const core::com::Signals::SignalKeyType s_CLOSED_SIG;
    typedef core::com::Signal< void ()> ClosedSignalType;
    /**
     * @}
     */

    /**
     * @name Slots Keys
     * @{
     */

    /// Slot to show/hide the container
    static const core::com::Slots::SlotKeyType s_SET_VISIBLE_SLOT;

    /// Slot to show the container
    static const core::com::Slots::SlotKeyType s_SHOW_SLOT;

    /// Slot to hide the container
    static const core::com::Slots::SlotKeyType s_HIDE_SLOT;
    /**
     * @}
     */

protected:

    GUI_API IFrameSrv();

    GUI_API virtual ~IFrameSrv();

    typedef core::runtime::ConfigurationElement::sptr ConfigurationType;
    /**
     * @brief Initialize frame managers.
     *
     * @see gui::registrar::ViewRegistrar::initialize(), gui::layoutManager::IFrameLayoutManager::initialize(),
     *      gui::builder::IToolBarBuilder::initialize(), gui::builder::IMenuBarBuilder::initialize()
     */
    GUI_API void initialize();

    /// Creates frame, sub-view, menubar and toolbar containers. Manages sub-view, menubar and toobar services.
    GUI_API void create();

    /// Stops sub-view, menubar and toobar services. Destroys frame, sub-view, menubar and toolbar containers.
    GUI_API void destroy();

    GUI_API static const std::string CLOSE_POLICY_EXIT;
    GUI_API static const std::string CLOSE_POLICY_NOTIFY;
    GUI_API static const std::string CLOSE_POLICY_MESSAGE;

    /// Static reference on a widget defined for progress bar installation
    GUI_API static gui::container::fwContainer::wptr m_progressWidget;

private:

    /// SLOT: show/hide the container
    void setVisible(bool isVisible);
    /// SLOT: show the container
    void show();
    /// SLOT: hide the container
    void hide();

    void onCloseExit();
    void onCloseNotify();
    void onCloseMessage();
    void initializeLayoutManager( core::runtime::ConfigurationElement::sptr layoutConfig );
    void initializeMenuBarBuilder( core::runtime::ConfigurationElement::sptr menuBarConfig );
    void initializeToolBarBuilder( core::runtime::ConfigurationElement::sptr toolBarConfig );

    gui::layoutManager::IFrameLayoutManager::sptr m_frameLayoutManager;
    gui::registrar::ViewRegistrar::sptr m_viewRegistrar;
    gui::builder::IMenuBarBuilder::sptr m_menuBarBuilder;
    gui::builder::IToolBarBuilder::sptr m_toolBarBuilder;

    ConfigurationType m_registrarConfig;
    ConfigurationType m_frameConfig;
    ConfigurationType m_menuBarConfig;
    ConfigurationType m_toolBarConfig;

    bool m_hasMenuBar;
    bool m_hasToolBar;

    std::string m_closePolicy;

    /// Signal emitted when frame is closed and onclose mode is message
    ClosedSignalType::sptr m_sigClosed;
};

} // namespace sight::gui
