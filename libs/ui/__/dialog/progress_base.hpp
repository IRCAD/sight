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

    typedef std::string factory_registry_key_t;
    typedef boost::function<void ()> cancel_callback_t;

    /// this *unique* key should  be used *for all* factory for specific location(qt,wx,...)
    UI_API static const factory_registry_key_t REGISTRY_KEY;

    UI_API ~progress_base() override;
    UI_API progress_base();

    ///set the title for the dialog
    UI_API virtual void setTitle(const std::string& _title) = 0;

    ///set the message for the dialog
    UI_API virtual void setMessage(const std::string& _msg) = 0;

    /// action called by core::tools::progress_adviser
    UI_API virtual void operator()(float _percent, std::string _msg) = 0;

    UI_API virtual void setCancelCallback(cancel_callback_t _callback);

    //------------------------------------------------------------------------------

    virtual void setCancelRaiseException(bool _raise)
    {
        m_raise = _raise;
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

    virtual void setProcessUserEvents(bool _process)
    {
        m_processUserEvents = _process;
    }

protected:

    UI_API virtual void cancelPressed();

    cancel_callback_t m_cancelCallback;
    bool m_canceled {false};
    bool m_raise {true};

    ///progress bar's current value: [0-100]
    int m_value {0};
    bool m_processUserEvents {true};

protected:

    //------------------------------------------------------------------------------

    static sptr progressDialogFactory()
    {
        ui::object::sptr gui_obj = ui::factory::make(
            ui::dialog::progress_base::REGISTRY_KEY
        );
        ui::dialog::progress_base::sptr progress_dlg =
            std::dynamic_pointer_cast<ui::dialog::progress_base>(gui_obj);
        return progress_dlg;
    }
};

} // namespace sight::ui::dialog
