/************************************************************************
 *
 * Copyright (C) 2016-2019 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#include <fwGui/builder/ISlideViewBuilder.hpp>
#include <fwGui/container/fwContainer.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

namespace fwGui
{
namespace builder
{

/**
 * @brief   Defines the qt implementation of the slide view builder.
 */
class FWGUIQML_CLASS_API SlideViewBuilder : public ::fwGui::builder::ISlideViewBuilder
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (SlideViewBuilder)(::fwGui::builder::ISlideViewBuilder),
                                            (()),
                                            ::fwGui::factory::New< SlideViewBuilder >);

    FWGUIQML_API SlideViewBuilder(::fwGui::GuiBaseObject::Key key);

    FWGUIQML_API virtual ~SlideViewBuilder();

    /// Create the slide view container
    FWGUIQML_API virtual void createContainer( ::fwGui::container::fwContainer::sptr parent ) override;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUIQML_API virtual void destroyContainer() override;

protected:
    ::fwGuiQml::container::QmlContainer::sptr m_parent;
};

} // namespace builder
} // namespace fwGui
