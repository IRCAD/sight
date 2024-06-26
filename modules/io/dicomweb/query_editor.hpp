/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include <data/series_set.hpp>

#include <io/http/client_qt.hpp>

#include <ui/__/editor.hpp>

#include <QDateEdit>
#include <QLineEdit>
#include <QObject>
#include <QPointer>
#include <QPushButton>
#include <QWidget>

#include <filesystem>

namespace sight::module::io::dicomweb
{

/**
 * @brief   This editor service is used to perform an HTTP query on a Pacs.
 *
 * @section Slots Slots
 * - \b displayErrorMessage(const std::string&) : display an error message.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::io::dicomweb::query_editor">
            <inout key="seriesSet" uid="..." />
            <server>%PACS_SERVER_HOSTNAME%:%PACS_SERVER_PORT%</server>
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b seriesSet [sight::data::series_set]: series_set on which the queried data will be pushed.
 * @subsection Configuration Configuration:
 * - \b server: server URL. Need hostname and port in this format addr:port (default value is 127.0.0.1:8042).
 * @note : hostname and port of this service can be a value or a nameKey from preference settings
 *  (for example <server>%HOSTNAME%:%PORT%</server>)

 */
class query_editor : public QObject,
                     public sight::ui::editor
{
Q_OBJECT;

public:

    SIGHT_DECLARE_SERVICE(query_editor, sight::ui::editor);

    /// Constructor
    query_editor() noexcept;

    /// Destructor
    ~query_editor() noexcept override;

protected:

    /// Gets the configurations.
    void configuring() override;

    /// Creates the widgets & connect the signals.
    void starting() override;

    /// Disconnect the signals and destroy the widgets.
    void stopping() override;

    /// Does nothing.
    void updating() override;

private Q_SLOTS:

    /// Slot called when querying on patient name
    void query_patient_name();

    /// Slot called when querying on study date
    void query_study_date();

private:

    /**
     * @brief Display an error message
     * @param[in] _message Error message to display
     */
    static void display_error_message(const std::string& _message);

    /**
     * @brief Update the series_set with the series retrieved from the pacs
     * @param[in] _series Series which must be added to the series_set
     */
    void update_series_set(const data::series_set::container_t& _series);

    /// Patient Name field
    QPointer<QLineEdit> m_patient_name_line_edit;

    /// Patient Name Query Button
    QPointer<QPushButton> m_patient_name_query_button;

    /// Begin study date widget
    QPointer<QDateEdit> m_begin_study_date_edit;

    /// End study date widget
    QPointer<QDateEdit> m_end_study_date_edit;

    /// Study Date Query Button
    QPointer<QPushButton> m_study_date_query_button;

    /// Http Qt Client
    sight::io::http::client_qt m_client_qt;

    /// Server hostname preference key
    std::string m_server_hostname_key;

    /// Server port preference key
    std::string m_server_port_key;

    /// Server hostname
    std::string m_server_hostname {"localhost"};

    /// Server port
    int m_server_port {4242};

    sight::data::ptr<sight::data::series_set, sight::data::access::inout> m_series_set {this, "seriesSet"};
};

} // namespace sight::module::io::dicomweb
