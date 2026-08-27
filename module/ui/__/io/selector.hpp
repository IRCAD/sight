/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <core/com/signal.hpp>
#include <core/com/slot.hpp>
#include <core/notification/base.hpp>
#include <core/notification/has_monitors.hpp>

#include <ui/__/dialog_editor.hpp>

namespace sight::module::ui::io
{

/**
 * @brief  This service displays a list of available readers or writers and lets you select one to load or save a data.
 *
 * @section Signals Signals
 * - \b notification_created(core::notification::monitor::sptr) : emitted when a monitor is created.
 * - \b failed() : emitted when the reader/writer has been cancelled by the user or has failed.
 * - \b succeeded() : emitted when a reader/writer finishes correctly.
 *
 * @section Slots Slots
 * - \b forward_notification(core::notification::monitor::sptr ) : slot connected to the reader/writer to forward the
 * signal
 * 'monitorCreated'
 *
 * @section XML XML Configuration
 *
 * Sample of configuration :
 * @code{.xml}
          <service uid="..." type="sight::module::ui::io::selector">
              <data write="${selection}" />
              <selection mode="include" />
              <addSelection service="sight::module::io::session::writer" />
              <config id="SightDataConfig" service="sight::module::io::session::writer" />
          </service>
 * @endcode
 *
 * @subsection In-Out In-Out
 * - \b data.read [sight::data::object]: the object to read.
 * @subsection Input Input
 * - \b data.write [sight::data::object]: the object to save.
 * @subsection Configuration Configuration
 * - \b selection
 *      - \b mode (mandatory) : must be include (to add the selection to selector list ) or exclude (to exclude the
 * selection of the selector list).
 * - \b addSelection
 *      - \b service (mandatory) : Name of the service to include/exclude to the choice list of the selector.
 * - \b config
 *      - \b id (mandatory) : the id of the configuration to use.
 *      - \b service (mandatory) :  the name of the service.
 */
class selector : public sight::ui::dialog_editor,
                 public sight::core::notification::has_monitors
{
public:

    SIGHT_DECLARE_SERVICE(selector, sight::ui::dialog_editor);

    struct signals
    {
        using failed_t    = core::com::signal<void ()>;
        using succeeded_t = core::com::signal<void ()>;

        static const inline signal_key_t FAILED    = "failed";
        static const inline signal_key_t SUCCEEDED = "succeeded";
    };

    struct slots
    {
        using forward_notification_t = core::com::slot<void (core::notification::base::sptr)>;
        static const inline slot_key_t FORWARD_NOTIFICATION = "forward_notification";
    };

    /**
     * @brief   Constructor. Do nothing (Just initialize parameters).
     *
     * By default, selector::m_servicesAreExcluded is true.
     */
    selector();

    /// Destructor. Do nothing.
    ~selector() noexcept override = default;

protected:

    ///Starts the service. Do nothing.
    void starting() override;

    /// Stops the service. Do nothing.
    void stopping() override;

    /**
     * @brief   This method initializes class member parameters from configuration elements.
     *
     * The method verifies if the configuration is well written and retrieves user parameter values.
     * Thanks to this method, selector::m_selectedServices value is up to date.
     **/
    void configuring() override;

    /// Create a dialogue box to provide the user different available readers (writer) for the IOSelector associated
    // objects. Then, the selected reader (writer) is executed.
    void updating() override;

    /// Gives the name of the class. Do nothing.
    void info(std::ostream& _sstream) override;

private:

    void forward_notification(core::notification::base::sptr _notification);

    /// Configure if selected services are included or excluded.
    bool m_services_are_excluded {true};

    /**
     * @brief List of services to be included or excluded.
     *
     * @see selector::m_servicesAreExcluded.
     */
    std::vector<std::string> m_selected_services;

    /// Map that specifies a configuration extension for a service
    std::map<std::string, std::string> m_service_to_config;

    sight::sptr<signals::failed_t> m_sig_failed;
    sight::sptr<signals::succeeded_t> m_sig_succeeded;

    sight::sptr<slots::forward_notification_t> m_slot_forward_notification;

    data::ptr<data::object, data::access::inout> m_read {this, "data.read"};
    data::ptr<data::object, data::access::in> m_write {this, "data.write"};
};

} // namespace sight::module::ui::io
