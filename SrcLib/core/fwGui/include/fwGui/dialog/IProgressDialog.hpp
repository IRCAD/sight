/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWGUI_DIALOG_IPROGRESSDIALOG_HPP__
#define __FWGUI_DIALOG_IPROGRESSDIALOG_HPP__

#include "fwGui/config.hpp"
#include "fwGui/GuiBaseObject.hpp"

#include <fwData/location/ILocation.hpp>

#include <boost/signals2.hpp>

#include <functional>
#include <string>

namespace fwGui
{
namespace dialog
{
/**
 * @brief   Defines the generic Progress dialog for IHM.
 *
 * @note    inherits from ::boost::signals2::trackable to autoDisconnect if handler is destroyed before the notifier.
 * @todo    add methods for behavior like autoClose, flying window or in status bar
 */
class FWGUI_CLASS_API IProgressDialog : public ::fwGui::GuiBaseObject,
                                        public ::boost::signals2::trackable
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (IProgressDialog)(::fwGui::GuiBaseObject), (()), progressDialogFactory);

    typedef std::string FactoryRegistryKeyType;
    typedef boost::function< void () >  CancelCallbackType;

    /// this *unique* key should  be used *for all* factory for specific LocationDialog(qt,wx,...)
    FWGUI_API static const FactoryRegistryKeyType REGISTRY_KEY;

    FWGUI_API virtual ~IProgressDialog();
    FWGUI_API IProgressDialog();

    ///set the title for the dialog
    FWGUI_API virtual void setTitle(const std::string& title) = 0;

    ///set the message for the dialog
    FWGUI_API virtual void setMessage(const std::string& msg) = 0;

    /// action called by ::fwTools::ProgressAdviser
    FWGUI_API virtual void operator()(float percent, std::string msg) = 0;

    FWGUI_API virtual void setCancelCallback(CancelCallbackType callback);

    //------------------------------------------------------------------------------

    virtual void setCancelRaiseException(bool raise)
    {
        m_raise = raise;
    }

    //------------------------------------------------------------------------------

    virtual bool getCanceled()
    {
        return m_canceled;
    }

    //------------------------------------------------------------------------------

    virtual void hideCancelButton()
    {
    }

    //------------------------------------------------------------------------------

    virtual void setProcessUserEvents(bool process)
    {
        m_processUserEvents = process;
    }

protected:

    FWGUI_API virtual void cancelPressed();

    CancelCallbackType m_cancelCallback;
    bool m_canceled;
    bool m_raise;
    ///progress bar's current value: [0-100]
    int m_value;
    bool m_processUserEvents;

protected:
    //------------------------------------------------------------------------------

    static sptr progressDialogFactory()
    {
        ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(
            ::fwGui::dialog::IProgressDialog::REGISTRY_KEY);
        ::fwGui::dialog::IProgressDialog::sptr progressDlg = ::fwGui::dialog::IProgressDialog::dynamicCast(guiObj);
        return progressDlg;
    }

};

} //namespace dialog
} // namespace fwGui

#endif /*__FWGUI_DIALOG_IPROGRESSDIALOG_HPP__*/

