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

/**
 * @brief This file defines the interface of the base class for managing a layout geometry.
 */

#pragma once

#include "ui/base/config.hpp"
#include "ui/base/container/fwContainer.hpp"
#include "ui/base/GuiBaseObject.hpp"

#include <core/runtime/ConfigurationElement.hpp>

namespace sight::ui::base
{

namespace layoutManager
{

/**
 * @brief   Defines the generic layout manager for IHM.
 */
class UI_BASE_CLASS_API IViewLayoutManager : public ui::base::GuiBaseObject
{
public:

    SIGHT_DECLARE_CLASS(IViewLayoutManager, ui::base::GuiBaseObject);

    typedef core::runtime::ConfigurationElement::sptr ConfigurationType;

    /// Constructor. Do nothing.
    UI_BASE_API IViewLayoutManager();

    /// Destructor. Do nothing.
    UI_BASE_API virtual ~IViewLayoutManager();

    /**
     * @brief Configure the layout before creation.
     */
    UI_BASE_API virtual void initialize(ConfigurationType configuration) = 0;

    /**
     * @brief Instantiate layout with parent container.
     * @pre LayoutManager must be initialized before.
     * @pre parent containers must be instanced.
     */
    UI_BASE_API virtual void createLayout(ui::base::container::fwContainer::sptr parent) = 0;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    UI_BASE_API virtual void destroyLayout() = 0;

    /**
     * @brief Returns all sub containers managed by this layout.
     */
    UI_BASE_API virtual std::vector<ui::base::container::fwContainer::sptr> getSubViews();

protected:

    /**
     * @brief Helper to destroy local sub views.
     */
    UI_BASE_API virtual void destroySubViews();

    /// All sub containers managed by this layout.
    std::vector<ui::base::container::fwContainer::sptr> m_subViews;
};

} // namespace layoutManager

} // namespace sight::ui::base
