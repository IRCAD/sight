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

#include <io/__/service/reader.hpp>
#include <io/__/service/writer.hpp>

#include <ui/__/dialog_editor.hpp>
namespace sight::module::ui::io
{

/**
 * @brief This service selects and runs a reader or writer compatible with the configured data object.
 *
 * @section Signals Signals
 * - \b notification_created(core::notification::base::sptr): forwards notifications emitted by the reader/writer.
 * - \b failed() : emitted when the reader/writer has been cancelled by the user or has failed.
 * - \b succeeded() : emitted when a reader/writer finishes correctly.
 *
 * @section Slots Slots
 * - \b forward_notification(core::notification::base::sptr): forwards the reader/writer notification through the
 * 'notification_created' signal.
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
 *      - \b mode (optional, default "exclude"): includes or excludes the services listed with addSelection.
 * - \b addSelection
 *      - \b service (mandatory) : Name of the service to include/exclude to the choice list of the selector.
 * - \b config (optional)
 *      - \b id (mandatory): identifier of the configuration to use.
 *      - \b service (mandatory): name of the associated service.
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
     * @brief Initializes the service signals and the notification forwarding slot.
     *
     * By default, selector::m_servicesAreExcluded is true.
     */
    selector();

    /// Destructor. Do nothing.
    ~selector() noexcept override = default;

protected:

    /// Moves the notification forwarding slot to the default worker.
    void starting() override;

    /// Stops the service. Do nothing.
    void stopping() override;

    /**
     * @brief Reads the selector mode, service filtering rules and optional service configurations from XML.
     *
     * The optional \c config elements associate a configuration with a reader or writer. When none is provided,
     * the selected service is configured with its own default configuration.
     */
    void configuring() override;

    /// Selects a compatible reader/writer, opens the appropriate location dialog and executes the service.
    void updating() override;

    /// Writes the service name to the supplied stream.
    void info(std::ostream& _sstream) override;

private:

    /**
     * @brief Selects the reader workflow according to the path types supported by the available services.
     * @param[in] _available_services pairs containing each reader implementation identifier and display name.
     */
    void update_reader(
        const std::vector<std::pair<std::string, std::string> >& _available_services
    );

    /**
     * @brief Selects and executes a writer, using a shared file dialog when its extensions are available.
     * @param[in] _available_extensions_map pairs containing each writer implementation identifier and display name.
     * @param[in] _available_extensions_selector display names presented when a writer must be selected explicitly.
     */
    void update_writer(
        const std::vector<std::pair<std::string, std::string> >& _available_extensions_map,
        const std::vector<std::string>& _available_extensions_selector
    );

    /**
     * @brief Creates, binds and configures a reader service.
     * @param[in] _service_id reader implementation identifier.
     * @return The configured reader service.
     */
    sight::io::service::reader::sptr create_and_configure_reader(const std::string& _service_id);

    /**
     * @brief Creates, binds and configures a writer service.
     * @param[in] _service_id writer implementation identifier.
     * @return The configured writer service.
     */
    sight::io::service::writer::sptr create_and_configure_writer(const std::string& _service_id);

    /**
     * @brief Builds a shared file dialog from reader extensions and executes the reader matching the selected file.
     * @param[in] _available_services pairs containing each reader implementation identifier and display name.
     */
    void select_file_reader(const std::vector<std::pair<std::string, std::string> >& _available_services);

    /**
     * @brief Selects a reader and lets it open its own location dialog before execution.
     * @param[in] _available_services pairs containing each reader implementation identifier and display name.
     */
    void select_folder_reader(const std::vector<std::pair<std::string, std::string> >& _available_services);

    void forward_notification(core::notification::base::sptr _notification);

    /// Configure if selected services are included or excluded.
    bool m_services_are_excluded {true};

    /**
     * @brief Services to include or exclude from the available services.
     *
     * @see selector::m_services_are_excluded.
     */
    std::vector<std::string> m_selected_services;

    /// Maps each service implementation to its optional service configuration.
    std::map<std::string, std::string> m_service_to_config;

    sight::sptr<signals::failed_t> m_sig_failed;
    sight::sptr<signals::succeeded_t> m_sig_succeeded;

    sight::sptr<slots::forward_notification_t> m_slot_forward_notification;

    data::ptr<data::object, data::access::inout> m_read {this, "data.read"};
    data::ptr<data::object, data::access::in> m_write {this, "data.write"};
};

} // namespace sight::module::ui::io
