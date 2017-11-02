/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_CURSOR_HPP__
#define __FWGUI_CURSOR_HPP__

#include "fwGui/config.hpp"
#include "fwGui/ICursor.hpp"

#include <fwCore/base.hpp>

namespace fwGui
{

/**
 * @brief   Defines the generic cursor for IHM.
 * Use the Delegate design pattern
 */
class FWGUI_CLASS_API Cursor : public ::fwGui::ICursor
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (Cursor)(::fwGui::ICursor), (()), new Cursor );

    /// Constructor. Create the implementation of the specific cursor
    FWGUI_API Cursor();
    /// Destructor. Do nothing
    FWGUI_API virtual ~Cursor();

    // TODO : update API to reflect the cursor stack
    /// Set the cursor
    FWGUI_API virtual void setCursor( ::fwGui::ICursor::CursorType cursor) override;

    /// Set the default cursor
    FWGUI_API virtual void setDefaultCursor() override;

protected:

    /// Implementation of a cursor in a specific IHM (wx/Qt)
    ::fwGui::ICursor::sptr m_implementation;
};

} // namespace fwGui

#endif /*__FWGUI_CURSOR_HPP__*/

