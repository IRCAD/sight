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

#include "ui/qt/config.hpp"
#include "ui/qt/container/QtContainer.hpp"

#include <core/base.hpp>
#include <core/runtime/ConfigurationElement.hpp>

#include <ui/base/builder/ISlideViewBuilder.hpp>
#include <ui/base/container/fwContainer.hpp>

namespace sight::ui::base
{
namespace builder
{

/// This builder creates a slide widget. The slide widget can be displayed over all others widgets.
class UI_QT_CLASS_API SlideViewBuilder : public ui::base::builder::ISlideViewBuilder
{

public:

    SIGHT_DECLARE_CLASS(SlideViewBuilder, ui::base::builder::ISlideViewBuilder,
                        ui::base::factory::New< SlideViewBuilder >)

    /// Creates the slide builder.
    UI_QT_API SlideViewBuilder(ui::base::GuiBaseObject::Key key);

    /// Destroyes the slide builder.
    UI_QT_API virtual ~SlideViewBuilder();

    /// Create the Qt container @ref ui::qt::widget::SlideBar.
    UI_QT_API virtual void createContainer( ui::base::container::fwContainer::sptr parent ) override;

    /// Destroyes the container.
    UI_QT_API virtual void destroyContainer() override;

protected:

    /// Contains the generic parent container.
    ui::qt::container::QtContainer::sptr m_parent;

};

} // namespace builder.
} // namespace sight::ui::base.
