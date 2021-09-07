/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <ui/base/dialog/IPulseProgressDialog.hpp>

#include <QPointer>
#include <QProgressDialog>

#include <string>

QT_BEGIN_NAMESPACE
class QProgressDialog;
QT_END_NAMESPACE

namespace sight::ui::qt
{

namespace dialog
{

/**
 * @brief   Allows to show a pulse progress bar.
 */
class UI_QT_CLASS_API PulseProgressDialog : public ui::base::dialog::IPulseProgressDialog
{
public:

    SIGHT_DECLARE_CLASS(
        PulseProgressDialog,
        ui::base::dialog::IPulseProgressDialog,
        ui::base::factory::New<PulseProgressDialog>
    )

    UI_QT_API PulseProgressDialog(ui::base::GuiBaseObject::Key key);

    UI_QT_API virtual ~PulseProgressDialog();

    /// override
    UI_QT_API void setTitle(const std::string& title) override;

    /// override
    UI_QT_API void setMessage(const std::string& message) override;

    UI_QT_API void show() override;

protected:

    QPointer<QProgressDialog> m_pdialog;
};

} // namespace dialog

} // namespace sight::ui::qt
