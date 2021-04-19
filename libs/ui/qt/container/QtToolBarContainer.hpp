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

#include <core/base.hpp>
#include <core/tools/Object.hpp>

#include <ui/base/container/fwToolBar.hpp>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QToolBar;
QT_END_NAMESPACE

namespace sight::ui::qt
{
namespace container
{

/**
 * @brief   Defines the Qt tool bar container.
 *
 */
class UI_QT_CLASS_API QtToolBarContainer : public ui::base::container::fwToolBar
{

public:

    SIGHT_DECLARE_CLASS(QtToolBarContainer, ui::base::container::fwToolBar,
                        ui::base::factory::New< QtToolBarContainer >)

    UI_QT_API QtToolBarContainer(ui::base::GuiBaseObject::Key key) noexcept;

    UI_QT_API virtual ~QtToolBarContainer() noexcept;

    UI_QT_API void clean() override;
    UI_QT_API void destroyContainer() override;

    UI_QT_API virtual void setQtToolBar(QToolBar* toolBar);
    UI_QT_API virtual QToolBar* getQtToolBar();

private:

    QPointer<QToolBar> m_toolBar;
};

} // namespace container
} // namespace sight::ui::qt
