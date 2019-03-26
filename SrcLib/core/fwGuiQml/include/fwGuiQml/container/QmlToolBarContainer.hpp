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

#include "fwGuiQml/config.hpp"

#include <fwCore/base.hpp>

#include <fwGui/container/fwToolBar.hpp>

#include <fwTools/Object.hpp>

#include <QPointer>
#include <QToolBar>

namespace fwGuiQml
{
namespace container
{

/**
 * @brief   Defines the Qt toolBar bar container for IHM.
 *
 */
class FWGUIQML_CLASS_API QmlToolBarContainer : public ::fwGui::container::fwToolBar
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (QmlToolBarContainer)(::fwGui::container::fwToolBar),
                                            (()),
                                            ::fwGui::factory::New< QmlToolBarContainer > );

    FWGUIQML_API QmlToolBarContainer(::fwGui::GuiBaseObject::Key key) noexcept;

    FWGUIQML_API virtual ~QmlToolBarContainer() noexcept;

    FWGUIQML_API virtual void clean() override;
    FWGUIQML_API virtual void destroyContainer() override;

    FWGUIQML_API virtual void setQmlToolBar(QToolBar* toolBar);
    FWGUIQML_API virtual QToolBar* getQmlToolBar();

private:

    QPointer<QToolBar> m_toolBar;
};

} // namespace container
} // namespace fwGuiQml
