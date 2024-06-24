/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/ui/__/config.hpp>

#include "ui/__/object.hpp"

#include <boost/signals2.hpp>

#include <functional>
#include <string>

namespace sight::ui::dialog
{

/**
 * @brief   Defines the generic Progress dialog for UI.
 *
 * @note    inherits from boost::signals2::trackable to auto_disconnect if handler is destroyed before the notifier.
 * @todo    add methods for behavior like autoClose, flying window or in status bar
 */
class SIGHT_UI_CLASS_API progress_base : public ui::object,
                                         public boost::signals2::trackable
{
public:

    SIGHT_DECLARE_CLASS(progress_base, ui::object);

    using factory_registry_key_t = std::string;
    using cancel_callback_t      = boost::function<void ()>;

    /// this *unique* key should  be used *for all* factory for specific location(qt,wx,...)
    SIGHT_UI_API static const factory_registry_key_t REGISTRY_KEY;

    SIGHT_UI_API ~progress_base() override;
    SIGHT_UI_API progress_base();

    ///set the title for the dialog
    SIGHT_UI_API virtual void set_title(const std::string& _title) = 0;

    ///set the message for the dialog
    SIGHT_UI_API virtual void set_message(const std::string& _msg) = 0;

    /// action called by core::tools::progress_adviser
    SIGHT_UI_API virtual void operator()(float _percent, std::string _msg) = 0;

    SIGHT_UI_API virtual void set_cancel_callback(cancel_callback_t _callback);

    //------------------------------------------------------------------------------

    virtual void set_cancel_raise_exception(bool _raise)
    {
        m_raise = _raise;
    }

    //------------------------------------------------------------------------------

    virtual bool get_canceled()
    {
        return m_canceled;
    }

    //------------------------------------------------------------------------------

    virtual void hide_cancel_button()
    {
    }

    //------------------------------------------------------------------------------

    virtual void set_process_user_events(bool _process)
    {
        m_process_user_events = _process;
    }

protected:

    SIGHT_UI_API virtual void cancel_pressed();

    cancel_callback_t m_cancel_callback;
    bool m_canceled {false};
    bool m_raise {true};

    ///progress bar's current value: [0-100]
    int m_value {0};
    bool m_process_user_events {true};

protected:

    //------------------------------------------------------------------------------

    static sptr progress_dialog_factory()
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
