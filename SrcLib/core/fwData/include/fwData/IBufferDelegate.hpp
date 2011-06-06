/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_IBUFFERDELEGATE_HPP_
#define _FWDATA_IBUFFERDELEGATE_HPP_

#include <fwCore/base.hpp>

#include "fwData/config.hpp"
#include "fwData/macros.hpp"

namespace fwData
{
/**
 * @class   IBufferDelegate
 * @brief   This class is an abstract class, it defines a delegate buffer. It is use in ::fwData::Image.
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API IBufferDelegate
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (IBufferDelegate) );

    /// @return the buffer
    FWDATA_API virtual void *getBuffer() = 0;

    /// Set the buffer
    FWDATA_API virtual void setBuffer(void *) = 0;

    fwGettersSettersDocMacro(ManagesBuff, bManagesBuff, bool, if buffer is managed);

protected :

    /// Constructor
    FWDATA_API IBufferDelegate();

    /// Destructor
    FWDATA_API virtual ~IBufferDelegate();

    void *m_buffer;

    bool m_bManagesBuff;
};

} // namespace fwData

#endif // _FWDATA_IBUFFERDELEGATE_HPP_
