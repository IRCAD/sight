/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "ioPacs/config.hpp"

#include <fwGui/editor/IEditor.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <fwPacsIO/data/PacsConfiguration.hpp>
#include <fwPacsIO/SeriesEnquirer.hpp>

#include <QDateEdit>
#include <QLineEdit>
#include <QObject>
#include <QPointer>
#include <QPushButton>

namespace ioPacs
{

/**
 * @brief This editor allows to perform queries on a pacs.
 *
 * Queries results are stored in a SeriesDB where each Series is a DicomSeries.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::ioPacs::SQueryEditor">
            <in key="pacsConfig" uid="..." />
            <inout key="seriesDB" uid="..." />
            <config icon="..." />
       </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b pacsConfig [::fwPacsIO::data::PacsConfiguration]: PACS configuration data.
 *
 * @subsection In-Out In-Out:
 * - \b seriesDB [::fwData::Object]: seriesDB where to push the queried data.
 *
 * @subsection Configuration Configuration:
 * - \b icon (optional, string, default=""): path of the icon used in the search button.
 * - \b width (optional, unsigned int, default=20): width of the icon used in the search button.
 * - \b height (optional, unsigned int, default=20): height of the icon used in the search button.
 */
class IOPACS_CLASS_API SQueryEditor final :
    public QObject,
    public ::fwGui::editor::IEditor
{

Q_OBJECT;

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SQueryEditor,  ::fwGui::editor::IEditor )

    /// Creates the service.
    IOPACS_API SQueryEditor() noexcept;

    /// Destroyes the service.
    IOPACS_API virtual ~SQueryEditor() noexcept;

private:

    /// Configures the editor.
    virtual void configuring() override;

    /// Creates the GUI and connect widget to @ref updateSeriesDB(const ::fwMedData::SeriesDB::ContainerType&).
    virtual void starting() override;

    /// Executes a query with last settings.
    void updating() override;

    /// Destroys the GUI and disconnect signals.
    virtual void stopping() override;

    /**
     * @brief Adds series in the series DB.
     * @param _series series to add.
     */
    void updateSeriesDB(const ::fwMedData::SeriesDB::ContainerType& _series);

    /// Defines the path of the button's icon.
    std::filesystem::path m_iconPath {};

    /// Defines the with of the button's icon.
    unsigned int m_iconWidth {20};

    /// Defines the height of the button's icon.
    unsigned int m_iconHeight {20};

    /// Contains the search line editor.
    QPointer< QLineEdit > m_searchEdit;

    /// Contains the search button.
    QPointer< QPushButton > m_searchButton;

    /// Contains the begin date line editor.
    QPointer< QDateEdit > m_beginStudyDateEdit;

    /// Contains the end date line editor.
    QPointer< QDateEdit > m_endStudyDateEdit;

    /// Contains the name line editor.
    QPointer< QLineEdit > m_patientNameEdit;

    /// Contains the patient id line editor.
    QPointer< QLineEdit > m_patientUIDEdit;

    /// Contains the uid line editor.
    QPointer< QLineEdit > m_seriesUIDEdit;

    /// Contains the description line editor.
    QPointer< QLineEdit > m_seriesDescriptionEdit;

    /// Contains the modality line editor.
    QPointer< QLineEdit > m_seriesModalityEdit;

    /// Contains the configuratuion of the PACS.
    ::fwPacsIO::data::PacsConfiguration::csptr m_pacsConfiguration;

private Q_SLOTS:

    /// Executes a query and fills the result in the series DB.
    void executeQuery();

};

} // namespace ioPacs.
