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

#include <fwQml/QmlEngine.hpp>

MultiSelectorDialogs::MultiSelectorDialogs()
{
    qmlRegisterType<MultiSelectorDialogs>("PoCDialog", 1, 0, "PocDialogMultiSelectorDialogs");
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
    auto results = dialog->show();
    for (auto result: results)
    {
        m_result += QString::fromStdString(result.first);
    }
    Q_EMIT onResultChanged();
}
