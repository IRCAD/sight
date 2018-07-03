/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
