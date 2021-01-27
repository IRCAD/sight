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

#include "gui/config.hpp"
#include "gui/container/fwContainer.hpp"
#include "gui/container/fwMenuBar.hpp"
#include "gui/GuiBaseObject.hpp"

#include <core/runtime/ConfigurationElement.hpp>

namespace sight::gui
{
namespace builder
{

/**
 * @brief   Defines the interface class  for the menubar builder.
 */
class GUI_CLASS_API IMenuBarBuilder : public gui::GuiBaseObject
{
public:
    fwCoreClassMacro(IMenuBarBuilder, gui::GuiBaseObject)

    typedef std::string RegistryKeyType;

    GUI_API const static RegistryKeyType REGISTRY_KEY;

    /// Constructor. Do nothing.
    GUI_API IMenuBarBuilder();

    /// Destructor. Do nothing.
    GUI_API virtual ~IMenuBarBuilder();

    /**
     * @brief Returns the builded menu bar.
     */
    GUI_API virtual gui::container::fwMenuBar::sptr getMenuBar();

    /**
     * @brief Configure the layout before creation.
     */
    GUI_API virtual void initialize( core::runtime::ConfigurationElement::sptr configuration);

    /**
     * @brief Instantiate layout with parent menuBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent menuBar must be instanced.
     */
    GUI_API virtual void createMenuBar( gui::container::fwContainer::sptr parent ) = 0;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    GUI_API virtual void destroyMenuBar() = 0;

protected:

    /// MenuBar.
    gui::container::fwMenuBar::sptr m_menuBar;

    /// Background color. Use an empty string to use the default background color, else, set an hexadecimal value.
    std::string m_backgroundColor;

};

} // namespace builder
} // namespace sight::gui
