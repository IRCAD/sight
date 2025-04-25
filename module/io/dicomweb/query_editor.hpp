/************************************************************************
 *
 * Copyright (C) 2018-2025 IRCAD France
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

#include <data/integer.hpp>
#include <data/series_set.hpp>
#include <data/string.hpp>

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
            <properties host_name="${host_name}" port="${port_id}" />
            <config icon="..." />
       </service>
   @endcode
 *
 * @subsection In-Out In-Out:
 * - \b seriesSet [sight::data::series_set]: series_set on which the queried data will be pushed.
 *
 * @subsection Properties Properties
 * - \b host_name : Need hostname string (default value is "127.0.0.1").
 * - \b port : Need the value of port (default value is 8042).
 *
 * @subsection Configuration Configuration:
 * - \b advanced (optional, bool, default=true): define if advanced fields are displayed.
 * - \b icon (optional, string, default=""): path of the icon used in the search button.
 * - \b width (optional, unsigned int, default=20): width of the icon used in the search button.
 * - \b height (optional, unsigned int, default=20): height of the icon used in the search button.
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

    /// Defines the path of the button's icon.
    std::filesystem::path m_icon_path {};

    /// Server port preference key
    std::string m_server_port_key;

    /// Defines the with of the button's icon.
    unsigned int m_icon_width {20};

    /// Defines the height of the button's icon.
    unsigned int m_icon_height {20};

    /// Defines if advanced fields are displayed.
    bool m_advanced {true};

    sight::data::ptr<sight::data::series_set, sight::data::access::inout> m_series_set {this, "series_set"};

    sight::data::property<sight::data::string> m_server_hostname {this, "host_name", std::string("localhost")};
    sight::data::property<sight::data::integer> m_server_port {this, "port", 4242};
};

} // namespace sight::module::io::dicomweb
