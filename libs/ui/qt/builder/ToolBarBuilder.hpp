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

#include "ui/qt/config.hpp"
#include "ui/qt/container/QtContainer.hpp"

#include <core/base.hpp>
#include <core/runtime/ConfigurationElement.hpp>

#include <ui/base/builder/IToolBarBuilder.hpp>
#include <ui/base/container/fwContainer.hpp>
#include <ui/base/container/fwToolBar.hpp>

namespace sight::ui::base
{

namespace builder
{

/**
 * @brief   Defines a tool bar.
 *
 */
class UI_QT_CLASS_API ToolBarBuilder : public ui::base::builder::IToolBarBuilder
{
public:

    SIGHT_DECLARE_CLASS(ToolBarBuilder, ui::base::builder::IToolBarBuilder, ui::base::factory::New<ToolBarBuilder>);

    UI_QT_API ToolBarBuilder(ui::base::GuiBaseObject::Key key);

    UI_QT_API virtual ~ToolBarBuilder();

    /**
     * @brief Instantiate layout with parent toolBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent toolBar must be instanced.
     */
    UI_QT_API void createToolBar(ui::base::container::fwContainer::sptr parent) override;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    UI_QT_API void destroyToolBar() override;

protected:

    ui::qt::container::QtContainer::sptr m_parent;
};

} // namespace builder

} // namespace sight::ui::base
