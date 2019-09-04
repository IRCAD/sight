/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

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

    fwCoreClassDefinitionsWithFactoryMacro( (Cursor)(::fwGui::ICursor), new Cursor );

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
