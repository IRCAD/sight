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
#include "ui/qt/container/QtContainer.hpp"

#include <QApplication>
#include <QMainWindow>
#include <QPointer>
#include <QProgressBar>
#include <QProgressDialog>
#include <QPushButton>
#include <QStatusBar>
#include <QString>

#include <ui/base/dialog/IProgressDialog.hpp>

#include <string>

QT_BEGIN_NAMESPACE
class QProgressDialog;
QT_END_NAMESPACE

namespace sight::ui::qt
{
namespace dialog
{
/**
 * @brief   This class allows us to select an acquisition in a patient data base.
 *
 * @note Do not inherit from fwProgressDialog but embed a fwProgressDialog because this lasted is not copiable.
 * @todo ProgressDialog is not commented.
 */
class UI_QT_CLASS_API ProgressDialog : public QObject,
                                       public ui::base::dialog::IProgressDialog
{
Q_OBJECT

public:

    fwCoreClassMacro(ProgressDialog, ui::base::dialog::IProgressDialog, ui::base::factory::New< ProgressDialog >)

    UI_QT_API ProgressDialog( ui::base::GuiBaseObject::Key key,
                              const std::string& title   = "Progression",
                              const std::string& message = std::string(86, ' ') );

    UI_QT_API virtual ~ProgressDialog();

    UI_QT_API void operator()(float percent, std::string msg) override;

    /// override
    UI_QT_API void setTitle(const std::string& title) override;

    /// override
    UI_QT_API void setMessage(const std::string& message) override;

    UI_QT_API void hideCancelButton() override;

protected Q_SLOTS:
    void cancelPressed() override;

protected:

    QString m_title;

    QPointer< QProgressDialog >  m_pdialog;
    QPointer< QProgressBar >     m_pprogressbar;
    QPointer< QPushButton >      m_pcancelButton;
    QPointer< QMainWindow >      m_pmainWindow;

};
} // namespace dialog
} // namespace sight::ui::qt
