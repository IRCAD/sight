/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "SStringEditor.hpp"

#include <core/com/Signal.hxx>

using namespace sight;

namespace Tuto05EditorQml
{

const service::key_t SStringEditor::s_STRING_INOUT = "string";

//------------------------------------------------------------------------------

SStringEditor::SStringEditor() noexcept
{
}

//------------------------------------------------------------------------------

SStringEditor::~SStringEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SStringEditor::configuring()
{
}

//------------------------------------------------------------------------------

void SStringEditor::starting()
{
    this->::ui::qml::IQmlEditor::starting();
}

//------------------------------------------------------------------------------

void SStringEditor::stopping()
{
    this->::ui::qml::IQmlEditor::stopping();
}

//------------------------------------------------------------------------------

void SStringEditor::updating()
{
    auto sstr = m_string.lock();
    SIGHT_ASSERT("'" + s_STRING_INOUT + "' data must be set as 'inout'", sstr);
    const std::string value = sstr->value();

    Q_EMIT edited(QString::fromStdString(value));
}

//------------------------------------------------------------------------------

void SStringEditor::updateString(const QString& str)
{
    auto sstr = m_string.lock();
    SIGHT_ASSERT("'" + s_STRING_INOUT + "' data must be set as 'inout'", sstr);

    sstr->value() = str.toStdString();

    auto sig = sstr->signal<data::String::ModifiedSignalType>(data::String::s_MODIFIED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SStringEditor::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_STRING_INOUT, data::String::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace Tuto05EditorQml
