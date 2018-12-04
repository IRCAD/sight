/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWGUIQT_DIALOG_PROGRESSDIALOG_HPP__
#define __FWGUIQT_DIALOG_PROGRESSDIALOG_HPP__

#include "fwGuiQt/config.hpp"
#include "fwGuiQt/container/QtContainer.hpp"

#include <fwGui/dialog/IProgressDialog.hpp>

#include <QApplication>
#include <QMainWindow>
#include <QPointer>
#include <QProgressBar>
#include <QProgressDialog>
#include <QPushButton>
#include <QStatusBar>
#include <QString>

#include <string>

QT_BEGIN_NAMESPACE
class QProgressDialog;
QT_END_NAMESPACE

namespace fwGuiQt
{
namespace dialog
{
/**
 * @brief   This class allows us to select an acquisition in a patient data base.
 *
 * @note Do not inherit from fwProgressDialog but embed a fwProgressDialog because this lasted is not copiable.
 * @todo ProgressDialog is not commented.
 */
class FWGUIQT_CLASS_API ProgressDialog : public QObject,
                                         public ::fwGui::dialog::IProgressDialog
{
Q_OBJECT

public:

    fwCoreClassDefinitionsWithFactoryMacro( (ProgressDialog)(::fwGui::dialog::IProgressDialog),
                                            (()),
                                            ::fwGui::factory::New< ProgressDialog > )

    FWGUIQT_API ProgressDialog( ::fwGui::GuiBaseObject::Key key,
                                const std::string& title = "Progression",
                                const std::string& message = std::string(86, ' ') );

    FWGUIQT_API virtual ~ProgressDialog();

    FWGUIQT_API void operator()(float percent, std::string msg) override;

    /// override
    FWGUIQT_API void setTitle(const std::string& title) override;

    /// override
    FWGUIQT_API void setMessage(const std::string& message) override;

    FWGUIQT_API void hideCancelButton() override;

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
} // namespace fwGuiQt

#endif /* __FWGUIQT_DIALOG_PROGRESSDIALOG_HPP__ */
