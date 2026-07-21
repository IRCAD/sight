/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <service/base.hpp>

#include <ui/__/object.hpp>
#include <ui/__/parameter.hpp>

namespace sight::ui
{

// Forward declarations
namespace container
{

class widget;

} // namespace container

namespace layout
{

class view;

} // namespace layout

namespace detail::registry
{

class view;

} // namespace detail::registry

namespace builder
{

class toolbar;
class widget;
class slideview;

} // namespace builder

/**
 * @brief   Defines the service interface managing the layout.
 *
 * @section Slots Slots
 *
 * - \b set_enabled(bool enabled) : this slot enables the container (if enabled = true) or disables it.
 * - \b set_enabled_by_param(ui::parameter_t enabled) : this slot enables the container (if enabled holds boolean
 * alternative) or disables it.
 * - \b enable() : this slot enables the container.
 * - \b disable() : this slot disables the container
 * - \b set_visible(bool visible) : this slot shows the container (if visible = true) or hides it.
 * - \b show() : this slot shows the container.
 * - \b hide() : this slot hides the container.
 * - \b toggle_visibility() : this slot shows the container if it is hidden or hides it if it is shown
 * - \b set_blurred(bool blurred) : this slot blurs the container (if blurred = true) or unblurs it.
 * - \b blur() : this slot blurs the container.
 * - \b unblur() : this slot unblurs the container.
 * - \b modify_layout(ui::parameter_t parameter, std::string key) : this slot modifies a layout element, depending
 *                                                                  of the key.
 *
 * @section XML Example of XML configuration
 *
 * @code{.xml}
   <service uid="..." type="sight::module::ui::view" >
       <gui>
           <layout type="sight::ui::layout::line" >
               <orientation value="horizontal" />
               <view caption="view3" />
               <view caption="view4" />
               <view caption="view5" />
           </layout>
           <toolbar />
           <slideView />
           <slideView align="top" size="200" opacity="1.0">
               <styleSheet>color: blue; background-color: yellow</styleSheet>
           </slideView>
       </gui>
       <registry>
           <parent wid="my_view" />
           <toolbar sid="toolbar1" />
           <view sid="sub_view3" />
           <view wid="sub_view4" />
           <view sid="sub_view5" />
           <slideView sid="slide_view1" />
           <slideView wid="slide_view2" />
        </registry>
   </service>
   @endcode
 *  - \<layout type="ui::layout::line" \> : give the type of layout.
 *    - \b type {sight::ui::layout::line | sight::ui::layout::cardinal | sight::ui::layout::tab |
 *               sight::ui::layout::toolbox} :
 *     - \b sight::ui::layout::line : all views will be on the same line or column (it depends of the orientation
 *           value attribute)
 *           @see sight::ui::layout::line
 *     - \b sight::ui::layout::cardinal : all views will be added around a central view define by the align
 * attribute.
 *           @see sight::ui::layout::cardinal
 *     - \b sight::ui::layout::tab : all views will be draw as tab.
 *           @see sight::ui::layout::tab
 *     - \b sight::ui::layout::toolbox : all views will be draw in toolbox.
 *           @see sight::ui::layout::toolbox
 *  - \b toolbar: defines the toolbar configuration.
 *           @see ui::builder::toolbar
 *  - \b slideView: defines a slide view.
 *           @see ui::builder::slideview
 *
 * @note The layout and registry sections can be empty. In this case no subview will be created.
 *
 * @warning
 * - The order of the view in each section (gui and registry) must be the same.\n
 *   For example: the view caption "view3" will be connected with the service which have the sid =sub_view3" and so one
 *   (it also could be a wid).
 *
 */
class SIGHT_UI_CLASS_API service : public sight::service::base
{
public:

    SIGHT_DECLARE_SERVICE(service, service::base);

    SIGHT_UI_API ~service() override = default;

    SIGHT_UI_API SPTR(ui::container::widget) get_container();

    SIGHT_UI_API void set_parent(std::string _wid);
    /// @name Slots
    /// @{

    struct SIGHT_UI_CLASS_API slots : public sight::service::slots
    {
        inline static const slot_key_t SET_ENABLED       = "set_enabled";
        inline static const slot_key_t ENABLE            = "enable";
        inline static const slot_key_t DISABLE           = "disable";
        inline static const slot_key_t SET_VISIBLE       = "set_visible";
        inline static const slot_key_t SHOW              = "show";
        inline static const slot_key_t HIDE              = "hide";
        inline static const slot_key_t TOGGLE_VISIBILITY = "toggle_visibility";
        inline static const slot_key_t SET_BLURRED       = "set_blurred";
        inline static const slot_key_t BLUR              = "blur";
        inline static const slot_key_t UNBLUR            = "unblur";
        inline static const slot_key_t MODIFY_LAYOUT     = "modify_layout";
    };

    /// SLOT: enable/disable the container
    SIGHT_UI_API virtual void set_enabled(bool _enabled);

    /// SLOT: enable the container
    SIGHT_UI_API virtual void enable();

    /// SLOT: disable the container
    SIGHT_UI_API virtual void disable();

    /// SLOT: show/hide the container
    SIGHT_UI_API virtual void set_visible(bool _visible);

    /// SLOT: show the container
    SIGHT_UI_API virtual void show();

    /// SLOT: hide the container
    SIGHT_UI_API virtual void hide();

    /// SLOT: show the container if it is hidden or hide it if it is shown
    SIGHT_UI_API void toggle_visibility();

    /// SLOT: blur/unblur the container
    SIGHT_UI_API void set_blurred(bool _blurred);

    /// SLOT: modify a layout element, depending of the key. Forwarded to the view layout manager.
    SIGHT_UI_API virtual void modify_layout(ui::parameter_t _parameter, std::string _key);

    /// @}

protected:

    SIGHT_UI_API service();

    /**
     * @brief Initialize managers.
     *
       @see ui::registry::view::initialize(), ui::layout::view::initialize(),
     *::ui::builder::toolbar::initialize()
     */
    SIGHT_UI_API void initialize();

    /**
     * @brief Creates view, sub-views and toolbar containers. Manages sub-views and toobar services.
     *
     * @pre Parent container must be registered.
     */
    SIGHT_UI_API void create();

    /// Stops sub-views and toobar services. Destroys view, sub-views and toolbar containers.
    SIGHT_UI_API void destroy();

private:

    using slide_view_container_t = std::vector<std::shared_ptr<ui::builder::slideview> >;

    void initialize_layout_manager(const ui::config_t& _layout_config);
    void initialize_tool_bar_builder(const ui::config_t& _tool_bar_config);
    void initialize_slide_view_builder(const ui::config_t& _slide_view_config);

    bool m_view_layout_manager_is_created {false};
    SPTR(ui::layout::view) m_view_layout_manager;

    SPTR(ui::detail::registry::view) m_view_registry;
    SPTR(ui::builder::toolbar) m_tool_bar_builder;
    SPTR(ui::builder::widget) m_container_builder;
    slide_view_container_t m_slide_view_builders;

    bool m_has_tool_bar {false};
};

} // namespace sight::ui
