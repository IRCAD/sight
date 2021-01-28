/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ui/base/config.hpp"
#include "ui/base/GuiBaseObject.hpp"

#include <data/location/ILocation.hpp>

#include <boost/signals2.hpp>

#include <functional>
#include <string>

namespace sight::ui::base
{
namespace dialog
{
/**
 * @brief   Defines the generic Progress dialog for IHM.
 *
 * @note    inherits from ::boost::signals2::trackable to autoDisconnect if handler is destroyed before the notifier.
 * @todo    add methods for behavior like autoClose, flying window or in status bar
 */
class UI_BASE_CLASS_API IProgressDialog : public ui::base::GuiBaseObject,
                                          public ::boost::signals2::trackable
{

public:

    fwCoreClassMacro(IProgressDialog, ui::base::GuiBaseObject, progressDialogFactory)

    typedef std::string FactoryRegistryKeyType;
    typedef boost::function< void () >  CancelCallbackType;

    /// this *unique* key should  be used *for all* factory for specific LocationDialog(qt,wx,...)
    UI_BASE_API static const FactoryRegistryKeyType REGISTRY_KEY;

    UI_BASE_API virtual ~IProgressDialog();
    UI_BASE_API IProgressDialog();

    ///set the title for the dialog
    UI_BASE_API virtual void setTitle(const std::string& title) = 0;

    ///set the message for the dialog
    UI_BASE_API virtual void setMessage(const std::string& msg) = 0;

    /// action called by core::tools::ProgressAdviser
    UI_BASE_API virtual void operator()(float percent, std::string msg) = 0;

    UI_BASE_API virtual void setCancelCallback(CancelCallbackType callback);

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

    UI_BASE_API virtual void cancelPressed();

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
        ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(
            ui::base::dialog::IProgressDialog::REGISTRY_KEY);
        ui::base::dialog::IProgressDialog::sptr progressDlg = ui::base::dialog::IProgressDialog::dynamicCast(guiObj);
        return progressDlg;
    }

};

} //namespace dialog
} // namespace sight::ui::base
