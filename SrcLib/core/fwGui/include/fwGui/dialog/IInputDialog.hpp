/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_DIALOG_IINPUTDIALOG_HPP__
#define __FWGUI_DIALOG_IINPUTDIALOG_HPP__

#include <string>

#include "fwGui/GuiBaseObject.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{
namespace dialog
{
/**
 * @brief   Defines the generic input dialog box for IHM.
 * @class   IInputDialog
 *
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IInputDialog : public ::fwGui::GuiBaseObject
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (IInputDialog)(::fwGui::GuiBaseObject) )

    /// Constructor. Do nothing.
    FWGUI_API IInputDialog();
    /// Destructor. Do nothing.
    FWGUI_API virtual ~IInputDialog();

    typedef std::string FactoryRegistryKeyType;
    FWGUI_API static const FactoryRegistryKeyType REGISTRY_KEY;

    /// Set the title of the input dialog
    FWGUI_API virtual void setTitle(const std::string &title) = 0;

    /// Set the input text in the input field
    FWGUI_API virtual void setInput(const std::string &text) = 0;

    /// Get the input text in the input field
    FWGUI_API virtual std::string getInput() = 0;

    /// Set the message
    FWGUI_API virtual void setMessage(const std::string &msg) = 0;

};

} //namespace dialog
} // namespace fwGui

#endif /*__FWGUI_DIALOG_IINPUTDIALOG_HPP__*/


