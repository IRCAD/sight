/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_SELECTORDIALOG_HPP_
#define _FWGUI_SELECTORDIALOG_HPP_

#include <fwCore/base.hpp>

#include "fwGui/config.hpp"
#include "fwGui/dialog/ISelectorDialog.hpp"

namespace fwGui
{
namespace dialog
{
/**
 * @brief   SelectorDialog allows the choice of an element among several (_selections)
 * Use the Delegate design pattern. The specific implementation selection is ensured by ::fwClassFactoryRegistry
 * The specific implementation are in fwWX and fwQT libraries
 * @class   SelectorDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API SelectorDialog : public ISelectorDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (SelectorDialog)(::fwGui::dialog::ISelectorDialog), (()), new SelectorDialog );

    /**
     * Opens a default selector with the specified title, message and choices.
     *
     * @param title Title of the selector
     * @param message Message of the selector
     * @param std::vector< std::string > _selections list of choices in the selector
     * @return Returns selected choice
     */
    FWGUI_API static std::string showSelectorDialog(const std::string& title, const std::string& message, std::vector< std::string > _selections);

    /// will instanciate the concrete implementation
    FWGUI_API SelectorDialog();

    /**
     * Constructor. Create a selector with the specified title, message and choices.
     *
     * @param title Title of the selector
     * @param message Message of the selector
     * @param std::vector< std::string > _selections list of choices in the selector
     */
    FWGUI_API SelectorDialog(const std::string& title, const std::string& message, std::vector< std::string > _selections);

    /// Sets the selector title.
    FWGUI_API void setTitle(std::string title);

    /// Set the string list that can be chosen by the selector.
    FWGUI_API virtual void setSelections(std::vector< std::string > _selections);

    /// Show the selector and return the selection.
    FWGUI_API std::string show();

    /// Set the message
    FWGUI_API virtual void setMessage(const std::string &msg);

protected :

    ::fwGui::dialog::ISelectorDialog::sptr m_implementation;

};

} //namespace dialog
} // namespace fwGui

#endif /*_FWGUI_SELECTORDIALOG_HPP_*/


