/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <ui/base/ActionCallbackBase.hpp>

#include <QObject>

namespace sight::ui::qt
{

/**
 * @brief   Defines the menu item callback.
 */
class UI_QT_CLASS_QT_API ActionCallback : public QObject,
                                          public ui::base::ActionCallbackBase
{
Q_OBJECT

public:

    SIGHT_DECLARE_CLASS(ActionCallback, ui::base::ActionCallbackBase, ui::base::factory::New<ActionCallback>);

    UI_QT_QT_API ActionCallback(ui::base::GuiBaseObject::Key key);

    UI_QT_QT_API ~ActionCallback() override;

/**
 * @brief  Qt slot.
 */

public Q_SLOTS:

    void executeQt(bool checked = false);
    void checkQt(bool checked);
};

} // namespace sight::ui::qt
