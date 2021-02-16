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

#include "ui/base/builder/IMenuBarBuilder.hpp"
#include "ui/base/builder/IToolBarBuilder.hpp"
#include "ui/base/config.hpp"
#include "ui/base/container/fwContainer.hpp"
#include "ui/base/layoutManager/IFrameLayoutManager.hpp"
#include "ui/base/registrar/ViewRegistrar.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>

#include <service/IService.hpp>

namespace sight::ui::base
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
   <service uid="mainFrame" type="::ui::base::IFrameSrv" impl="::sight::modules::ui::base::SFrame"
 * autoConnect="no">
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
class UI_BASE_CLASS_API IFrameSrv : public service::IService
{

public:

    fwCoreServiceMacro(IFrameSrv, service::IService)

    /// Get widget defined for progress bar
    UI_BASE_API static ui::base::container::fwContainer::sptr getProgressWidget();

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

    UI_BASE_API IFrameSrv();

    UI_BASE_API virtual ~IFrameSrv();

    typedef core::runtime::ConfigurationElement::sptr ConfigurationType;
    /**
     * @brief Initialize frame managers.
     *
     * @see ui::base::registrar::ViewRegistrar::initialize(),
     * ui::base::layoutManager::IFrameLayoutManager::initialize(),
     *      ui::base::builder::IToolBarBuilder::initialize(), ui::base::builder::IMenuBarBuilder::initialize()
     */
    UI_BASE_API void initialize();

    /// Creates frame, sub-view, menubar and toolbar containers. Manages sub-view, menubar and toobar services.
    UI_BASE_API void create();

    /// Stops sub-view, menubar and toobar services. Destroys frame, sub-view, menubar and toolbar containers.
    UI_BASE_API void destroy();

    UI_BASE_API static const std::string CLOSE_POLICY_EXIT;
    UI_BASE_API static const std::string CLOSE_POLICY_NOTIFY;
    UI_BASE_API static const std::string CLOSE_POLICY_MESSAGE;

    /// Static reference on a widget defined for progress bar installation
    UI_BASE_API static ui::base::container::fwContainer::wptr m_progressWidget;

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

    ui::base::layoutManager::IFrameLayoutManager::sptr m_frameLayoutManager;
    ui::base::registrar::ViewRegistrar::sptr m_viewRegistrar;
    ui::base::builder::IMenuBarBuilder::sptr m_menuBarBuilder;
    ui::base::builder::IToolBarBuilder::sptr m_toolBarBuilder;

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

} // namespace sight::ui::base
