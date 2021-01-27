/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <gui/container/fwContainer.hpp>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QLayout;
class QWidget;
QT_END_NAMESPACE

namespace sight::guiQt
{
namespace container
{

/**
 * @brief   Defines the QWidget container for UI.
 *
 */

class GUIQT_CLASS_API QtContainer : public gui::container::fwContainer
{

public:

    fwCoreClassMacro(QtContainer, gui::container::fwContainer, gui::factory::New< QtContainer >)

    GUIQT_API QtContainer(gui::GuiBaseObject::Key key) noexcept;

    GUIQT_API virtual ~QtContainer() noexcept override;

    GUIQT_API virtual void setQtContainer(QWidget* container);
    GUIQT_API virtual QWidget* getQtContainer() const;

    /**
     * @brief Assign a layout to the container widget. Previous layout and its children are deleted and the container
     *  widget take the ownership of the new layout
     *
     * @param The layout to set on the container widget
     */
    GUIQT_API virtual void setLayout(QLayout* const layout);

    GUIQT_API virtual void clean() override;
    GUIQT_API virtual void destroyContainer() override;
    GUIQT_API virtual bool isShownOnScreen() override;

    GUIQT_API virtual void setVisible(bool isVisible) override;
    GUIQT_API virtual void setEnabled(bool isEnabled) override;

private:

    QPointer<QWidget> m_container;
};

} // namespace container
} // namespace sight::guiQt
