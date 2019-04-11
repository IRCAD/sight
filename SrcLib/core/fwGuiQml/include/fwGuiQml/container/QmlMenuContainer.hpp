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

// use Menu QML Type

#pragma once

#include "fwGuiQml/config.hpp"

#include <fwCore/base.hpp>

#include <fwGui/container/fwMenu.hpp>

#include <fwTools/Object.hpp>

#include <QPointer>

namespace fwGuiQml
{
namespace container
{

/**
 * @brief   Defines the Qt menu bar container for IHM.
 *
 */
class FWGUIQML_CLASS_API QmlMenuContainer : public ::fwGui::container::fwMenu
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (QmlMenuContainer)(::fwGui::container::fwMenu),
                                            (()),
                                            ::fwGui::factory::New< QmlMenuContainer > );

    FWGUIQML_API QmlMenuContainer(::fwGui::GuiBaseObject::Key key) noexcept;

    FWGUIQML_API virtual ~QmlMenuContainer() noexcept;

    FWGUIQML_API virtual void clean() override;
    FWGUIQML_API virtual void destroyContainer() override;

//    FWGUIQML_API virtual void setQmlMenu(QMenu* menu);
//    FWGUIQML_API virtual QMenu* getQmlMenu();

private:

//    QPointer<QMenu> m_menu;
};

} // namespace container
} // namespace fwGuiQml
