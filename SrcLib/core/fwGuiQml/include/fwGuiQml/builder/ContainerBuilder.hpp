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

#include "fwGuiQml/config.hpp"
#include "fwGuiQml/container/QmlContainer.hpp"

#include <fwCore/base.hpp>

#include <fwGui/builder/IContainerBuilder.hpp>
#include <fwGui/container/fwContainer.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

namespace fwGui
{
namespace builder
{

/**
 * @brief   Defines the generic layout manager for IHM.
 *
 */
class FWGUIQML_CLASS_API ContainerBuilder : public ::fwGui::builder::IContainerBuilder
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ContainerBuilder)(::fwGui::builder::IContainerBuilder),
                                            (()),
                                            ::fwGui::factory::New< ContainerBuilder > );

    FWGUIQML_API ContainerBuilder(::fwGui::GuiBaseObject::Key key);

    FWGUIQML_API virtual ~ContainerBuilder();

    /**
     * @brief Instantiate layout with parent container.
     * @pre LayoutManager must be initialized before.
     * @pre parent container must be instanced.
     */
    FWGUIQML_API virtual void createContainer( ::fwGui::container::fwContainer::sptr parent ) override;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUIQML_API virtual void destroyContainer() override;

    FWGUIQML_API virtual void setParent(::fwGui::container::fwContainer::sptr parent) override;

protected:
    ::fwGuiQml::container::QmlContainer::sptr m_parent;
};

} // namespace builder
} // namespace fwGui
