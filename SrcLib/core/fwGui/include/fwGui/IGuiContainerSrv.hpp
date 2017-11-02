/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_IGUICONTAINERSRV_HPP__
#define __FWGUI_IGUICONTAINERSRV_HPP__

#include "fwGui/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwServices/IService.hpp>

namespace fwGui
{
// Forward declarations
namespace container
{
class fwContainer;
}
namespace layoutManager
{
class IViewLayoutManager;
}
namespace registrar
{
class ViewRegistrar;
}
namespace builder
{
class IToolBarBuilder;
class IContainerBuilder;
class ISlideViewBuilder;
}

/**
 * @brief   Defines the service interface managing the layout.
 *
 * @section Slots Slots
 *
 * - \b setEnabled(bool isEnabled) : this slot enables the container (if isEnabled = true) or disables it.
 * - \b enable() : this slot enables the container.
 * - \b disable() : this slot disables the container
 * - \b setVisible(bool isVisible) : this slot shows the container (if isVisible = true) or hides it.
 * - \b show() : this slot shows the container.
 * - \b hide() : this slot hides the container.
 *
 * @section XML Example of XML configuration
 *
 * @code{.xml}
   <service uid="subView1" type="::gui::view::IView" impl="::gui::view::SDefaultView" autoConnect="no" >
       <gui>
           <layout type="::fwGui::LineLayoutManager" >
               <orientation value="horizontal" />
               <view caption="view3" />
               <view caption="view4" />
               <view caption="view5" />
           </layout>
           <toolBar />
           <slideView />
           <slideView align="top" size="200" opacity="1.0">
               <styleSheet>color: blue; background-color: yellow</styleSheet>
           </slideView>
       </gui>
       <registry>
           <parent wid="myView" />
           <toolBar sid="toolbar1" start="yes" />
           <view sid="subView3" start="yes" />
           <view wid="subView4" />
           <view sid="subView5" />
           <slideView sid="slideView1" start="yes />
           <slideView wid="slideView2" />
        </registry>
   </service>
   @endcode
 *  - \<layout type="::fwGui::LineLayoutManager" \> : give the type of layout.
 *    - \b type {::fwGui::LineLayoutManager | ::fwGui::CardinalLayoutManager | ::fwGui::TabLayoutManager |
 *              ::fwGui::ToolboxLayoutManager} :
 *     - \b ::fwGui::LineLayoutManager : all views will be on the same line or column (it depends of the orientation
 *           value attribute)
 *           @see ::fwGui::layoutManager::LineLayoutManagerBase
 *     - \b ::fwGui::CardinalLayoutManager : all views will be added around a central view define by the align
 * attribute.
 *           @see ::fwGui::layoutManager::CardinalLayoutManagerBase
 *     - \b ::fwGui::TabLayoutManager : all views will be draw as tab.
 *           @see ::fwGui::layoutManager::TabLayoutManagerBase
 *     - \b ::fwGui::ToolboxLayoutManager : all views will be draw in toolbox.
 *           @see ::fwGui::layoutManager::ToolboxLayoutManagerBase
 *  - \b toolBar: defines the toolBar configuration.
 *           @see ::fwGui::builder::IToolBarBuilder
 *  - \b slideView: defines a slide view.
 *           @see ::fwGui::builder::ISlideViewBuilder
 *
 * @note The layout and registry sections can be empty. In this case no subview will be created.
 *
 * @warning
 * - The order of the view in each section (gui and registry) must be the same.\n
 *   For example: the view caption "view3" will be connected with the service which have the sid = "subView3" and so one
 *   (it also could be a wid).
 *
 *
 */
class FWGUI_CLASS_API IGuiContainerSrv : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro( (IGuiContainerSrv)(::fwServices::IService) );

    FWGUI_API SPTR(::fwGui::container::fwContainer) getContainer();

    FWGUI_API void setParent(std::string wid);

protected:

    FWGUI_API IGuiContainerSrv();

    FWGUI_API virtual ~IGuiContainerSrv();

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /**
     * @brief Initialize managers.
     *
       @see ::fwGui::registrar::ViewRegistrar::initialize(), ::fwGui::layoutManager::IViewLayoutManager::initialize(),
     *::fwGui::builder::IToolBarBuilder::initialize()
     */
    FWGUI_API void initialize();

    /**
     * @brief Creates view, sub-views and toolbar containers. Manages sub-views and toobar services.
     *
     * @pre Parent container must be registered.
     */
    FWGUI_API void create();

    /// Stops sub-views and toobar services. Destroys view, sub-views and toolbar containers.
    FWGUI_API void destroy();

    /**
     * @name Slots
     * @{
     */
    /// Slot to enable/disable the action
    FWGUI_API static const ::fwCom::Slots::SlotKeyType s_SET_ENABLED_SLOT;

    /// Slot to enable the container
    FWGUI_API static const ::fwCom::Slots::SlotKeyType s_ENABLE_SLOT;

    /// Slot to disable the container
    FWGUI_API static const ::fwCom::Slots::SlotKeyType s_DISABLE_SLOT;

    /// Slot to show/hide the container
    FWGUI_API static const ::fwCom::Slots::SlotKeyType s_SET_VISIBLE_SLOT;

    /// Slot to show the container
    FWGUI_API static const ::fwCom::Slots::SlotKeyType s_SHOW_SLOT;

    /// Slot to hide the container
    FWGUI_API static const ::fwCom::Slots::SlotKeyType s_HIDE_SLOT;

    /// SLOT: enable/disable the container
    FWGUI_API void setEnabled(bool isEnabled);
    /// SLOT: enable the container
    FWGUI_API void enable();
    /// SLOT: disable the container
    FWGUI_API void disable();
    /// SLOT: show/hide the container
    FWGUI_API void setVisible(bool isVisible);
    /// SLOT: show the container
    FWGUI_API void show();
    /// SLOT: hide the container
    FWGUI_API void hide();

    /**
     * @}
     */

private:

    typedef std::vector< SPTR(::fwGui::builder::ISlideViewBuilder) > SlideViewContainerType;

    void initializeLayoutManager( ::fwRuntime::ConfigurationElement::sptr layoutConfig );
    void initializeToolBarBuilder( ::fwRuntime::ConfigurationElement::sptr toolBarConfig );
    void initializeSlideViewBuilder( ::fwRuntime::ConfigurationElement::sptr slideViewConfig );

    bool m_viewLayoutManagerIsCreated;
    SPTR( ::fwGui::layoutManager::IViewLayoutManager) m_viewLayoutManager;

    SPTR( ::fwGui::registrar::ViewRegistrar) m_viewRegistrar;
    SPTR( ::fwGui::builder::IToolBarBuilder) m_toolBarBuilder;
    SPTR( ::fwGui::builder::IContainerBuilder) m_containerBuilder;
    SlideViewContainerType m_slideViewBuilders;

    ConfigurationType m_viewRegistrarConfig;
    ConfigurationType m_viewLayoutConfig;
    ConfigurationType m_toolBarConfig;
    ConfigurationType m_slideViewConfig;

    bool m_hasToolBar;
};

} // namespace fwGui

#endif /*__FWGUI_IGUICONTAINERSRV_HPP__*/
