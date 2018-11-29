/************************************************************************
 *
 * Copyright (C) 2016-2017 IRCAD France
 * Copyright (C) 2016-2017 IHU Strasbourg
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

#ifndef __FWGUIQT_BUILDER_SLIDEVIEWBUILDER_HPP__
#define __FWGUIQT_BUILDER_SLIDEVIEWBUILDER_HPP__

#include "fwGuiQt/config.hpp"
#include "fwGuiQt/container/QtContainer.hpp"

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
class FWGUIQT_CLASS_API SlideViewBuilder : public ::fwGui::builder::ISlideViewBuilder
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (SlideViewBuilder)(::fwGui::builder::ISlideViewBuilder),
                                            (()),
                                            ::fwGui::factory::New< SlideViewBuilder >);

    FWGUIQT_API SlideViewBuilder(::fwGui::GuiBaseObject::Key key);

    FWGUIQT_API virtual ~SlideViewBuilder();

    /// Create the slide view container
    FWGUIQT_API virtual void createContainer( ::fwGui::container::fwContainer::sptr parent ) override;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUIQT_API virtual void destroyContainer() override;

protected:
    ::fwGuiQt::container::QtContainer::sptr m_parent;
};

} // namespace builder
} // namespace fwGui

#endif /*__FWGUIQT_BUILDER_SLIDEVIEWBUILDER_HPP__*/
