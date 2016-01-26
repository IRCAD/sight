/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_ICURSOR_HPP__
#define __FWGUI_ICURSOR_HPP__

#include "fwGui/GuiBaseObject.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{

/**
 * @brief   Defines the generic cursor for IHM.
 * @class   ICursor
 *
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API ICursor : public ::fwGui::GuiBaseObject
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (ICursor)(::fwGui::GuiBaseObject) )

    /// Cursor type
    typedef enum
    {
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

#endif /*__FWGUI_ICURSOR_HPP__*/


