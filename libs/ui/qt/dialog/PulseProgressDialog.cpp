/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "ui/qt/dialog/PulseProgressDialog.hpp"

#include <core/base.hpp>

#include <ui/base/registry/macros.hpp>

#include <QApplication>
#include <QFutureWatcher>
#include <QString>
#include <QtConcurrent>
#include <QtCore>

namespace sight::ui::qt
{

namespace dialog
{

//------------------------------------------------------------------------------

PulseProgressDialog::PulseProgressDialog(ui::base::GuiBaseObject::Key /*key*/)
{
    m_dialog = new QProgressDialog(qApp->activeWindow());
}

//------------------------------------------------------------------------------

PulseProgressDialog::~PulseProgressDialog()
{
    if(m_dialog)
    {
        m_dialog->hide();
        delete m_dialog;
    }
}

//------------------------------------------------------------------------------

void PulseProgressDialog::setTitle(const std::string& title)
{
    m_dialog->setWindowTitle(QString::fromStdString(title));
}

//------------------------------------------------------------------------------

void PulseProgressDialog::setMessage(const std::string& msg)
{
    m_dialog->setLabelText(QString::fromStdString(msg));
}

//------------------------------------------------------------------------------

void PulseProgressDialog::show()
{
    // Create a QFutureWatcher and connect signals and slots.
    QFutureWatcher<void> futureWatcher;
    QObject::connect(&futureWatcher, SIGNAL(finished()), m_dialog, SLOT(reset()));
    QObject::connect(m_dialog, SIGNAL(canceled()), &futureWatcher, SLOT(cancel()));
    QObject::connect(&futureWatcher, SIGNAL(progressRangeChanged(int,int)), m_dialog, SLOT(setRange(int,int)));
    QObject::connect(&futureWatcher, SIGNAL(progressValueChanged(int)), m_dialog, SLOT(setValue(int)));

    // Start the computation.
    futureWatcher.setFuture(QtConcurrent::run(m_stuff));

    m_dialog->exec();
}

//------------------------------------------------------------------------------

} // namespace dialog

} // namespace sight::ui::qt
