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

#include <fwGui/container/fwMenu.hpp>

#include <fwTools/Object.hpp>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

namespace fwGuiQt
{
namespace container
{

/**
 * @brief   Defines the Qt menu bar container for IHM.
 *
 */
class FWGUIQT_CLASS_API QtMenuContainer : public ::fwGui::container::fwMenu
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (QtMenuContainer)(::fwGui::container::fwMenu),
                                            ::fwGui::factory::New< QtMenuContainer > );

    FWGUIQT_API QtMenuContainer(::fwGui::GuiBaseObject::Key key) noexcept;

    FWGUIQT_API virtual ~QtMenuContainer() noexcept;

    FWGUIQT_API virtual void clean() override;
    FWGUIQT_API virtual void destroyContainer() override;

    FWGUIQT_API virtual void setQtMenu(QMenu* menu);
    FWGUIQT_API virtual QMenu* getQtMenu();

private:

    QPointer<QMenu> m_menu;
};

} // namespace container
} // namespace fwGuiQt
