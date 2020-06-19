/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwGui/config.hpp"
#include "fwGui/container/fwContainer.hpp"
#include "fwGui/GuiBaseObject.hpp"

#include <fwRuntime/ConfigurationElement.hpp>

namespace fwGui
{
namespace builder
{

/**
 * @brief   Defines the interface class  for the container builder.
 */
class FWGUI_CLASS_API IContainerBuilder : public ::fwGui::GuiBaseObject
{
public:
    fwCoreClassMacro(IContainerBuilder, ::fwGui::GuiBaseObject);

    typedef std::string RegistryKeyType;

    FWGUI_API const static RegistryKeyType REGISTRY_KEY;

    /// Constructor. Do nothing.
    FWGUI_API IContainerBuilder();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~IContainerBuilder();

    /**
     * @brief Returns the builded menu bar.
     */
    FWGUI_API virtual ::fwGui::container::fwContainer::sptr getContainer();

    /**
     * @brief Configure the layout before creation. Does nothing.
     */
    FWGUI_API virtual void initialize( ::fwRuntime::ConfigurationElement::sptr configuration);

    /**
     * @brief Instantiate layout with parent container.
     * @pre LayoutManager must be initialized before.
     * @pre parent container must be instanced.
     */
    FWGUI_API virtual void createContainer( ::fwGui::container::fwContainer::sptr parent ) = 0;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUI_API virtual void destroyContainer() = 0;

    /// Replace parent container
    FWGUI_API virtual void setParent(::fwGui::container::fwContainer::sptr parent) = 0;

protected:

    /// Container.
    ::fwGui::container::fwContainer::sptr m_container;

};

} // namespace builder
} // namespace fwGui
