/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_CURSOR_HPP_
#define _FWGUI_CURSOR_HPP_

#include <fwCore/base.hpp>

#include "fwGui/ICursor.hpp"
#include "fwGui/config.hpp"


namespace fwGui
{

/**
 * @brief   Defines the generic cursor for IHM.
 * Use the Delegate design pattern
 * @class   Cursor.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API Cursor : public ::fwGui::ICursor
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (Cursor)(::fwGui::ICursor), (()), new Cursor )

    /// Constructor. Create the implementation of the specific cursor
    FWGUI_API Cursor();
    /// Destructor. Do nothing
    FWGUI_API virtual ~Cursor();

    /// Set the cursor
    FWGUI_API virtual void setCursor( ::fwGui::ICursor::CursorType cursor);

    /// Set the default cursor
    FWGUI_API virtual void setDefaultCursor();

protected:

    /// Implementation of a cursor in a specific IHM (wx/Qt)
    ::fwGui::ICursor::sptr m_implementation;
};

} // namespace fwGui

#endif /*_FWGUI_CURSOR_HPP_*/


