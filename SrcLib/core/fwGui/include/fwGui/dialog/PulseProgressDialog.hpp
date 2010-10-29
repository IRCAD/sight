/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_PULSEPROGRESSDIALOG_HPP_
#define _FWGUI_PULSEPROGRESSDIALOG_HPP_

#include <fwCore/base.hpp>

#include "fwGui/config.hpp"
#include "fwGui/dialog/IPulseProgressDialog.hpp"

namespace fwGui
{
namespace dialog
{
/**
 * @brief   Defines the generic file/folder dialog for IHM.
 * Use the Delegate design pattern. The specific implementation selection is ensured by ::fwClassFactoryRegistry
 * The specific implementation are in fwWX and fwQT libraries
 * @class   PulseProgressDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API PulseProgressDialog : public IPulseProgressDialog
{

public:


    fwCoreClassDefinitionsWithFactoryMacro( (PulseProgressDialog)(::fwGui::dialog::IPulseProgressDialog), (((const std::string))((::fwGui::dialog::IPulseProgressDialog::Stuff))), new PulseProgressDialog );

    /// will instanciate the concrete implementation
    FWGUI_API PulseProgressDialog(
            const std::string &title,
            Stuff stuff,
            const std::string &msg = std::string(),
            ::fwGui::dialog::IPulseProgressDialog::MilliSecond frequenceRefresh = 100 );

    /// override
    FWGUI_API void setTitle(const std::string &title);

    /// override
    FWGUI_API void setMessage(const std::string &message);

    ///show the pulse progress dialog
    FWGUI_API virtual void show();

protected :

    ::fwGui::dialog::IPulseProgressDialog::sptr m_implementation;

};

} //namespace dialog
} // namespace fwGui

#endif /*_FWGUI_PULSEPROGRESSDIALOG_HPP_*/


