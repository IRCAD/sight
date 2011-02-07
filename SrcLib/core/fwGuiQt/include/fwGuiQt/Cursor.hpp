/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIQT_CURSOR_HPP_
#define _FWGUIQT_CURSOR_HPP_

#include <fwCore/base.hpp>

#include <fwGui/ICursor.hpp>

#include "fwGuiQt/config.hpp"

namespace fwGuiQt
{

/**
 * @brief   Defines the generic cursor for IHM.
 * @class   Cursor.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIQT_CLASS_API Cursor : public ::fwGui::ICursor
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (Cursor)(::fwGui::ICursor), (()), new Cursor );

    /// Constructor. Do nothing.
    FWGUIQT_API Cursor();
    /// Destructor. Do nothing.
    FWGUIQT_API virtual ~Cursor();

    /// Set the cursor
    FWGUIQT_API virtual void setCursor( ::fwGui::ICursor::CursorType cursor);

    /// Set the default cursor
    FWGUIQT_API virtual void setDefaultCursor();

};

} // namespace fwGuiQt

#endif /*_FWGUIQT_CURSOR_HPP_*/


