/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_CURSOR_HPP_
#define _FWWX_CURSOR_HPP_

#include <string>

#include <fwCore/base.hpp>



#include <fwGui/ICursor.hpp>

#include "fwWX/config.hpp"

namespace fwWX
{

/**
 * @brief   Defines the generic message box for IHM.
 * @class   Cursor.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWWX_CLASS_API Cursor : public ::fwGui::ICursor
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (Cursor)(::fwGui::ICursor) )

    /// Constructor. Do nothing.
    FWWX_API Cursor();
    /// Destructor. Do nothing.
    FWWX_API virtual ~Cursor();

    /// Set the cursor
    FWWX_API virtual void setCursor( ::fwGui::ICursor::CursorType cursor);

    /// Set the default cursor
    FWWX_API virtual void setDefaultCursor();

};

} // namespace fwWX

#endif /*_FWWX_CURSOR_HPP_*/


