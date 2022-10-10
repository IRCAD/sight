/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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
#include "ui/base/container/fwContainer.hpp"
#include "ui/base/container/fwToolBar.hpp"
#include "ui/base/GuiBaseObject.hpp"

namespace sight::ui::base::builder
{

/**
 * @brief   Defines the interface class  for the toolbar builder.
 */
class UI_BASE_CLASS_API IToolBarBuilder : public ui::base::GuiBaseObject
{
public:

    SIGHT_DECLARE_CLASS(IToolBarBuilder, ui::base::GuiBaseObject);

    typedef std::string RegistryKeyType;

    UI_BASE_API const static RegistryKeyType REGISTRY_KEY;

    typedef enum
    {
        TOP,
        BOTTOM,
        RIGHT,
        LEFT
    } Alignment;

    /// Constructor. Do nothing.
    UI_BASE_API IToolBarBuilder();

    /// Destructor. Do nothing.
    UI_BASE_API ~IToolBarBuilder() override = default;

    /**
     * @brief Returns the builded tool bar.
     */
    UI_BASE_API virtual ui::base::container::fwToolBar::sptr getToolBar();

    /**
     * @brief Initialize the tool bar.
     *
     * Example of configuration
     * @code{.xml}
        <toolBar align="top" backgroundColor="default" >
            <toolBitmapSize height= "50" width="50" />
        </toolBar>
       @endcode
        - \b align : toolbar alignment (top, bottom, left, right).
        - \b backgroundColor (optional) : (hexa) background color.
     *  - \<toolBitmapSize height= "50" width="50"/\> : give the size of the icon.
     */

    UI_BASE_API virtual void initialize(const ui::base::config_t& configuration);

    /**
     * @brief Instantiate layout with parent toolBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent toolBar must be instanced.
     */
    UI_BASE_API virtual void createToolBar(ui::base::container::fwContainer::sptr parent) = 0;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    UI_BASE_API virtual void destroyToolBar() = 0;

protected:

    /// ToolBar.
    ui::base::container::fwToolBar::sptr m_toolBar;

    std::pair<int, int> m_toolBitmapSize;

    Alignment m_alignment {TOP};

    /// Background color. Use an empty string to use the default background color, else, set an hexadecimal value.
    std::string m_backgroundColor;
};

} // namespace sight::ui::base::builder
