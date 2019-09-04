/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwGuiQt/config.hpp"

#include <fwCore/base.hpp>

#include <fwGui/container/fwMenuBar.hpp>

#include <fwTools/Object.hpp>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QMenuBar;
QT_END_NAMESPACE

namespace fwGuiQt
{

namespace container
{

/**
 * @brief   Defines the Qt menu bar container for GUI.
 *
 */
class FWGUIQT_CLASS_API QtMenuBarContainer : public ::fwGui::container::fwMenuBar
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (QtMenuBarContainer)(::fwGui::container::fwMenuBar),
                                            ::fwGui::factory::New< QtMenuBarContainer > );

    FWGUIQT_API QtMenuBarContainer(::fwGui::GuiBaseObject::Key key) noexcept;

    FWGUIQT_API virtual ~QtMenuBarContainer() noexcept;

    FWGUIQT_API virtual void clean() override;
    FWGUIQT_API virtual void destroyContainer() override;

    FWGUIQT_API virtual void setQtMenuBar(QMenuBar* menuBar);
    FWGUIQT_API virtual QMenuBar* getQtMenuBar();

private:

    QPointer<QMenuBar> m_menuBar;
};

} // namespace container
} // namespace fwGuiQt
