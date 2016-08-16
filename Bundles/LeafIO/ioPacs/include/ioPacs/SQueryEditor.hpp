/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOPACS_SQUERYEDITOR_HPP__
#define __IOPACS_SQUERYEDITOR_HPP__

#include "ioPacs/config.hpp"

#include <fwPacsIO/data/PacsConfiguration.hpp>
#include <fwPacsIO/SeriesEnquirer.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <gui/editor/IEditor.hpp>

#include <QDateEdit>
#include <QLineEdit>
#include <QObject>
#include <QPointer>
#include <QPushButton>
#include <QWidget>

#include <boost/filesystem/path.hpp>

namespace ioPacs
{

/**
 * @brief   This editor service is used to perform a IOPACS query on a pacs.
 */
class IOPACS_CLASS_API SQueryEditor : public QObject,
                                      public ::gui::editor::IEditor
{
Q_OBJECT;

public:

    fwCoreServiceClassDefinitionsMacro ( (SQueryEditor)( ::gui::editor::IEditor ) );
    /**
     * @brief Constructor
     */
    IOPACS_API SQueryEditor() throw();

    /**
     * @brief Destructor
     */
    IOPACS_API virtual ~SQueryEditor() throw();

protected:

    /**
     * @brief Configuring method. This method is used to configure the service.
     *
     * XML configuration sample:
       @code{.xml}
       <service uid="queryEditor" impl="::ioPacs::SQueryEditor" autoConnect="yes">
         <config pacsConfigurationUID="pacsConfiguration" />
       </service>
       @endcode
     */
    IOPACS_API virtual void configuring() throw(::fwTools::Failed);

    /// Override
    IOPACS_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IOPACS_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IOPACS_API void updating() throw(::fwTools::Failed);

    /// Override
    IOPACS_API void info(std::ostream& _sstream );

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

    /// Pacs Configuration UID
    std::string m_pacsConfigurationUID;

    /// Pacs Configuration object
    ::fwPacsIO::data::PacsConfiguration::sptr m_pacsConfiguration;


};

} // namespace ioPacs

#endif // __IOPACS_SQUERYEDITOR_HPP__
