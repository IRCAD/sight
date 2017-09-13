/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_IFRAMESRV_HPP__
#define __FWGUI_IFRAMESRV_HPP__

#include "fwGui/builder/IMenuBarBuilder.hpp"
#include "fwGui/builder/IToolBarBuilder.hpp"
#include "fwGui/config.hpp"
#include "fwGui/container/fwContainer.hpp"
#include "fwGui/layoutManager/IFrameLayoutManager.hpp"
#include "fwGui/registrar/ViewRegistrar.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwServices/IService.hpp>

namespace fwGui
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
   <service uid="mainFrame" type="::fwGui::IFrameSrv" impl="::gui::frame::SDefaultFrame" autoConnect="no">
     <window onclose="notify" />
     <gui>
         <frame>
             <name>My App</name>
             <icon>myApp_1-0/icon.ico</icon>
             <minSize width="800" height="600" />
             <style mode="MODAL" />
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
 * - The toolBar section isn't mandatory.
 * - The menuBar section isn't mandatory.

 */
class FWGUI_CLASS_API IFrameSrv : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro( (IFrameSrv)(::fwServices::IService) );

    /// Get widget defined for progress bar
    FWGUI_API static ::fwGui::container::fwContainer::sptr getProgressWidget();

    /**
     * @name Signals
     * @{
     */
    /// Signal emitted when frame is closed and onclose policy is notify
    static const ::fwCom::Signals::SignalKeyType s_CLOSED_SIG;
    typedef ::fwCom::Signal< void ()> ClosedSignalType;
    /**
     * @}
     */

    /**
     * @name Slots Keys
     * @{
     */

    /// Slot to show/hide the container
    static const ::fwCom::Slots::SlotKeyType s_SET_VISIBLE_SLOT;

    /// Slot to show the container
    static const ::fwCom::Slots::SlotKeyType s_SHOW_SLOT;

    /// Slot to hide the container
    static const ::fwCom::Slots::SlotKeyType s_HIDE_SLOT;
    /**
     * @}
     */

protected:

    FWGUI_API IFrameSrv();

    FWGUI_API virtual ~IFrameSrv();

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    /**
     * @brief Initialize frame managers.
     *
     * @see ::fwGui::registrar::ViewRegistrar::initialize(), ::fwGui::layoutManager::IFrameLayoutManager::initialize(),
     *      ::fwGui::builder::IToolBarBuilder::initialize(), ::fwGui::builder::IMenuBarBuilder::initialize()
     */
    FWGUI_API void initialize();

    /// Creates frame, sub-view, menubar and toolbar containers. Manages sub-view, menubar and toobar services.
    FWGUI_API void create();

    /// Stops sub-view, menubar and toobar services. Destroys frame, sub-view, menubar and toolbar containers.
    FWGUI_API void destroy();

    FWGUI_API static const std::string CLOSE_POLICY_EXIT;
    FWGUI_API static const std::string CLOSE_POLICY_NOTIFY;
    FWGUI_API static const std::string CLOSE_POLICY_MESSAGE;

    /// Static reference on a widget defined for progress bar installation
    FWGUI_API static ::fwGui::container::fwContainer::wptr m_progressWidget;

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
    void initializeLayoutManager( ::fwRuntime::ConfigurationElement::sptr layoutConfig );
    void initializeMenuBarBuilder( ::fwRuntime::ConfigurationElement::sptr menuBarConfig );
    void initializeToolBarBuilder( ::fwRuntime::ConfigurationElement::sptr toolBarConfig );

    ::fwGui::layoutManager::IFrameLayoutManager::sptr m_frameLayoutManager;
    ::fwGui::registrar::ViewRegistrar::sptr m_viewRegistrar;
    ::fwGui::builder::IMenuBarBuilder::sptr m_menuBarBuilder;
    ::fwGui::builder::IToolBarBuilder::sptr m_toolBarBuilder;

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

} // namespace fwGui

#endif /*__FWGUI_IFRAMESRV_HPP__*/

