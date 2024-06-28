/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/ui/qt/config.hpp>

#include <core/base.hpp>
#include <core/object.hpp>

#include <ui/__/container/toolbar.hpp>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QToolBar;
QT_END_NAMESPACE

namespace sight::ui::qt::container
{

/**
 * @brief   Defines the Qt tool bar container.
 *
 */
class SIGHT_UI_QT_CLASS_API toolbar : public ui::container::toolbar
{
public:

    SIGHT_DECLARE_CLASS(
        toolbar,
        ui::container::toolbar,
        ui::factory::make<toolbar>
    )

    SIGHT_UI_QT_API ~toolbar() noexcept override = default;

    SIGHT_UI_QT_API void clean() override;
    SIGHT_UI_QT_API void destroy_container() override;

    SIGHT_UI_QT_API virtual void set_qt_tool_bar(QToolBar* _tool_bar);
    SIGHT_UI_QT_API virtual QToolBar* get_qt_tool_bar();

private:

    QPointer<QToolBar> m_tool_bar;
};

} // namespace sight::ui::qt::container
