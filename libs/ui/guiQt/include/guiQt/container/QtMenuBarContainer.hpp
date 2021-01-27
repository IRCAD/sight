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

#include "guiQt/config.hpp"

#include <core/base.hpp>
#include <core/tools/Object.hpp>

#include <gui/container/fwMenuBar.hpp>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QMenuBar;
QT_END_NAMESPACE

namespace sight::guiQt
{

namespace container
{

/**
 * @brief   Defines the Qt menu bar container for GUI.
 *
 */
class GUIQT_CLASS_API QtMenuBarContainer : public gui::container::fwMenuBar
{

public:

    fwCoreClassMacro(QtMenuBarContainer, gui::container::fwMenuBar, gui::factory::New< QtMenuBarContainer >)

    GUIQT_API QtMenuBarContainer(gui::GuiBaseObject::Key key) noexcept;

    GUIQT_API virtual ~QtMenuBarContainer() noexcept;

    GUIQT_API virtual void clean() override;
    GUIQT_API virtual void destroyContainer() override;

    GUIQT_API virtual void setQtMenuBar(QMenuBar* menuBar);
    GUIQT_API virtual QMenuBar* getQtMenuBar();

private:

    QPointer<QMenuBar> m_menuBar;
};

} // namespace container
} // namespace sight::guiQt
