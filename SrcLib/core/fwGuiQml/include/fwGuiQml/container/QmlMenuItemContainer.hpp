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

// use MenuItem QML Type

#pragma once

#include "fwGuiQml/config.hpp"

#include <fwCore/base.hpp>

#include <fwGui/container/fwMenuItem.hpp>

#include <fwTools/Object.hpp>

#include <QPointer>

namespace fwGuiQml
{
namespace container
{

/**
 * @brief   Defines the Qt menuItem bar container for IHM.
 */
class FWGUIQML_CLASS_API QmlMenuItemContainer : public ::fwGui::container::fwMenuItem
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (QmlMenuItemContainer)(::fwGui::container::fwMenuItem),
                                            (()),
                                            ::fwGui::factory::New< QmlMenuItemContainer > );

    FWGUIQML_API QmlMenuItemContainer(::fwGui::GuiBaseObject::Key key) noexcept;

    FWGUIQML_API virtual ~QmlMenuItemContainer() noexcept;

    FWGUIQML_API virtual void clean() override;
    FWGUIQML_API virtual void destroyContainer() override;

//    FWGUIQML_API virtual void setQmlMenuItem(QAction* menuItem);
//    FWGUIQML_API virtual QAction* getQmlMenuItem();

private:

//    QPointer<QAction> m_menuItem;
};

} // namespace container
} // namespace fwGuiQml
