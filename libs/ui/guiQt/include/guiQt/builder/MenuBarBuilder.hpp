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

#include "guiQt/config.hpp"
#include "guiQt/container/QtContainer.hpp"

#include <core/base.hpp>
#include <core/runtime/ConfigurationElement.hpp>

#include <gui/builder/IMenuBarBuilder.hpp>
#include <gui/container/fwContainer.hpp>
#include <gui/container/fwMenuBar.hpp>
namespace sight::gui
{
namespace builder
{

/**
 * @brief   Defines the generic layout manager for IHM.
 *
 */
class GUIQT_CLASS_API MenuBarBuilder : public gui::builder::IMenuBarBuilder
{
public:
    fwCoreClassMacro(MenuBarBuilder, gui::builder::IMenuBarBuilder, gui::factory::New< MenuBarBuilder >)

    GUIQT_API MenuBarBuilder(gui::GuiBaseObject::Key key);

    GUIQT_API virtual ~MenuBarBuilder();

    /**
     * @brief Instantiate layout with parent menuBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent menuBar must be instanced.
     */
    GUIQT_API virtual void createMenuBar( gui::container::fwContainer::sptr parent ) override;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    GUIQT_API virtual void destroyMenuBar() override;

protected:
    guiQt::container::QtContainer::sptr m_parent;
};

} // namespace builder
} // namespace sight::gui
