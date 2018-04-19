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
 * @brief   This editor service is used to perform a IOPACS query on a pacs.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::ioDicomWeb::SQueryEditor">
            <in key="pacsConfig" uid="..." />
            <inout key="seriesDB" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b pacsConfig [::fwDicomWebIO::data::PacsConfiguration]: PACS configuration data.
 * @subsection In-Out In-Out:
 * - \b seriesDB [::fwData::Object]: seriesDB where to push the queried data.
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

    /// Configuring method. This method is used to configure the service.
    IODICOMWEB_API virtual void configuring() override;

    /// Override
    IODICOMWEB_API virtual void starting() override;

    /// Override
    IODICOMWEB_API virtual void stopping() override;

    /// Override
    IODICOMWEB_API void updating() override;

    /// Override
    IODICOMWEB_API void info(std::ostream& _sstream ) override;

    /**
     * @brief Display an error message
     * @param[in] message Error message to display
     */
    IODICOMWEB_API void displayErrorMessage(const std::string& message) const;

private Q_SLOTS:
    /// Slot called when querying on patient name
    IODICOMWEB_API void queryPatientName();

    /// Slot called when querying on study date
    IODICOMWEB_API void queryStudyDate();

private:
    /**
     * @brief Update the seriesDB with the series retrieved from the pacs
     * @param[in] series Series which must be added to the SeriesDB
     */
    IODICOMWEB_API void updateSeriesDB(::fwMedData::SeriesDB::ContainerType series);

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
