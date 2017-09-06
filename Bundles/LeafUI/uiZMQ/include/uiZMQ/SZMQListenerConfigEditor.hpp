/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIZMQ_SZMQLISTENERCONFIGEDITOR_HPP__
#define __UIZMQ_SZMQLISTENERCONFIGEDITOR_HPP__

#include "uiZMQ/SZMQConfigEditor.hpp"
#include "uiZMQ/config.hpp"

namespace uiZMQ
{
/**
 *
 * @brief listener config editor
 */
class UIZMQ_CLASS_API SZMQListenerConfigEditor : public SZMQConfigEditor
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SZMQListenerConfigEditor) (SZMQConfigEditor));

    /// Constructor
    UIZMQ_API SZMQListenerConfigEditor();

    /// Destructor
    UIZMQ_API virtual ~SZMQListenerConfigEditor() noexcept;

    /**
     * @brief method to init pattern box with listener pattern
     *
     * @see Patterns
     */
    void initPatternBox();
};



} // namespace uiZMQ

#endif /*__UIZMQ_SZMQLISTENERCONFIGEDITOR_HPP__*/

