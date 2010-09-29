/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_ICURSOR_HPP_
#define _FWGUI_ICURSOR_HPP_

#include <fwCore/base.hpp>

#include "fwGui/config.hpp"

namespace fwGui
{

/**
 * @brief   Defines the generic cursor for IHM.
 * @class   ICursor.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API ICursor : public ::fwCore::BaseObject
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (ICursor)(::fwCore::BaseObject) )

    /// Cursor type
    typedef enum {
        DEFAULT,
        BUSY,
        WAIT,
        CROSS
    } CursorType;

    /// Constructor. Do nothing.
    FWGUI_API ICursor();
    /// Destructor. Do nothing.
    FWGUI_API virtual ~ICursor();


    typedef std::string FactoryRegistryKeyType;
    FWGUI_API static const FactoryRegistryKeyType REGISTRY_KEY;

    /// Set the cursor
    FWGUI_API virtual void setCursor( CursorType cursor) = 0;

    /// Set the default cursor
    FWGUI_API virtual void setDefaultCursor() = 0;

};

} // namespace fwGui

#endif /*_FWGUI_ICURSOR_HPP_*/


