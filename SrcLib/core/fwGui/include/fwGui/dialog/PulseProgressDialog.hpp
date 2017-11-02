/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_DIALOG_PULSEPROGRESSDIALOG_HPP__
#define __FWGUI_DIALOG_PULSEPROGRESSDIALOG_HPP__

#include "fwGui/config.hpp"
#include "fwGui/dialog/IPulseProgressDialog.hpp"

#include <fwCore/base.hpp>

namespace fwGui
{
namespace dialog
{
/**
 * @brief   Defines the generic pulse progress dialog for IHM.
 * Use the Delegate design pattern. The specific implementation selection is ensured by fwGuiRegisterMacro.
 * The specific implementation are in fwGuiWX and fwGuiQT libraries
 */
class FWGUI_CLASS_API PulseProgressDialog : public IPulseProgressDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (PulseProgressDialog)(::fwGui::dialog::IPulseProgressDialog),
                                            (((const std::string))(
                                                 (::fwGui::dialog::IPulseProgressDialog::Stuff))),
                                            new PulseProgressDialog );

    /// will instantiate the concrete implementation
    FWGUI_API PulseProgressDialog(
        const std::string& title,
        Stuff stuff,
        const std::string& msg = std::string(),
        ::fwGui::dialog::IPulseProgressDialog::MilliSecond frequenceRefresh = 100 );

    ///set the title for the dialog
    FWGUI_API void setTitle(const std::string& title) override;

    ///set the message for the dialog
    FWGUI_API void setMessage(const std::string& message) override;

    ///show the pulse progress dialog
    FWGUI_API virtual void show() override;

protected:

    ::fwGui::dialog::IPulseProgressDialog::sptr m_implementation;

};

} //namespace dialog
} // namespace fwGui

#endif /*__FWGUI_DIALOG_PULSEPROGRESSDIALOG_HPP__*/

