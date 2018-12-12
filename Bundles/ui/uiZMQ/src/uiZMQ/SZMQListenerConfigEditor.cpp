/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "uiZMQ/SZMQListenerConfigEditor.hpp"

#include <fwData/Object.hpp>

#include <fwServices/macros.hpp>

#include <ioZMQ/Patterns.hpp>

#include <QString>

fwServicesRegisterMacro(::uiZMQ::SZMQConfigEditor, ::uiZMQ::SZMQListenerConfigEditor, ::fwData::Object);

namespace uiZMQ
{

//-----------------------------------------------------------------------------

SZMQListenerConfigEditor::SZMQListenerConfigEditor() :
    SZMQConfigEditor()
{
    FW_DEPRECATED_MSG("'uiZMQ' bundle and the associated services are no longer supported.", "19.0");
}

//-----------------------------------------------------------------------------

SZMQListenerConfigEditor::~SZMQListenerConfigEditor() noexcept
{
}

//-----------------------------------------------------------------------------

void SZMQListenerConfigEditor::initPatternBox()
{
    ::ioZMQ::Patterns::SupportedPatternMapType::const_iterator it;

    it = ::ioZMQ::Patterns::getSupportedReaderPatterns().begin();
    for (; it != ::ioZMQ::Patterns::getSupportedReaderPatterns().end(); ++it)
    {
        m_patternBox->addItem(QString::fromStdString(it->first), QVariant(static_cast<int>(it->second)));
    }
}

//-----------------------------------------------------------------------------

} // namespace uiZMQ
