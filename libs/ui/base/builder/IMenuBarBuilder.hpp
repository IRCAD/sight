/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include "ui/base/container/fwContainer.hpp"
#include "ui/base/container/fwMenuBar.hpp"
#include "ui/base/GuiBaseObject.hpp"

namespace sight::ui::base::builder
{

/**
 * @brief   Defines the interface class  for the menubar builder.
 */
class UI_BASE_CLASS_API IMenuBarBuilder : public ui::base::GuiBaseObject
{
public:

    SIGHT_DECLARE_CLASS(IMenuBarBuilder, ui::base::GuiBaseObject);

    typedef std::string RegistryKeyType;

    UI_BASE_API const static RegistryKeyType REGISTRY_KEY;

    /// Constructor. Do nothing.
    UI_BASE_API IMenuBarBuilder() = default;

    /// Destructor. Do nothing.
    UI_BASE_API ~IMenuBarBuilder() override = default;

    /**
     * @brief Returns the builded menu bar.
     */
    UI_BASE_API virtual ui::base::container::fwMenuBar::sptr getMenuBar();

    /**
     * @brief Configure the layout before creation.
     */
    UI_BASE_API virtual void initialize(const ui::base::config_t& configuration);

    /**
     * @brief Instantiate layout with parent menuBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent menuBar must be instanced.
     */
    UI_BASE_API virtual void createMenuBar(ui::base::container::fwContainer::sptr parent) = 0;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    UI_BASE_API virtual void destroyMenuBar() = 0;

protected:

    /// MenuBar.
    ui::base::container::fwMenuBar::sptr m_menuBar;

    /// Background color. Use an empty string to use the default background color, else, set an hexadecimal value.
    std::string m_backgroundColor;
};

} // namespace sight::ui::base::builder
