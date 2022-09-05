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
#include "ui/base/GuiBaseObject.hpp"

#include <core/runtime/ConfigurationElement.hpp>

namespace sight::ui::base::builder
{

/**
 * @brief   Defines the interface class  for the container builder.
 */
class UI_BASE_CLASS_API IContainerBuilder : public ui::base::GuiBaseObject
{
public:

    SIGHT_DECLARE_CLASS(IContainerBuilder, ui::base::GuiBaseObject);

    typedef std::string RegistryKeyType;

    UI_BASE_API const static RegistryKeyType REGISTRY_KEY;

    /// Constructor. Do nothing.
    UI_BASE_API IContainerBuilder();

    /// Destructor. Do nothing.
    UI_BASE_API ~IContainerBuilder() override;

    /**
     * @brief Returns the builded menu bar.
     */
    UI_BASE_API virtual ui::base::container::fwContainer::sptr getContainer();

    /**
     * @brief Configure the layout before creation. Does nothing.
     */
    UI_BASE_API virtual void initialize(core::runtime::ConfigurationElement::sptr configuration);

    /**
     * @brief Instantiate layout with parent container.
     * @pre LayoutManager must be initialized before.
     * @pre parent container must be instanced.
     */
    UI_BASE_API virtual void createContainer(ui::base::container::fwContainer::sptr parent) = 0;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    UI_BASE_API virtual void destroyContainer() = 0;

    /// Replace parent container
    UI_BASE_API virtual void setParent(ui::base::container::fwContainer::sptr parent) = 0;

protected:

    /// Container.
    ui::base::container::fwContainer::sptr m_container;
};

} // namespace sight::ui::base::builder
