/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "PoCDialog/ProgressDialogs.hpp"

#include <fwQml/QmlEngine.hpp>

ProgressDialogs::ProgressDialogs()
{
    Q_EMIT onOpenChanged();
}

//------------------------------------------------------------------------------

ProgressDialogs::~ProgressDialogs()
{
}

//------------------------------------------------------------------------------

void ProgressDialogs::open()
{
    m_dialog = ::fwGui::dialog::ProgressDialog::New();
    std::function<void()> callback
        = [this]()
          {
            m_dialog.reset();
          };
    if (m_addCallback)
    {
        m_dialog->setCancelCallback(callback);
    }
    else
    {
        m_dialog->hideCancelButton();
    }
    m_dialog->setTitle(m_title.toStdString());
    m_dialog->setMessage(m_message.toStdString());
    m_isOpen = true;
    Q_EMIT onOpenChanged();
}

//------------------------------------------------------------------------------

void ProgressDialogs::addPercent(QVariant percent, QString message)
{
    (*m_dialog)(percent.toFloat(), message.toStdString());
    m_result = percent.toString();
    Q_EMIT onResultChanged();
}
