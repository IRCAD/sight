/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioDicomWeb/config.hpp"

#include <fwGui/editor/IEditor.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <fwNetworkIO/http/ClientQt.hpp>

#include <boost/filesystem/path.hpp>

#include <QDateEdit>
#include <QLineEdit>
#include <QObject>
#include <QPointer>
#include <QPushButton>
#include <QWidget>

namespace ioDicomWeb
{

/**
 * @brief   This editor service is used to perform an HTTP query on a Pacs.
 *
 * @section Slots Slots
 * - \b displayErrorMessage(const std::string&) : display an error message.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::ioDicomWeb::SQueryEditor">
            <inout key="seriesDB" uid="..." />
            <server>%PACS_SERVER_HOSTNAME%:%PACS_SERVER_PORT%</server>
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b seriesDB [::fwMedData::SeriesDB]: SeriesDB on which the queried data will be pushed.
 * @subsection Configuration Configuration:
 * - \b server: server URL. Need hostname and port in this format addr:port (default value is 127.0.0.1:8042).
 * @note : hostname and port of this service can be a value or a nameKey from preference settings
 *  (for example <server>%HOSTNAME%:%PORT%</server>)

 */
class IODICOMWEB_CLASS_API SQueryEditor : public QObject,
                                          public ::fwGui::editor::IEditor
{
Q_OBJECT;

public:

    fwCoreServiceClassDefinitionsMacro( (SQueryEditor)( ::fwGui::editor::IEditor ) );

    /// Constructor
    IODICOMWEB_API SQueryEditor() noexcept;

    /// Destructor
    IODICOMWEB_API virtual ~SQueryEditor() noexcept;

protected:

    /// Gets the configurations.
    IODICOMWEB_API virtual void configuring() override;

    /// Creates the widgets & connect the signals.
    IODICOMWEB_API virtual void starting() override;

    /// Disconnect the signals and destroy the widgets.
    IODICOMWEB_API virtual void stopping() override;

    /// Does nothing.
    IODICOMWEB_API void updating() override;

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
    IODICOMWEB_API void displayErrorMessage(const std::string& message) const;

    /**
     * @brief Update the seriesDB with the series retrieved from the pacs
     * @param[in] series Series which must be added to the SeriesDB
     */
    void updateSeriesDB(::fwMedData::SeriesDB::ContainerType series);

    /// Patient Name Field
    QPointer< QLineEdit > m_patientNameLineEdit;

    /// Patient Name Query Button
    QPointer< QPushButton > m_patientNameQueryButton;

    /// Begin study date widget
    QPointer< QDateEdit > m_beginStudyDateEdit;

    /// End study date widget
    QPointer< QDateEdit > m_endStudyDateEdit;

    /// Study Date Query Button
    QPointer< QPushButton > m_studyDateQueryButton;

    /// Http Qt Client
    ::fwNetworkIO::http::ClientQt m_clientQt;

    /// Returns a well formatted preferences key
    std::string getPreferenceKey(const std::string& key) const;

    /// Server hostname preference key
    std::string m_serverHostnameKey;

    /// Server port preference key
    std::string m_serverPortKey;

    /// Server hostname
    std::string m_serverHostname;

    /// Server port
    int m_serverPort;

};

} // namespace ioDicomWeb
