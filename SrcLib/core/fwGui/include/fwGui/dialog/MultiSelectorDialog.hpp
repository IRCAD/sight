/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_DIALOG_MULTISELECTORDIALOG_HPP__
#define __FWGUI_DIALOG_MULTISELECTORDIALOG_HPP__

#include "fwGui/config.hpp"
#include "fwGui/dialog/IMultiSelectorDialog.hpp"

#include <fwCore/base.hpp>

namespace fwGui
{
namespace dialog
{
/**
 * @brief   MultiSelectorDialog allows the choice of some element among several (_selections)
 *
 * Use the Delegate design pattern. The specific implementation selection is ensured by fwGuiRegisterMacro
 * The specific implementation are in fwGuiWX and fwGuiQT libraries
 *
 */
class FWGUI_CLASS_API MultiSelectorDialog : public IMultiSelectorDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (MultiSelectorDialog)(::fwGui::dialog::IMultiSelectorDialog), (()),
                                            new MultiSelectorDialog );

    /// will instanciate the concrete implementation
    FWGUI_API MultiSelectorDialog();

    /// Sets the selector title.
    FWGUI_API void setTitle(std::string title) override;

    /// Set the string list that can be chosen by the selector.
    FWGUI_API virtual void setSelections(Selections _selections) override;

    /**
     * @brief Show the selector and return the selection.
     */
    FWGUI_API Selections show() override;

    /// Set the message
    FWGUI_API virtual void setMessage(const std::string& msg) override;

protected:

    ::fwGui::dialog::IMultiSelectorDialog::sptr m_implementation;

};

} //namespace dialog
} // namespace fwGui

#endif /*__FWGUI_DIALOG_MULTISELECTORDIALOG_HPP__*/

