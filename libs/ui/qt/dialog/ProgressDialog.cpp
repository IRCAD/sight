/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "ui/qt/dialog/ProgressDialog.hpp"

#include <core/base.hpp>

#include <ui/base/container/fwContainer.hpp>
#include <ui/base/IFrame.hpp>
#include <ui/base/registry/macros.hpp>

#include <QPaintEvent>

namespace sight::ui::qt
{

namespace dialog
{

//------------------------------------------------------------------------------

ProgressDialog::ProgressDialog(
    ui::base::GuiBaseObject::Key /*key*/,
    const std::string& title,
    const std::string& message
) :
    m_title(""),
    m_dialog(NULL),
    m_progressbar(NULL),
    m_cancelButton(NULL),
    m_mainWindow(NULL)
{
    // Use progress widget defined by IFrame
    ui::base::container::fwContainer::sptr progressWidget = ui::base::IFrame::getProgressWidget();
    QWidget* activeWindow                                 =
        ui::qt::container::QtContainer::dynamicCast(progressWidget)->getQtContainer();
    m_mainWindow = qobject_cast<QMainWindow*>(activeWindow);

//    QWidget *activeWindow = NULL;
//
//    BOOST_FOREACH (QWidget *widget, QApplication::topLevelWidgets())
//    {
//        activeWindow = qobject_cast< QMainWindow * >(widget);
//        // activeWindow must also have a layout to use statusBar()
//        if ( activeWindow && activeWindow->layout())
//        {
//            m_mainWindow = qobject_cast< QMainWindow * >(activeWindow);
//            break;
//        }
//    }

    m_cancelButton = new QPushButton("Cancel");
    QObject::connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(cancelPressed()));

    if(m_mainWindow)
    {
        m_progressbar = new QProgressBar();
        m_progressbar->setRange(0, 100);
        m_progressbar->setValue(0);
        m_mainWindow->statusBar()->addPermanentWidget(m_progressbar, 0);
        m_mainWindow->statusBar()->addPermanentWidget(m_cancelButton, 0);
        m_mainWindow->statusBar()->setMinimumHeight(25);
        m_mainWindow->statusBar()->setMaximumHeight(25);
    }
    else
    {
        m_dialog = new QProgressDialog(0, Qt::WindowStaysOnTopHint);
        //m_dialog = new QProgressDialog( activeWindow, Qt::WindowStaysOnTopHint );

        // FIXME modal dialog has conflict with MessageHandler
        //m_dialog->setWindowModality(Qt::WindowModal);
        m_dialog->setWindowModality(Qt::NonModal);
        m_dialog->setMinimum(0);
        m_dialog->setMaximum(100);
        m_dialog->setValue(0);
        m_dialog->setCancelButton(m_cancelButton);

        this->setTitle(title);
        this->setMessage(message);

        m_dialog->show();
    }
}

//------------------------------------------------------------------------------

ProgressDialog::~ProgressDialog()
{
    QObject::disconnect(m_cancelButton, SIGNAL(clicked()), this, SLOT(cancelPressed()));

    this->setTitle("");
    this->setMessage("");

    if(m_dialog)
    {
        m_dialog->hide();
        delete m_dialog;
    }
    else if(m_progressbar)
    {
        m_mainWindow->statusBar()->removeWidget(m_progressbar);
        m_mainWindow->statusBar()->removeWidget(m_cancelButton);
        m_cancelButton->hide();
        delete m_cancelButton;
        m_progressbar->hide();
        delete m_progressbar;
    }

    m_mainWindow = 0;
}

//------------------------------------------------------------------------------

void ProgressDialog::operator()(float percent, std::string msg)
{
    SIGHT_ASSERT("m_dialog or m_progressbar not instanced", m_progressbar || m_dialog);
    int value = (int) (percent * 100);
    if(value != this->m_value)
    {
        this->setMessage(msg);

        if(m_progressbar)
        {
            m_progressbar->setValue(value);
        }
        else if(m_dialog)
        {
            m_dialog->setValue(value);
        }

        if(m_processUserEvents)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents);
        }
        else
        {
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }

        this->m_value = value;
    }
}

//------------------------------------------------------------------------------

void ProgressDialog::setTitle(const std::string& title)
{
    m_title = QString::fromStdString(title);
    if(m_progressbar)
    {
        m_mainWindow->statusBar()->showMessage(m_title);
    }
    else if(m_dialog)
    {
        m_dialog->setWindowTitle(m_title);
    }
}

//------------------------------------------------------------------------------

void ProgressDialog::setMessage(const std::string& msg)
{
    QString message("");
    if(!m_title.isEmpty())
    {
        message += m_title;
        message += " - ";
    }

    message += QString::fromStdString(msg);
    if(m_progressbar)
    {
        m_mainWindow->statusBar()->showMessage(message);
    }
    else if(m_dialog)
    {
        m_dialog->setLabelText(message);
    }
}

//------------------------------------------------------------------------------

void ProgressDialog::cancelPressed()
{
    IProgressDialog::cancelPressed();
}

//------------------------------------------------------------------------------
void ProgressDialog::hideCancelButton()
{
    m_cancelButton->hide();
}

} // namespace dialog

} // namespace sight::ui::qt
