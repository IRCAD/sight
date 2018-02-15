/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "uiZMQ/config.hpp"
#include "uiZMQ/SZMQConfigEditor.hpp"

namespace uiZMQ
{

class UIZMQ_CLASS_API SZMQSenderConfigEditor : public SZMQConfigEditor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SZMQSenderConfigEditor) (SZMQConfigEditor))

    ///constructor
    UIZMQ_API SZMQSenderConfigEditor();

    /// Destructor
    UIZMQ_API virtual ~SZMQSenderConfigEditor() noexcept;

    void initPatternBox() override;

};

} // namespace uiZMQ
