/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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
#include "ui/base/layoutManager/IViewLayoutManager.hpp"

namespace sight::ui::base::layoutManager
{

/**
 * @brief   Defines the base for line layout manager.
 */
class UI_BASE_CLASS_API OverlayLayoutManager : public IViewLayoutManager
{
public:

    SIGHT_DECLARE_CLASS(OverlayLayoutManager, ui::base::layoutManager::IViewLayoutManager);

    struct View
    {
        struct Coord
        {
            bool relative = false;
            int value     = 0;
        };
        Coord x;
        Coord y;
        Coord width;
        Coord height;
        int minWidth {0};
        int minHeight {0};
        bool visible {true};
    };

    /**
     * @brief Initialize Overlay layout manager before the creation of layout.
     *
     * Example of configuration with overlay layout.
     * @code{.xml}
       <service uid="mainView" type="sight::module::ui::base::SView">
            <gui>
                <layout type="sight::ui::base::OverlayLayoutManager">
                    <view />
                    <view x="0" y="0" minWidth="55" minHeight="100" />
                    <view x="-1" y="0" width="50" height="55" visible="false" />
                    <view x="0" y="-1" height="35" width="100%" visible="false" />
                    <view x="-50%" y="0" width="400" height="300" />
                </layout>
            </gui>
            <registry>
                <parent wid="${WID_PARENT}" />
                <view sid="scenesView" start="true" />
                <view sid="topToolbarSliderView" start="true" />
                <view sid="rightToolbarSliderView" start="true" />
                <view sid="videoSliderView" start="true" />
                <view sid="advancedQueryEditorSliderView" start="true" />
            </registry>
        </service>

       @endcode
     *  - \<layout type="ui::base::OverlayLayoutManager" \> : define a overlay layout.
     *  - \<view /\> : define a new view. The first view is the background widget and the other ones are the overlays.
                       The overlay views can have the following attributes:
     *   - \b x : The horizontal offset of the widget. If positive, from left; if negative, from right
     *   - \b y : The vertical offset of the widget. If positive, from top; if negative, from bottom
     *   - \b width : The fixed weight of the widget
     *   - \b height : The fixed height of the widget
     *   - \b minWidth : The minimum width of the widget
     *   - \b minHeight : The minimum height of the widget
     *   - \b visible : Whether the widget should be shown on application start (default: true)
     */

    UI_BASE_API void initialize(const ui::base::config_t& configuration) override;

    static inline std::string REGISTRY_KEY = "sight::ui::base::OverlayLayoutManager";

protected:

    //------------------------------------------------------------------------------

    const std::vector<View>& views() const
    {
        return m_views;
    }

private:

    /// Save layout configuration definition
    std::vector<View> m_views;
};

} // namespace sight::ui::base::layoutManager
