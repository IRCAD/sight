/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/__/config.hpp"
#include "ui/__/object.hpp"

#include <boost/signals2.hpp>

#include <functional>
#include <string>

namespace sight::ui::dialog
{

/**
 * @brief   Defines the generic Progress dialog for IHM.
 *
 * @note    inherits from boost::signals2::trackable to auto_disconnect if handler is destroyed before the notifier.
 * @todo    add methods for behavior like autoClose, flying window or in status bar
 */
class UI_CLASS_API progress_base : public ui::object,
                                   public boost::signals2::trackable
{
public:

    SIGHT_DECLARE_CLASS(progress_base, ui::object);

    typedef std::string FactoryRegistryKeyType;
    typedef boost::function<void ()> CancelCallbackType;

    /// this *unique* key should  be used *for all* factory for specific location(qt,wx,...)
    UI_API static const FactoryRegistryKeyType REGISTRY_KEY;

    UI_API ~progress_base() override;
    UI_API progress_base();

    ///set the title for the dialog
    UI_API virtual void setTitle(const std::string& title) = 0;

    ///set the message for the dialog
    UI_API virtual void setMessage(const std::string& msg) = 0;

    /// action called by core::tools::progress_adviser
    UI_API virtual void operator()(float percent, std::string msg) = 0;

    UI_API virtual void setCancelCallback(CancelCallbackType callback);

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

    UI_API virtual void cancelPressed();

    CancelCallbackType m_cancelCallback;
    bool m_canceled {false};
    bool m_raise {true};

    ///progress bar's current value: [0-100]
    int m_value {0};
    bool m_processUserEvents {true};

protected:

    //------------------------------------------------------------------------------

    static sptr progressDialogFactory()
    {
        ui::object::sptr guiObj = ui::factory::make(
            ui::dialog::progress_base::REGISTRY_KEY
        );
        ui::dialog::progress_base::sptr progressDlg =
            std::dynamic_pointer_cast<ui::dialog::progress_base>(guiObj);
        return progressDlg;
    }
};

} // namespace sight::ui::dialog
