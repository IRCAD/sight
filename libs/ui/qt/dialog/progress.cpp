/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/qt/dialog/progress.hpp"

#include <core/base.hpp>

#include <ui/__/container/widget.hpp>
#include <ui/__/frame.hpp>
#include <ui/__/macros.hpp>

#include <QPaintEvent>

namespace sight::ui::qt::dialog
{

//------------------------------------------------------------------------------

progress::progress(
    const std::string& _title,
    const std::string& _message
) :
    m_title(""),
    m_dialog(nullptr),
    m_progressbar(nullptr),
    m_cancelButton(nullptr),
    m_mainWindow(nullptr)
{
    // Use progress widget defined by frame
    ui::container::widget::sptr progress_widget = ui::frame::getProgressWidget();
    QWidget* active_window                      =
        std::dynamic_pointer_cast<ui::qt::container::widget>(progress_widget)->getQtContainer();
    m_mainWindow = qobject_cast<QMainWindow*>(active_window);

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

    if(m_mainWindow != nullptr)
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
        m_dialog = new QProgressDialog(nullptr, Qt::WindowStaysOnTopHint);
        //m_dialog = new QProgressDialog( activeWindow, Qt::WindowStaysOnTopHint );

        // FIXME modal dialog has conflict with MessageHandler
        //m_dialog->setWindowModality(Qt::WindowModal);
        m_dialog->setWindowModality(Qt::NonModal);
        m_dialog->setMinimum(0);
        m_dialog->setMaximum(100);
        m_dialog->setValue(0);
        m_dialog->setCancelButton(m_cancelButton);

        this->progress::setTitle(_title);
        this->progress::setMessage(_message);

        m_dialog->show();
    }
}

//------------------------------------------------------------------------------

progress::~progress()
{
    QObject::disconnect(m_cancelButton, SIGNAL(clicked()), this, SLOT(cancelPressed()));

    this->progress::setTitle("");
    this->progress::setMessage("");

    if(m_dialog != nullptr)
    {
        m_dialog->hide();
        delete m_dialog;
    }
    else if(m_progressbar != nullptr)
    {
        m_mainWindow->statusBar()->removeWidget(m_progressbar);
        m_mainWindow->statusBar()->removeWidget(m_cancelButton);
        m_cancelButton->hide();
        delete m_cancelButton;
        m_progressbar->hide();
        delete m_progressbar;
    }

    m_mainWindow = nullptr;
}

//------------------------------------------------------------------------------

void progress::operator()(float _percent, std::string _msg)
{
    SIGHT_ASSERT("m_dialog or m_progressbar not instanced", m_progressbar || m_dialog);
    int value = (int) (_percent * 100);
    if(value != this->m_value)
    {
        this->setMessage(_msg);

        if(m_progressbar != nullptr)
        {
            m_progressbar->setValue(value);
        }
        else if(m_dialog != nullptr)
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

void progress::setTitle(const std::string& _title)
{
    m_title = QString::fromStdString(_title);
    if(m_progressbar != nullptr)
    {
        m_mainWindow->statusBar()->showMessage(m_title);
    }
    else if(m_dialog != nullptr)
    {
        m_dialog->setWindowTitle(m_title);
    }
}

//------------------------------------------------------------------------------

void progress::setMessage(const std::string& _msg)
{
    QString message("");
    if(!m_title.isEmpty())
    {
        message += m_title;
        message += " - ";
    }

    message += QString::fromStdString(_msg);
    if(m_progressbar != nullptr)
    {
        m_mainWindow->statusBar()->showMessage(message);
    }
    else if(m_dialog != nullptr)
    {
        m_dialog->setLabelText(message);
    }
}

//------------------------------------------------------------------------------

void progress::cancelPressed()
{
    progress_base::cancelPressed();
}

//------------------------------------------------------------------------------
void progress::hideCancelButton()
{
    m_cancelButton->hide();
}

} // namespace sight::ui::qt::dialog
