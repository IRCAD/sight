/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <data/vector.hpp>

#include <io/dimse/data/pacs_configuration.hpp>
#include <io/dimse/series_enquirer.hpp>

#include <service/controller.hpp>

namespace sight::data
{

class series;

} // namespace sight::data

namespace sight::module::io::dimse
{

/**
 * @brief   This service is used to push a DICOM series to a PACS.
 *
 * @section Signals Signals
 * - \b progressed(std::string) : Signal to start the progress (bar id).
 * - \b started_progress(std::string, float, std::string) :  Signal to update the progress (bar id, percentage,
 * message).
 * - \b stopped_progress(std::string) : Signal to stop the progress (bar id).
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::io::dimse::series_pusher">
            <in key="pacsConfig" uid="..." />
            <in key="selectedSeries" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b pacsConfig [sight::io::dimse::data::pacs_configuration]: PACS configuration data.
 * - \b selectedSeries [sight::data::vector]: List of DICOM series to push to the PACS.
 */
class series_pusher : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(series_pusher, service::controller);

    using dicom_series_container_t = std::vector<std::shared_ptr<const data::series> >;

    static const core::com::slots::key_t DISPLAY_SLOT;
    using display_message_slot_t = core::com::slot<void (const std::string&, bool)>;

    /// Signal to start the progress (bar id)
    using started_progress_signal_t = core::com::signal<void (std::string)>;
    /// Signal to update the progress (bar id, percentage, message)
    using progressed_signal_t = core::com::signal<void (std::string, float, std::string)>;
    /// Signal to stop the progress (bar id)
    using stopped_progress_signal_t = core::com::signal<void (std::string)>;

    /// Key in m_signals map of signal m_sigProgressed
    static const core::com::signals::key_t PROGRESSED_SIG;
    static const core::com::signals::key_t STARTED_PROGRESS_SIG;
    static const core::com::signals::key_t STOPPED_PROGRESS_SIG;

    /**
     * @brief Constructor
     */
    series_pusher() noexcept;

    /**
     * @brief Destructor
     */
    ~series_pusher() noexcept override;

protected:

    /// Does nothing.
    void configuring() override;

    /// Override
    void starting() override;

    /// Override
    void stopping() override;

    /// Override
    void updating() override;

    /// Override
    void info(std::ostream& _sstream) override;

protected:

    /**
     * @brief Check whether some series are already on the PACS
     * @return True if the series must be pushed
     */
    bool check_series_on_pacs();

    /// Pull Series
    void push_series();

    /**
     * @brief Display a message
     * @param[in] _message Message to display
     * @param[in] _error True if the message is an error messsage
     */
    static void display_message(const std::string& _message, bool _error);

    /**
     * @brief Progress callback
     * @param[in] _series_instance_uid Series instance UID
     * @param[in] _instance_number Instance number
     * @param[in] _file_path File path
     */
    void progress_callback(
        const std::string& _series_instance_uid,
        unsigned int _instance_number,
        const std::string& _file_path
    );

    /// Slot to call displayMessage method;
    display_message_slot_t::sptr m_slot_display_message;

    /// Slot to call progress_callback method
    sight::io::dimse::series_enquirer::progress_callback_slot_t::sptr m_slot_progress_callback;

    /// Signal emitted when the bar is progressing
    progressed_signal_t::sptr m_sig_progressed;

    /// Signal emitted when the bar is starting
    started_progress_signal_t::sptr m_sig_started_progress;

    /// Signal emitted when the bar is stopping
    stopped_progress_signal_t::sptr m_sig_stopped_progress;

    /// Progress Bar ID
    std::string m_progressbar_id;

    /// Series enquirer
    sight::io::dimse::series_enquirer::sptr m_series_enquirer;

    /// Push Worker
    core::thread::worker::sptr m_push_series_worker;

    /// Is pushing is set to true when we are pushing series
    bool m_is_pushing {false};

    /// Total number of instances that must be uploaded
    std::uint64_t m_instance_count {};

    sight::data::ptr<sight::data::vector, sight::data::access::in> m_selected_series {this, "selectedSeries"};
    sight::data::ptr<sight::io::dimse::data::pacs_configuration, sight::data::access::in> m_config {this, "pacsConfig"};
};

} // namespace sight::module::io::dimse
