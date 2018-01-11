/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIZMQ_SZMQSENDERCONFIGEDITOR_HPP__
#define __UIZMQ_SZMQSENDERCONFIGEDITOR_HPP__

#include "uiZMQ/SZMQConfigEditor.hpp"
#include "uiZMQ/config.hpp"

namespace uiZMQ
{

class UIZMQ_CLASS_API SZMQSenderConfigEditor : public SZMQConfigEditor
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SZMQSenderConfigEditor) (SZMQConfigEditor));

    ///constructor
    UIZMQ_API SZMQSenderConfigEditor();

    /// Destructor
    UIZMQ_API virtual ~SZMQSenderConfigEditor() noexcept;

    void initPatternBox();

};

} // namespace uiZMQ

#endif /*__UIZMQ_SZMQSENDERCONFIGEDITOR_HPP__*/

