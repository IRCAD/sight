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

#include <core/thread/timer.hpp>
#include <core/thread/worker.hpp>

#include <data/dicom_series.hpp>
#include <data/image.hpp>
#include <data/series_set.hpp>

#include <io/__/service/reader.hpp>
#include <io/dimse/data/pacs_configuration.hpp>

#include <service/has_services.hpp>
#include <service/notifier.hpp>

#include <ui/__/editor.hpp>

#include <QLineEdit>
#include <QObject>
#include <QPointer>
#include <QSlider>

namespace sight::module::io::dimse
{

/**
 * @brief This editor service is used to select a slice index and pull the image from the pacs if it is not
 *        available on the local computer.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::io::dimse::slice_index_dicom_editor">
        <in key="pacsConfig" uid="..." />
        <inout key="series" uid="..." />
        <inout key="image" uid="..." />
        <config delay="500" dicomReader="sight::module::io::dicom::series_set_reader" readerConfig="config" />
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b pacsConfig [sight::io::dimse::data::pacs_configuration]: PACS configuration data.
 *
 * @subsection In-Out In-Out:
 * - \b series [sight::data::dicom_series]: DICOM series where to extract the images.
 * - \b image [sight::data::image]: downloaded slice.
 *
 * @subsection Configuration Configuration:
 * - \b delay (optional, unsigned, default=500): delay to wait between each slice move.
 * - \b dicomReader (mandatory, string): reader type to use.
 * - \b readerConfig (optional, string, default=""): configuration for the DICOM Reader.
 */
class MODULE_IO_DIMSE_CLASS_API slice_index_dicom_editor final :
    public QObject,
    public sight::ui::editor,
    public sight::service::has_services,
    private sight::service::notifier
{
Q_OBJECT;

public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(slice_index_dicom_editor, sight::ui::editor);

    /// Creates the service.
    MODULE_IO_DIMSE_API slice_index_dicom_editor() noexcept;

    /// Destroys the service.
    MODULE_IO_DIMSE_API ~slice_index_dicom_editor() noexcept override = default;

protected:

    /// Configures the service.
    MODULE_IO_DIMSE_API void configuring(const config_t& _config) override;

    /// Creates the slider.
    MODULE_IO_DIMSE_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::dicom_series::MODIFIED_SIG of s_DICOMSERIES_INOUT to service::slots::UPDATE
     */
    MODULE_IO_DIMSE_API service::connections_t auto_connections() const override;

    /// Updates slider informations and retrieve the image.
    MODULE_IO_DIMSE_API void updating() override;

    /// Destroys the slider.
    MODULE_IO_DIMSE_API void stopping() override;

private Q_SLOTS:

    /// Updates slider information and trigger the slice puller timer.
    void change_slice_index(int _value);

private:

    /// Fills editor information.
    void set_slider_information(unsigned _value);

    /// Retrieves the local slice or pull it, then, read it.
    void retrieve_slice();

    /**
     * @brief Pulls the slice from the PACS.
     * @param _selectedSliceIndex index of the slice to pull.
     */
    void pull_slice(std::size_t _selected_slice_index) const;

    /**
     * @brief Reads a local slice.
     * @param _dicomSeries the dicom series instance.
     * @param _selectedSliceIndex index of the slice to read.
     */
    void read_slice(
        const data::mt::locked_ptr<data::dicom_series>& _dicom_series,
        std::size_t _selected_slice_index
    ) const;

    /// Contains the worker of the series enquire thread.
    core::thread::worker::sptr m_request_worker;

    /// Contains the slider.
    QPointer<QSlider> m_slider {nullptr};

    /// Contains the slider informations.
    QPointer<QLineEdit> m_line_edit {nullptr};

    /// Contains the timer used to trigger the new slice retrieving.
    core::thread::timer::sptr m_slice_trigger {nullptr};

    /// Defines the delay to wait to trigger a slice retrieving.
    unsigned int m_delay {500};

    /// Defines the DICOM reader implementation.
    std::string m_dicom_reader_implementation;

    /// Contains the optional configuration to set to reader implementation.
    std::string m_reader_config;

    /// Contains the DICOM reader.
    sight::io::service::reader::sptr m_dicom_reader;

    /// Contains the series_set where the DICOM reader sets its output.
    data::series_set::sptr m_series_set;

    data::ptr<sight::io::dimse::data::pacs_configuration, data::access::in> m_config {this, "pacsConfig"};
    data::ptr<sight::data::image, data::access::inout> m_image {this, "image"};

    static constexpr std::string_view DICOMSERIES_INOUT = "series";
    data::ptr<sight::data::dicom_series, data::access::inout> m_series {this, DICOMSERIES_INOUT};
};

} // namespace sight::module::io::dimse.
