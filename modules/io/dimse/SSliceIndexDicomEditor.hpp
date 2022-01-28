/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
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

#include <core/thread/Timer.hpp>
#include <core/thread/Worker.hpp>

#include <data/DicomSeries.hpp>
#include <data/Image.hpp>
#include <data/SeriesDB.hpp>

#include <io/base/service/IReader.hpp>
#include <io/dimse/data/PacsConfiguration.hpp>

#include <service/IHasServices.hpp>

#include <ui/base/IEditor.hpp>

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
    <service type="sight::module::io::dimse::SSliceIndexDicomEditor">
        <in key="pacsConfig" uid="..." />
        <inout key="series" uid="..." />
        <inout key="image" uid="..." />
        <config delay="500" dicomReader="sight::module::io::dicom::SSeriesDBReader" readerConfig="config" />
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b pacsConfig [sight::io::dimse::data::PacsConfiguration]: PACS configuration data.
 *
 * @subsection In-Out In-Out:
 * - \b series [sight::data::DicomSeries]: DICOM series where to extract the images.
 * - \b image [sight::data::Image]: downloaded slice.
 *
 * @subsection Configuration Configuration:
 * - \b delay (optional, unsigned, default=500): delay to wait between each slice move.
 * - \b dicomReader (mandatory, string): reader type to use.
 * - \b readerConfig (optional, string, default=""): configuration for the DICOM Reader.
 */
class MODULE_IO_DIMSE_CLASS_API SSliceIndexDicomEditor final :
    public QObject,
    public sight::ui::base::IEditor,
    public service::IHasServices
{
Q_OBJECT;

public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SSliceIndexDicomEditor, sight::ui::base::IEditor);

    /// Creates the service.
    MODULE_IO_DIMSE_API SSliceIndexDicomEditor() noexcept;

    /// Destroys the service.
    MODULE_IO_DIMSE_API virtual ~SSliceIndexDicomEditor() noexcept;

protected:

    /// Configures the service.
    MODULE_IO_DIMSE_API void configuring() override;

    /// Creates the slider.
    MODULE_IO_DIMSE_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::DicomSeries::s_MODIFIED_SIG of s_DICOMSERIES_INOUT to s_UPDATE_SLOT
     */
    MODULE_IO_DIMSE_API service::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Updates slider informations and retrieve the image.
    MODULE_IO_DIMSE_API void updating() override;

    /// Destroys the slider.
    MODULE_IO_DIMSE_API void stopping() override;

private Q_SLOTS:

    /// Updates slider information and trigger the slice puller timer.
    void changeSliceIndex(int _value);

private:

    /// Fills editor information.
    void setSliderInformation(unsigned _value);

    /// Retrieves the local slice or pull it, then, read it.
    void retrieveSlice();

    /**
     * @brief Pulls the slice from the PACS.
     * @param _selectedSliceIndex index of the slice to pull.
     */
    void pullSlice(std::size_t _selectedSliceIndex) const;

    /**
     * @brief Reads a local slice.
     * @param _dicomSeries the dicom series instance.
     * @param _selectedSliceIndex index of the slice to read.
     */
    void readSlice(
        const data::mt::locked_ptr<data::DicomSeries>& _dicomSeries,
        std::size_t _selectedSliceIndex
    ) const;

    /// Contains the worker of the series enquire thread.
    core::thread::Worker::sptr m_requestWorker;

    /// Contains the slider.
    QPointer<QSlider> m_slider {nullptr};

    /// Contains the slider informations.
    QPointer<QLineEdit> m_lineEdit {nullptr};

    /// Contains the timer used to trigger the new slice retrieving.
    core::thread::Timer::sptr m_sliceTrigger {nullptr};

    /// Defines the delay to wait to trigger a slice retrieving.
    unsigned int m_delay {500};

    /// Defines the DICOM reader implementation.
    std::string m_dicomReaderImplementation {""};

    /// Contains the optional configuration to set to reader implementation.
    std::string m_readerConfig {""};

    /// Contains the DICOM reader.
    sight::io::base::service::IReader::sptr m_dicomReader;

    /// Contains the seriesDB where the DICOM reader sets its output.
    data::SeriesDB::sptr m_seriesDB;

    data::ptr<sight::io::dimse::data::PacsConfiguration, data::Access::in> m_config {this, "pacsConfig"};
    data::ptr<sight::data::Image, data::Access::inout> m_image {this, "image"};

    static constexpr std::string_view s_DICOMSERIES_INOUT = "series";
    data::ptr<sight::data::DicomSeries, data::Access::inout> m_series {this, s_DICOMSERIES_INOUT};
};

} // namespace sight::module::io::dimse.
