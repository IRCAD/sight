/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include "core/macros.hpp"

#include "ui/__/service.hpp"

#include <ui/__/toolbar.hpp>
#include <ui/qt/widget/speed_dial.hpp>

#include <QShortcut>

namespace sight::module::ui::qt
{

/**
 * A service which allows to display a menu as a speed dial.

 * Example of configuration:
 * @code{.xml}
    <service uid="iconSpeedDialSrv" type="sight::module::ui::qt::icon_speed_dial">
        <config direction="right" spacing="10" icon="sight::module::ui::flaticons/BlueLoad.svg" />
        <actions>
            <action sid="openImageAct" name="Open image" icon="sight::module::ui::flaticons/OpenImage.svg"
                    shortcut="Ctrl+I" />
            <action sid="openMeshAct" name="Open mesh" icon="sight::module::ui::flaticons/OpenMesh.svg"
                    shortcut="Ctrl+M" />
            <action sid="openTextureAct" name="Open texture" icon="sight::module::ui::flaticons/OpenTexture.svg"
                    shortcut="Ctrl+T" />
        </actions>
    </service>
    @endcode
 *  - \<config\>
 *    - direction (enum, mandatory): The direction where the buttons are to be shown. Either "up", "right", "down" or
 *                                   "left".
 *    - spacing (int, default=10): The space, in pixels, between the buttons.
 *    - icon (string, mandatory): The icon of the button.
 *    - unfoldedIcon (string, default=""): The icon of the button when it is unfolded. Defaults to the icon when folded
 *                                         if unspecified.
 *    - animationDuration (int, default=250): The duration, in milliseconds, of the animation when folding/unfolding.
 *  - \<actions\>
 *    - \<action\>
 *      - sid (string, mandatory): The sid of the related action.
 *      - name (string, default=""): The name of the button (shown in the tooltip).
 *      - icon (string, mandatory): The icon of the button.
 *      - shortcut (string, default=""): The shortcut associated to the action. If unspecified, no shortcuts are
 *                                       created.
 */
class icon_speed_dial : public sight::ui::service
{
public:

    icon_speed_dial();

    struct action
    {
        std::string sid;
        std::string name;
        std::string icon;
        std::string shortcut;
    };

    SIGHT_DECLARE_SERVICE(icon_speed_dial, sight::ui::service);

    struct slots
    {
        static inline const core::com::slots::key_t FOLD           = "fold";
        static inline const core::com::slots::key_t UNFOLD         = "unfold";
        static inline const core::com::slots::key_t UPDATE_ACTIONS = "updateActions";
    };

    void configuring() override;
    void starting() override;
    void updating() override;
    void stopping() override;

private:

    void update_actions();

    enum sight::ui::qt::widget::speed_dial::direction m_direction = sight::ui::qt::widget::speed_dial::direction::up;
    int m_spacing                                                 = -1;
    std::string m_icon;
    std::string m_unfolded_icon;
    int m_animation_duration = -1;
    std::vector<action> m_actions;
    std::vector<QWidget*> m_widgets;

    sight::ui::qt::widget::speed_dial* m_speed_dial = nullptr;
    std::vector<QShortcut*> m_shortcuts;
};

} // namespace sight::module::ui::qt
