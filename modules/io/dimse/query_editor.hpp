/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "modules/io/dimse/config.hpp"

#include <core/thread/worker.hpp>

#include <data/series_set.hpp>

#include <io/dimse/data/PacsConfiguration.hpp>
#include <io/dimse/SeriesEnquirer.hpp>

#include <service/notifier.hpp>

#include <ui/__/editor.hpp>

#include <QDateEdit>
#include <QLineEdit>
#include <QObject>
#include <QPointer>
#include <QPushButton>

namespace sight::module::io::dimse
{

/**
 * @brief This editor allows to perform queries on a pacs.
 *
 * Queries results are stored in a series_set where each Series is a DicomSeries.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="sight::module::io::dimse::query_editor">
            <in key="pacsConfig" uid="..." />
            <inout key="seriesSet" uid="..." />
            <config icon="..." />
       </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b pacsConfig [sight::io::dimse::data::PacsConfiguration]: PACS configuration data.
 *
 * @subsection In-Out In-Out:
 * - \b seriesSet [sight::data::object]: series_set where to push the queried data.
 *
 * @subsection Configuration Configuration:
 * - \b advanced (optional, bool, default=true): define if advanced fields are displayed.
 * - \b icon (optional, string, default=""): path of the icon used in the search button.
 * - \b width (optional, unsigned int, default=20): width of the icon used in the search button.
 * - \b height (optional, unsigned int, default=20): height of the icon used in the search button.
 */
class MODULE_IO_DIMSE_CLASS_API query_editor final :
    public QObject,
    public sight::ui::editor,
    private sight::service::notifier
{
Q_OBJECT;

public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(query_editor, sight::ui::editor);

    /// Creates the service.
    MODULE_IO_DIMSE_API query_editor() noexcept;

    /// Destroyes the service.
    MODULE_IO_DIMSE_API ~query_editor() noexcept override = default;

protected:

    /// Configures the editor.
    MODULE_IO_DIMSE_API void configuring() override;

    /// Creates the GUI and connects widget to updateSeriesSet(const data::series_set::container_type&).
    MODULE_IO_DIMSE_API void starting() override;

    /// Executes a query with last settings.
    MODULE_IO_DIMSE_API void updating() override;

    /// Destroys the GUI and disconnect signals.
    MODULE_IO_DIMSE_API void stopping() override;

private:

    /// Executes a query and fills the result in the series set.
    void executeQuery();

    /**
     * @brief Adds series in the series set.
     * @param _series series to add.
     */
    void updateSeriesSet(const data::series_set::container_type& _series);

    /// Contains the worker of the series enquire thread.
    core::thread::worker::sptr m_requestWorker {nullptr};

    /// Defines if the service is executing query.
    std::atomic<bool> m_isQuerying {false};

    /// Defines if advanced fields are displayed.
    bool m_advanced {true};

    /// Defines the path of the button's icon.
    std::filesystem::path m_iconPath {};

    /// Defines the with of the button's icon.
    unsigned int m_iconWidth {20};

    /// Defines the height of the button's icon.
    unsigned int m_iconHeight {20};

    /// Contains the search line editor.
    QPointer<QLineEdit> m_searchEdit;

    /// Contains the search button.
    QPointer<QPushButton> m_searchButton;

    /// Contains the begin date line editor.
    QPointer<QDateEdit> m_beginStudyDateEdit;

    /// Contains the end date line editor.
    QPointer<QDateEdit> m_endStudyDateEdit;

    /// Contains the name line editor.
    QPointer<QLineEdit> m_patientNameEdit;

    /// Contains the birth date line editor.
    QPointer<QDateEdit> m_birthDateEdit;

    /// Contains the patient id line editor.
    QPointer<QLineEdit> m_patientUIDEdit;

    /// Contains the uid line editor.
    QPointer<QLineEdit> m_seriesUIDEdit;

    /// Contains the description line editor.
    QPointer<QLineEdit> m_seriesDescriptionEdit;

    /// Contains the modality line editor.
    QPointer<QLineEdit> m_seriesModalityEdit;

private Q_SLOTS:

    /// Executes a query and fills the result in the series set.
    void executeQueryAsync();

    /**
     * @brief Enables the birth date editor.
     * @param _enable value used as a boolean.
     */
    void enableBirthDateEdit(int _enable);

private:

    data::ptr<sight::io::dimse::data::PacsConfiguration, data::Access::in> m_config {this, "pacsConfig"};
    data::ptr<sight::data::series_set, data::Access::inout> m_series_set {this, "seriesSet"};
};

} // namespace sight::module::io::dimse.
