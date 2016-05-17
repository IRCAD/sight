/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiZMQ/SZMQListenerConfigEditor.hpp"

#include <fwData/Object.hpp>
#include <fwServices/macros.hpp>
#include <ioZMQ/Patterns.hpp>

#include <QString>

fwServicesRegisterMacro (::uiZMQ::SZMQConfigEditor, ::uiZMQ::SZMQListenerConfigEditor, ::fwData::Object);


namespace uiZMQ
{

//-----------------------------------------------------------------------------

SZMQListenerConfigEditor::SZMQListenerConfigEditor() :
    SZMQConfigEditor()
{
}

//-----------------------------------------------------------------------------

SZMQListenerConfigEditor::~SZMQListenerConfigEditor() throw()
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


