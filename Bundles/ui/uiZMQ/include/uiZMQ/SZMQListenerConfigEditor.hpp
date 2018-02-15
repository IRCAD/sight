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
/**
 * @brief listener config editor
 */
class UIZMQ_CLASS_API SZMQListenerConfigEditor : public SZMQConfigEditor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SZMQListenerConfigEditor) (SZMQConfigEditor))

    /// Constructor
    UIZMQ_API SZMQListenerConfigEditor();

    /// Destructor
    UIZMQ_API virtual ~SZMQListenerConfigEditor() noexcept;

    /**
     * @brief method to init pattern box with listener pattern
     *
     * @see Patterns
     */
    void initPatternBox() override;
};

} // namespace uiZMQ
