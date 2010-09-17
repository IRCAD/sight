/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIWX_CURSOR_HPP_
#define _FWGUIWX_CURSOR_HPP_

#include <string>

#include <fwCore/base.hpp>

#include <fwGui/ICursor.hpp>

#include "fwGuiWx/config.hpp"

namespace fwGuiWx
{

/**
 * @brief   Defines the generic message box for IHM.
 * @class   Cursor.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIWX_CLASS_API Cursor : public ::fwGui::ICursor
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (Cursor)(::fwGui::ICursor), (()), new Cursor );

    /// Constructor. Do nothing.
    FWGUIWX_API Cursor();
    /// Destructor. Do nothing.
    FWGUIWX_API virtual ~Cursor();

    /// Set the cursor
    FWGUIWX_API virtual void setCursor( ::fwGui::ICursor::CursorType cursor);

    /// Set the default cursor
    FWGUIWX_API virtual void setDefaultCursor();

};

} // namespace fwGuiWx

#endif /*_FWGUIWX_CURSOR_HPP_*/


