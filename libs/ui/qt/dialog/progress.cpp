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
    m_cancel_button(nullptr),
    m_main_window(nullptr)
{
    // Use progress widget defined by frame
    ui::container::widget::sptr progress_widget = ui::frame::get_progress_widget();
    QWidget* active_window                      =
        std::dynamic_pointer_cast<ui::qt::container::widget>(progress_widget)->get_qt_container();
    m_main_window = qobject_cast<QMainWindow*>(active_window);

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

    m_cancel_button = new QPushButton("Cancel");
    QObject::connect(m_cancel_button, SIGNAL(clicked()), this, SLOT(cancel_pressed()));

    if(m_main_window != nullptr)
    {
        m_progressbar = new QProgressBar();
        m_progressbar->setRange(0, 100);
        m_progressbar->setValue(0);
        m_main_window->statusBar()->addPermanentWidget(m_progressbar, 0);
        m_main_window->statusBar()->addPermanentWidget(m_cancel_button, 0);
        m_main_window->statusBar()->setFixedHeight(28);

        m_cancel_button->setFixedHeight(25);
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
        m_dialog->setCancelButton(m_cancel_button);

        this->progress::set_title(_title);
        this->progress::set_message(_message);

        m_dialog->show();
    }
}

//------------------------------------------------------------------------------

progress::~progress()
{
    QObject::disconnect(m_cancel_button, SIGNAL(clicked()), this, SLOT(cancel_pressed()));

    this->progress::set_title("");
    this->progress::set_message("");

    if(m_dialog != nullptr)
    {
        m_dialog->hide();
        delete m_dialog;
    }
    else if(m_progressbar != nullptr)
    {
        m_main_window->statusBar()->removeWidget(m_progressbar);
        m_main_window->statusBar()->removeWidget(m_cancel_button);
        m_cancel_button->hide();
        delete m_cancel_button;
        m_progressbar->hide();
        delete m_progressbar;
    }

    m_main_window = nullptr;
}

//------------------------------------------------------------------------------

void progress::operator()(float _percent, std::string _msg)
{
    SIGHT_ASSERT("m_dialog or m_progressbar not instanced", m_progressbar || m_dialog);
    int value = (int) (_percent * 100);
    if(value != this->m_value)
    {
        this->set_message(_msg);

        if(m_progressbar != nullptr)
        {
            m_progressbar->setValue(value);
        }
        else if(m_dialog != nullptr)
        {
            m_dialog->setValue(value);
        }

        if(m_process_user_events)
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

void progress::set_title(const std::string& _title)
{
    m_title = QString::fromStdString(_title);
    if(m_progressbar != nullptr)
    {
        m_main_window->statusBar()->showMessage(m_title);
    }
    else if(m_dialog != nullptr)
    {
        m_dialog->setWindowTitle(m_title);
    }
}

//------------------------------------------------------------------------------

void progress::set_message(const std::string& _msg)
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
        m_main_window->statusBar()->showMessage(message);
    }
    else if(m_dialog != nullptr)
    {
        m_dialog->setLabelText(message);
    }
}

//------------------------------------------------------------------------------

void progress::cancel_pressed()
{
    progress_base::cancel_pressed();
}

//------------------------------------------------------------------------------
void progress::hide_cancel_button()
{
    m_cancel_button->hide();
}

} // namespace sight::ui::qt::dialog
