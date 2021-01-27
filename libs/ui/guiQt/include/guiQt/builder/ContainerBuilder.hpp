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

#include <gui/builder/IContainerBuilder.hpp>
#include <gui/container/fwContainer.hpp>

namespace sight::gui
{
namespace builder
{

/**
 * @brief   Defines the generic layout manager for IHM.
 *
 */
class GUIQT_CLASS_API ContainerBuilder : public gui::builder::IContainerBuilder
{
public:
    fwCoreClassMacro(ContainerBuilder, gui::builder::IContainerBuilder, gui::factory::New< ContainerBuilder >)

    GUIQT_API ContainerBuilder(gui::GuiBaseObject::Key key);

    GUIQT_API virtual ~ContainerBuilder();

    /**
     * @brief Instantiate layout with parent container.
     * @pre LayoutManager must be initialized before.
     * @pre parent container must be instanced.
     */
    GUIQT_API virtual void createContainer( gui::container::fwContainer::sptr parent ) override;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    GUIQT_API virtual void destroyContainer() override;

    GUIQT_API virtual void setParent(gui::container::fwContainer::sptr parent) override;

protected:
    guiQt::container::QtContainer::sptr m_parent;
};

} // namespace builder
} // namespace sight::gui
