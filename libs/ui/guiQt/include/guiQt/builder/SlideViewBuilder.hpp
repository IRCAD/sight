/************************************************************************
 *
 * Copyright (C) 2016-2021 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#include <gui/builder/ISlideViewBuilder.hpp>
#include <gui/container/fwContainer.hpp>

namespace sight::gui
{
namespace builder
{

/// This builder creates a slide widget. The slide widget can be displayed over all others widgets.
class GUIQT_CLASS_API SlideViewBuilder : public gui::builder::ISlideViewBuilder
{

public:

    fwCoreClassMacro(SlideViewBuilder, gui::builder::ISlideViewBuilder, gui::factory::New< SlideViewBuilder >)

    /// Creates the slide builder.
    GUIQT_API SlideViewBuilder(gui::GuiBaseObject::Key key);

    /// Destroyes the slide builder.
    GUIQT_API virtual ~SlideViewBuilder();

    /// Create the Qt container @ref guiQt::widget::SlideBar.
    GUIQT_API virtual void createContainer( gui::container::fwContainer::sptr parent ) override;

    /// Destroyes the container.
    GUIQT_API virtual void destroyContainer() override;

protected:

    /// Contains the generic parent container.
    guiQt::container::QtContainer::sptr m_parent;

};

} // namespace builder.
} // namespace sight::gui.
