/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_IMESSAGEBOX_HPP_
#define _FWGUI_IMESSAGEBOX_HPP_

#include <string>
#include <fwCore/base.hpp>

#include "fwGui/config.hpp"

namespace fwGui
{

/**
 * @brief   Defines the generic message box for IHM.
 * @class   IMessageDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IMessageDialog : public ::fwCore::BaseObject
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (IMessageDialog)(::fwCore::BaseObject) )

    /// Icon type
    typedef enum {
        CRITICAL,
        WARNING,
        INFO,
        QUESTION,
        NONE
    } Icons;

    /// Buttons type
    typedef enum {
        NOBUTTON = 0, // invalid

        OK     = 1 << 1,
        YES    = 1 << 2,
        NO     = 1 << 3,
        CANCEL = 1 << 4,

        YES_NO = YES | NO
    } Buttons;

    /// Constructor. Do nothing.
    FWGUI_API IMessageDialog();
    /// Destructor. Do nothing.
    FWGUI_API virtual ~IMessageDialog();


    typedef std::string FactoryRegistryKeyType;
    FWGUI_API static const FactoryRegistryKeyType REGISTRY_KEY;

    /// Set the title of the message box
    FWGUI_API virtual void setTitle(const std::string &title) = 0;

    /// Set the message
    FWGUI_API virtual void setMessage(const std::string &msg) = 0;

    /// Set the icon (CRITICAL, WARNING, INFO or QUESTION)
    FWGUI_API virtual void setIcon( Icons icon) =0;

    /// Add a button (OK, YES_NO, YES, NO, CANCEL)
    FWGUI_API virtual void addButton(Buttons button )=0;

    /// Show the message box and return the clicked button.
    FWGUI_API virtual Buttons show() = 0;

};

} // namespace fwGui

#endif /*_FWGUI_IMESSAGEBOX_HPP_*/


