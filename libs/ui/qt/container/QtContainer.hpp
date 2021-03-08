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

#include "ui/qt/config.hpp"

#include <core/base.hpp>
#include <core/tools/Object.hpp>

#include <ui/base/container/fwContainer.hpp>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QLayout;
class QWidget;
QT_END_NAMESPACE

namespace sight::ui::qt
{
namespace container
{

/**
 * @brief   Defines the QWidget container for UI.
 *
 */

class UI_QT_CLASS_API QtContainer : public ui::base::container::fwContainer
{

public:

    SIGHT_DECLARE_CLASS(QtContainer, ui::base::container::fwContainer, ui::base::factory::New< QtContainer >)

    UI_QT_API QtContainer(ui::base::GuiBaseObject::Key key) noexcept;

    UI_QT_API ~QtContainer() noexcept override;

    UI_QT_API virtual void setQtContainer(QWidget* container);
    UI_QT_API virtual QWidget* getQtContainer() const;

    /**
     * @brief Assign a layout to the container widget. Previous layout and its children are deleted and the container
     *  widget take the ownership of the new layout
     *
     * @param The layout to set on the container widget
     */
    UI_QT_API virtual void setLayout(QLayout* const layout);

    UI_QT_API void clean() override;
    UI_QT_API void destroyContainer() override;
    UI_QT_API bool isShownOnScreen() override;

    UI_QT_API void setVisible(bool isVisible) override;
    UI_QT_API void setEnabled(bool isEnabled) override;

private:

    QPointer<QWidget> m_container;
};

} // namespace container
} // namespace sight::ui::qt
