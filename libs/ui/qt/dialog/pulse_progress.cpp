/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "ui/qt/dialog/pulse_progress.hpp"

#include <core/base.hpp>

#include <ui/__/macros.hpp>

#include <QApplication>
#include <QFutureWatcher>
#include <QPushButton>
#include <QString>
#include <QtConcurrent>
#include <QtCore>

namespace sight::ui::qt::dialog
{

//------------------------------------------------------------------------------

pulse_progress::pulse_progress() :
    m_dialog(new QProgressDialog(qApp->activeWindow()))
{
}

//------------------------------------------------------------------------------

pulse_progress::~pulse_progress()
{
    if(m_dialog != nullptr)
    {
        m_dialog->hide();
        delete m_dialog;
    }
}

//------------------------------------------------------------------------------

void pulse_progress::set_title(const std::string& _title)
{
    m_dialog->setWindowTitle(QString::fromStdString(_title));
}

//------------------------------------------------------------------------------

void pulse_progress::set_message(const std::string& _msg)
{
    m_dialog->setLabelText(QString::fromStdString(_msg));
}

//------------------------------------------------------------------------------

void pulse_progress::set_cancellable(bool _cancellable)
{
    if(!_cancellable)
    {
        m_dialog->setCancelButton(nullptr);
    }
    else if(!m_cancellable)
    {
        m_dialog->setCancelButton(new QPushButton("Cancel", m_dialog));
    }

    m_cancellable = _cancellable;
}

//------------------------------------------------------------------------------

void pulse_progress::show()
{
    // Create a QFutureWatcher and connect signals and slots.
    QFutureWatcher<void> future_watcher;
    QObject::connect(&future_watcher, SIGNAL(finished()), m_dialog, SLOT(reset()));
    QObject::connect(&future_watcher, SIGNAL(progressRangeChanged(int,int)), m_dialog, SLOT(setRange(int,int)));
    QObject::connect(&future_watcher, SIGNAL(progressValueChanged(int)), m_dialog, SLOT(setValue(int)));

    if(m_cancellable)
    {
        QObject::connect(m_dialog, SIGNAL(canceled()), &future_watcher, SLOT(cancel()));
    }

    // Start the computation.
    future_watcher.setFuture(QtConcurrent::run(m_stuff));

    m_dialog->exec();
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qt::dialog
