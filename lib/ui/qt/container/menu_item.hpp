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

#include <ui/__/container/menu_item.hpp>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

namespace sight::ui::qt::container
{

/**
 * @brief   Defines the Qt menu item bar container.
 */
class SIGHT_UI_QT_CLASS_API menu_item : public ui::container::menu_item
{
public:

    SIGHT_DECLARE_CLASS(
        menu_item,
        ui::container::menu_item,
        ui::factory::make<menu_item>
    )

    SIGHT_UI_QT_API ~menu_item() noexcept override = default;

    SIGHT_UI_QT_API void clean() override;
    SIGHT_UI_QT_API void destroy_container() override;

    SIGHT_UI_QT_API virtual void set_qt_menu_item(QAction* _menu_item);
    SIGHT_UI_QT_API virtual QAction* get_qt_menu_item();

private:

    QPointer<QAction> m_menu_item;
};

} // namespace sight::ui::qt::container
