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

#include <ui/__/layout/tab.hpp>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QTabWidget;
QT_END_NAMESPACE

namespace sight::ui::qt::layout
{

/**
 * @brief   Defines the tab layout manager.
 */
class UI_QT_CLASS_API tab : public ui::layout::tab
{
public:

    SIGHT_DECLARE_CLASS(tab, ui::layout::tab, ui::factory::make<tab>)

    UI_QT_API ~tab() override = default;

    /**
     * @brief Instantiate layout with parent container.
     * @pre layout must be initialized before.
     * @pre parent containers must be instanced.
     */
    UI_QT_API void create_layout(ui::container::widget::sptr _parent, const std::string& _id) override;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    UI_QT_API void destroy_layout() override;

private:

    ui::qt::container::widget::sptr m_parent_container;

    // Layout manager of this view
    QPointer<QTabWidget> m_tab_widget;
};

} // namespace sight::ui::qt::layout
