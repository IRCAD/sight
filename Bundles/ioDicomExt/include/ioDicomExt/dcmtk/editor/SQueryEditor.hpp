/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOMEXT_DCMTK_EDITOR_SQUERYEDITOR_HPP__
#define __IODICOMEXT_DCMTK_EDITOR_SQUERYEDITOR_HPP__

#include <boost/filesystem/path.hpp>
#include <QObject>
#include <QWidget>
#include <QPointer>
#include <QLineEdit>
#include <QPushButton>
#include <QDateEdit>

#include <gui/editor/IEditor.hpp>
#include <fwDicomIOExt/data/PacsConfiguration.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwDicomIOExt/dcmtk/SeriesEnquirer.hpp>

#include "ioDicomExt/config.hpp"

namespace ioDicomExt
{

namespace dcmtk
{

namespace editor
{

/**
 * @brief   This editor service is used to perform a IODICOMEXT query on a pacs.
 * @class   SQueryEditor
 * @date    2013.
 */
class IODICOMEXT_CLASS_API SQueryEditor : public QObject, public ::gui::editor::IEditor
{
Q_OBJECT;

public :

    fwCoreServiceClassDefinitionsMacro ( (SQueryEditor)( ::gui::editor::IEditor ) ) ;
    /**
     * @brief Constructor
     */
    IODICOMEXT_API SQueryEditor() throw();

    /**
     * @brief Destructor
     */
    IODICOMEXT_API virtual ~SQueryEditor() throw();

protected:

    /**
     * @brief Configuring method. This method is used to configure the service.
     *
     * XML configuration sample:
     @verbatim
     <service uid="queryEditor" impl="::ioDicomExt::dcmtk::editor::SQueryEditor" autoConnect="yes">
         <config pacsConfigurationUID="pacsConfiguration" />
     </service>
     @endverbatim
    */
    IODICOMEXT_API virtual void configuring() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API void updating() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API virtual void receiving( CSPTR(::fwServices::ObjectMsg) _msg ) throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API void info(std::ostream &_sstream ) ;

    /**
     * @brief Display an error message
     * @param[in] message Error message to display
     */
    IODICOMEXT_API void displayErrorMessage(const std::string& message) const;

private Q_SLOTS:
    /// Slot called when querying on patient name
    IODICOMEXT_API void queryPatientName();

    /// Slot called when querying on study date
    IODICOMEXT_API void queryStudyDate();

    /**
     * @brief Update the seriesDB with the series retrieved from the pacs
     * @param[in] series Series which must be added to the SeriesDB
     */
    IODICOMEXT_API void updateSeriesDB(::fwMedData::SeriesDB::ContainerType series);

protected :
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
    ::fwDicomIOExt::dcmtk::SeriesEnquirer::sptr m_seriesEnquirer;

    /// Pacs Configuration UID
    std::string m_pacsConfigurationUID;

    /// Pacs Configuration object
    ::fwDicomIOExt::data::PacsConfiguration::sptr m_pacsConfiguration;


};

} // namespace editor
} // namespace dcmtk
} // namespace ioDicomExt

#endif // __IODICOMEXT_DCMTK_EDITOR_SQUERYEDITOR_HPP__
