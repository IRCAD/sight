/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/ui/qt/config.hpp>

#include <ui/__/dialog/pulse_progress_base.hpp>

#include <QPointer>
#include <QProgressDialog>

#include <string>

QT_BEGIN_NAMESPACE
class QProgressDialog;
QT_END_NAMESPACE

namespace sight::ui::qt::dialog
{

/**
 * @brief   Allows to show a pulse progress bar.
 */
class SIGHT_UI_QT_CLASS_API pulse_progress : public ui::dialog::pulse_progress_base
{
public:

    SIGHT_DECLARE_CLASS(
        pulse_progress,
        ui::dialog::pulse_progress_base,
        ui::factory::make<pulse_progress>
    )

    SIGHT_UI_QT_API pulse_progress();
    SIGHT_UI_QT_API ~pulse_progress() override;

    /// override
    SIGHT_UI_QT_API void set_title(const std::string& _title) override;

    /// override
    SIGHT_UI_QT_API void set_message(const std::string& _message) override;

    /// override
    SIGHT_UI_QT_API void set_cancellable(bool _cancellable) override;

    SIGHT_UI_QT_API void show() override;

protected:

    QPointer<QProgressDialog> m_dialog;

private:

    bool m_cancellable {true};
};

} // namespace sight::ui::qt::dialog
