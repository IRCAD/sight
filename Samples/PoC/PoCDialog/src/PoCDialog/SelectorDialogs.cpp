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

#include "PoCDialog/SelectorDialogs.hpp"

#include <fwGui/dialog/SelectorDialog.hpp>

#include <fwQml/QmlEngine.hpp>

SelectorDialogs::SelectorDialogs()
{
    Q_EMIT optionChanged();
}

//------------------------------------------------------------------------------

SelectorDialogs::~SelectorDialogs()
{
}

//------------------------------------------------------------------------------

void SelectorDialogs::open()
{
    ::fwGui::dialog::SelectorDialog::sptr dialog = ::fwGui::dialog::SelectorDialog::New();
    dialog->setTitle(m_title.toStdString());
    dialog->setMessage(m_message.toStdString());
    std::vector< std::string> options;
    for (auto option: m_options)
    {
        options.push_back(option.toStdString());
    }
    dialog->setSelections(options);
    m_result = QString::fromStdString(dialog->show());
    Q_EMIT onResultChanged();
    this->m_hasOption = false;
    Q_EMIT optionChanged();
}
