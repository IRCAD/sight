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

// use MenuBar QML Type

#pragma once

#include "fwGuiQml/config.hpp"

#include <fwCore/base.hpp>

#include <fwGui/container/fwMenuBar.hpp>

#include <fwTools/Object.hpp>

#include <QPointer>

class QMenuBar;

namespace fwGuiQml
{

namespace container
{

/**
 * @brief   Defines the Qt menu bar container for GUI.
 *
 */
class FWGUIQML_CLASS_API QmlMenuBarContainer : public ::fwGui::container::fwMenuBar
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (QmlMenuBarContainer)(::fwGui::container::fwMenuBar),
                                            (()),
                                            ::fwGui::factory::New< QmlMenuBarContainer > );

    FWGUIQML_API QmlMenuBarContainer(::fwGui::GuiBaseObject::Key key) noexcept;

    FWGUIQML_API virtual ~QmlMenuBarContainer() noexcept;

    FWGUIQML_API virtual void clean() override;
    FWGUIQML_API virtual void destroyContainer() override;

    FWGUIQML_API virtual void setQmlMenuBar(QMenuBar* menuBar);
    FWGUIQML_API virtual QMenuBar* getQmlMenuBar();

private:

//    QPointer<QMenuBar> m_menuBar;
};

} // namespace container
} // namespace fwGuiQt
