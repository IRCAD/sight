/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "modules/io/dicomweb/config.hpp"

#include <data/series_set.hpp>

#include <io/http/ClientQt.hpp>

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
class MODULE_IO_DICOMWEB_CLASS_API query_editor : public QObject,
                                                  public sight::ui::editor
{
Q_OBJECT;

public:

    SIGHT_DECLARE_SERVICE(query_editor, sight::ui::editor);

    /// Constructor
    MODULE_IO_DICOMWEB_API query_editor() noexcept;

    /// Destructor
    MODULE_IO_DICOMWEB_API ~query_editor() noexcept override;

protected:

    /// Gets the configurations.
    MODULE_IO_DICOMWEB_API void configuring() override;

    /// Creates the widgets & connect the signals.
    MODULE_IO_DICOMWEB_API void starting() override;

    /// Disconnect the signals and destroy the widgets.
    MODULE_IO_DICOMWEB_API void stopping() override;

    /// Does nothing.
    MODULE_IO_DICOMWEB_API void updating() override;

private Q_SLOTS:

    /// Slot called when querying on patient name
    void queryPatientName();

    /// Slot called when querying on study date
    void queryStudyDate();

private:

    /**
     * @brief Display an error message
     * @param[in] message Error message to display
     */
    static void displayErrorMessage(const std::string& _message);

    /**
     * @brief Update the series_set with the series retrieved from the pacs
     * @param[in] series Series which must be added to the series_set
     */
    void updateSeriesSet(const data::series_set::container_type& _series);

    /// Patient Name field
    QPointer<QLineEdit> m_patientNameLineEdit;

    /// Patient Name Query Button
    QPointer<QPushButton> m_patientNameQueryButton;

    /// Begin study date widget
    QPointer<QDateEdit> m_beginStudyDateEdit;

    /// End study date widget
    QPointer<QDateEdit> m_endStudyDateEdit;

    /// Study Date Query Button
    QPointer<QPushButton> m_studyDateQueryButton;

    /// Http Qt Client
    sight::io::http::ClientQt m_clientQt;

    /// Server hostname preference key
    std::string m_serverHostnameKey;

    /// Server port preference key
    std::string m_serverPortKey;

    /// Server hostname
    std::string m_serverHostname {"localhost"};

    /// Server port
    int m_serverPort {4242};

    sight::data::ptr<sight::data::series_set, sight::data::Access::inout> m_series_set {this, "seriesSet"};
};

} // namespace sight::module::io::dicomweb
