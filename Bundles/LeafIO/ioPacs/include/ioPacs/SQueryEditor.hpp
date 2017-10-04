/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOPACS_SQUERYEDITOR_HPP__
#define __IOPACS_SQUERYEDITOR_HPP__

#include "ioPacs/config.hpp"

#include <fwMedData/SeriesDB.hpp>

#include <fwPacsIO/data/PacsConfiguration.hpp>
#include <fwPacsIO/SeriesEnquirer.hpp>

#include <gui/editor/IEditor.hpp>

#include <boost/filesystem/path.hpp>

#include <QDateEdit>
#include <QLineEdit>
#include <QObject>
#include <QPointer>
#include <QPushButton>
#include <QWidget>

namespace ioPacs
{

/**
 * @brief   This editor service is used to perform a IOPACS query on a pacs.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::ioPacs::SQueryEditor">
            <in key="pacsConfig" uid="..." />
            <inout key="seriesDB" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b pacsConfig [::fwPacsIO::data::PacsConfiguration]: PACS configuration data.
 * @subsection In-Out In-Out:
 * - \b seriesDB [::fwData::Object]: seriesDB where to push the queried data.
 */
class IOPACS_CLASS_API SQueryEditor : public QObject,
                                      public ::gui::editor::IEditor
{
Q_OBJECT;

public:

    fwCoreServiceClassDefinitionsMacro( (SQueryEditor)( ::gui::editor::IEditor ) );

    /// Constructor
    IOPACS_API SQueryEditor() noexcept;

    /// Destructor
    IOPACS_API virtual ~SQueryEditor() noexcept;

protected:

    /// Configuring method. This method is used to configure the service.
    IOPACS_API virtual void configuring() override;

    /// Override
    IOPACS_API virtual void starting() override;

    /// Override
    IOPACS_API virtual void stopping() override;

    /// Override
    IOPACS_API void updating() override;

    /// Override
    IOPACS_API void info(std::ostream& _sstream ) override;

    /**
     * @brief Display an error message
     * @param[in] message Error message to display
     */
    IOPACS_API void displayErrorMessage(const std::string& message) const;

private Q_SLOTS:
    /// Slot called when querying on patient name
    IOPACS_API void queryPatientName();

    /// Slot called when querying on study date
    IOPACS_API void queryStudyDate();

    /**
     * @brief Update the seriesDB with the series retrieved from the pacs
     * @param[in] series Series which must be added to the SeriesDB
     */
    IOPACS_API void updateSeriesDB(::fwMedData::SeriesDB::ContainerType series);

protected:
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

    /// Series enquirer
    ::fwPacsIO::SeriesEnquirer::sptr m_seriesEnquirer;

    /// Pacs Configuration object
    ::fwPacsIO::data::PacsConfiguration::csptr m_pacsConfiguration;
};

} // namespace ioPacs

#endif // __IOPACS_SQUERYEDITOR_HPP__
