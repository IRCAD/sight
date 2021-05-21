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

#include "ui/base/config.hpp"

#include <core/com/Slot.hpp>
#include <core/com/Slots.hpp>

#include <service/IService.hpp>

namespace sight::ui::base
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
namespace registry
{

class View;

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
   <service uid="subView1" type="sight::module::ui::base::SView" autoConnect="false" >
       <gui>
           <layout type="ui::base::LineLayoutManager" >
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
 *  - \<layout type="ui::base::LineLayoutManager" \> : give the type of layout.
 *    - \b type {::ui::base::LineLayoutManager | ui::base::CardinalLayoutManager | ui::base::TabLayoutManager |
 *              ui::base::ToolboxLayoutManager} :
 *     - \b ui::base::LineLayoutManager : all views will be on the same line or column (it depends of the orientation
 *           value attribute)
 *           @see ui::base::layoutManager::LineLayoutManagerBase
 *     - \b ui::base::CardinalLayoutManager : all views will be added around a central view define by the align
 * attribute.
 *           @see ui::base::layoutManager::CardinalLayoutManagerBase
 *     - \b ui::base::TabLayoutManager : all views will be draw as tab.
 *           @see ui::base::layoutManager::TabLayoutManagerBase
 *     - \b ui::base::ToolboxLayoutManager : all views will be draw in toolbox.
 *           @see ui::base::layoutManager::ToolboxLayoutManagerBase
 *  - \b toolBar: defines the toolBar configuration.
 *           @see ui::base::builder::IToolBarBuilder
 *  - \b slideView: defines a slide view.
 *           @see ui::base::builder::ISlideViewBuilder
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
class UI_BASE_CLASS_API IGuiContainer : public service::IService
{
public:

    SIGHT_DECLARE_SERVICE(IGuiContainer, service::IService)

    UI_BASE_API SPTR(ui::base::container::fwContainer) getContainer();

    UI_BASE_API void setParent(std::string wid);

protected:

    UI_BASE_API IGuiContainer();

    UI_BASE_API virtual ~IGuiContainer();

    typedef core::runtime::ConfigurationElement::sptr ConfigurationType;

    /**
     * @brief Initialize managers.
     *
       @see ui::base::registry::View::initialize(), ui::base::layoutManager::IViewLayoutManager::initialize(),
     *::ui::base::builder::IToolBarBuilder::initialize()
     */
    UI_BASE_API void initialize();

    /**
     * @brief Creates view, sub-views and toolbar containers. Manages sub-views and toobar services.
     *
     * @pre Parent container must be registered.
     */
    UI_BASE_API void create();

    /// Stops sub-views and toobar services. Destroys view, sub-views and toolbar containers.
    UI_BASE_API void destroy();

    /**
     * @name Slots
     * @{
     */

    /// Slot to enable/disable the action
    UI_BASE_API static const core::com::Slots::SlotKeyType s_SET_ENABLED_SLOT;

    /// Slot to enable the container
    UI_BASE_API static const core::com::Slots::SlotKeyType s_ENABLE_SLOT;

    /// Slot to disable the container
    UI_BASE_API static const core::com::Slots::SlotKeyType s_DISABLE_SLOT;

    /// Slot to show/hide the container
    UI_BASE_API static const core::com::Slots::SlotKeyType s_SET_VISIBLE_SLOT;

    /// Slot to show the container
    UI_BASE_API static const core::com::Slots::SlotKeyType s_SHOW_SLOT;

    /// Slot to hide the container
    UI_BASE_API static const core::com::Slots::SlotKeyType s_HIDE_SLOT;

    /// SLOT: enable/disable the container
    UI_BASE_API void setEnabled(bool isEnabled);

    /// SLOT: enable the container
    UI_BASE_API void enable();

    /// SLOT: disable the container
    UI_BASE_API void disable();

    /// SLOT: show/hide the container
    UI_BASE_API void setVisible(bool isVisible);

    /// SLOT: show the container
    UI_BASE_API void show();

    /// SLOT: hide the container
    UI_BASE_API void hide();

/**
 * @}
 */

private:

    typedef std::vector<SPTR(ui::base::builder::ISlideViewBuilder)> SlideViewContainerType;

    void initializeLayoutManager(core::runtime::ConfigurationElement::sptr layoutConfig);
    void initializeToolBarBuilder(core::runtime::ConfigurationElement::sptr toolBarConfig);
    void initializeSlideViewBuilder(core::runtime::ConfigurationElement::sptr slideViewConfig);

    bool m_viewLayoutManagerIsCreated;
    SPTR(ui::base::layoutManager::IViewLayoutManager) m_viewLayoutManager;

    SPTR(ui::base::registry::View) m_viewRegistry;
    SPTR(ui::base::builder::IToolBarBuilder) m_toolBarBuilder;
    SPTR(ui::base::builder::IContainerBuilder) m_containerBuilder;
    SlideViewContainerType m_slideViewBuilders;

    ConfigurationType m_viewRegistryConfig;
    ConfigurationType m_viewLayoutConfig;
    ConfigurationType m_toolBarConfig;
    ConfigurationType m_slideViewConfig;

    bool m_hasToolBar;
};

} // namespace sight::ui::base
