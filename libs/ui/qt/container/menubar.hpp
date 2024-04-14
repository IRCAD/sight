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
#include <core/tools/object.hpp>

#include <ui/__/container/menubar.hpp>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QMenuBar;
QT_END_NAMESPACE

namespace sight::ui::qt::container
{

/**
 * @brief   Defines the Qt menu bar container.
 *
 */
class SIGHT_UI_QT_CLASS_API menubar : public ui::container::menubar
{
public:

    SIGHT_DECLARE_CLASS(
        menubar,
        ui::container::menubar,
        ui::factory::make<menubar>
    )

    SIGHT_UI_QT_API ~menubar() noexcept override = default;

    SIGHT_UI_QT_API void clean() override;
    SIGHT_UI_QT_API void destroy_container() override;

    SIGHT_UI_QT_API virtual void set_qt_menu_bar(QMenuBar* _menu_bar);
    SIGHT_UI_QT_API virtual QMenuBar* get_qt_menu_bar();

private:

    QPointer<QMenuBar> m_menu_bar;
};

} // namespace sight::ui::qt::container
