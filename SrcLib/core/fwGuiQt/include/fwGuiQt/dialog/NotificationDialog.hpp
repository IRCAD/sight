/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "fwGuiQt/config.hpp"

#include <fwCore/macros.hpp>

#include <fwGui/dialog/INotificationDialog.hpp>

#include <QLabel>
#include <QMouseEvent>

#include <string>

/// Create a clickable QLabel.
class FWGUIQT_CLASS_API ClickableQLabel : public QLabel
{
Q_OBJECT

public:
    /// Constructor, construct a QLabel.
    explicit ClickableQLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags()) :
        QLabel(parent)
    {
        FwCoreNotUsedMacro(f);
    }

    /// Destructor, does nothing.
    virtual ~ClickableQLabel()
    {

    }

Q_SIGNALS:
    /// Clicked signal, emited when mousePressEvent occurs.
    void clicked();

protected:
    /// Emit clicked signal.
    void mousePressEvent(QMouseEvent* _event)
    {
        FwCoreNotUsedMacro(_event);
        Q_EMIT clicked();
    }
};

namespace fwGuiQt
{
namespace dialog
{

/**
 * @brief Defines a notification popup.
 */
class FWGUIQT_CLASS_API NotificationDialog : public ::fwGui::dialog::INotificationDialog
{

public:

    fwCoreClassMacro(NotificationDialog, ::fwGui::dialog::INotificationDialog,
                     ::fwGui::factory::New< NotificationDialog > )

    /// Constructor, does nothing.
    FWGUIQT_API NotificationDialog(::fwGui::GuiBaseObject::Key key);

    /// Destructor, does nothing.
    FWGUIQT_API virtual ~NotificationDialog() override final;

    /// Show the notification relative to the active window.
    FWGUIQT_API void show() override;

private:

    /**
     * @brief Computes the position using the active window boundings.
     * @return the position in pixel as QPoint.
     */
    QPoint computePosition();

};

} // namespace dialog
} // namespace fwGuiQt
