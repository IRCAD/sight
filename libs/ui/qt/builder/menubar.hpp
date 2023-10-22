/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include "ui/qt/container/widget.hpp"

#include <core/base.hpp>

#include <ui/__/builder/menubar.hpp>
#include <ui/__/container/menubar.hpp>
#include <ui/__/container/widget.hpp>

namespace sight::ui::qt::builder
{

/**
 * @brief   Defines a menu bar.
 *
 */
class UI_QT_CLASS_API menubar : public ui::builder::menubar
{
public:

    SIGHT_DECLARE_CLASS(menubar, ui::builder::menubar, ui::factory::make<menubar>);

    UI_QT_API ~menubar() override = default;

    /**
     * @brief Instantiate layout with parent menuBar.
     * @pre layout must be initialized before.
     * @pre parent menuBar must be instanced.
     */
    UI_QT_API void createMenuBar(ui::container::widget::sptr _parent) override;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    UI_QT_API void destroyMenuBar() override;

protected:

    ui::qt::container::widget::sptr m_parent;
};

} // namespace sight::ui::qt::builder
