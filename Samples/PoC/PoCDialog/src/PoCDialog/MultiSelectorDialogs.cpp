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

#include "PoCDialog/MultiSelectorDialogs.hpp"

#include <fwGui/dialog/MultiSelectorDialog.hpp>
#include <fwGui/registry/macros.hpp>

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>

#include <boost/assign/list_of.hpp>

#include <QGuiApplication>
#include <QMessageBox>
#include <QObject>

MultiSelectorDialogs::MultiSelectorDialogs()
{
    // get the qml engine QmlApplicationEngine
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();
    // add context for root Context
    engine->getRootContext()->setContextProperty("pocDialogMultiSelectorDialogs", this);
}

//------------------------------------------------------------------------------

MultiSelectorDialogs::~MultiSelectorDialogs()
{
}

//------------------------------------------------------------------------------

void MultiSelectorDialogs::open()
{
    ::fwGui::dialog::MultiSelectorDialog::sptr dialog = ::fwGui::dialog::MultiSelectorDialog::New();
    dialog->setTitle(m_title.toStdString());
    dialog->setMessage(m_message.toStdString());
    std::map< std::string, bool > options;
    for (int i = 0; i < m_options.size(); ++i)
    {
        auto option = m_options.at(i).toStdString();
        auto check  = m_checked.at(i);
        options.insert(std::pair<std::string, bool>(option, check));
    }
    dialog->setSelections(options);
    auto result = dialog->show();
    Q_EMIT onResultChanged();
}
