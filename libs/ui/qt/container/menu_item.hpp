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

#include <core/base.hpp>
#include <core/tools/object.hpp>

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
class UI_QT_CLASS_API menu_item : public ui::container::menu_item
{
public:

    SIGHT_DECLARE_CLASS(
        menu_item,
        ui::container::menu_item,
        ui::factory::make<menu_item>
    )

    UI_QT_API ~menu_item() noexcept override = default;

    UI_QT_API void clean() override;
    UI_QT_API void destroyContainer() override;

    UI_QT_API virtual void setQtMenuItem(QAction* menuItem);
    UI_QT_API virtual QAction* getQtMenuItem();

private:

    QPointer<QAction> m_menuItem;
};

} // namespace sight::ui::qt::container
