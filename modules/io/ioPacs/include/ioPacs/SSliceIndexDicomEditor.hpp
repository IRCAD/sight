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

#include <fwIO/IReader.hpp>

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwServices/IHasServices.hpp>

#include <fwThread/Timer.hpp>

#include <QLineEdit>
#include <QObject>
#include <QPointer>
#include <QSlider>

namespace ioPacs
{

/**
 * @brief This editor service is used to select a slice index and pull the image from the pacs if it is not
 *        available on the local computer.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="::ioPacs::SSliceIndexDicomEditor">
        <in key="pacsConfig" uid="..." />
        <inout key="series" uid="..." />
        <inout key="image" uid="..." />
        <config delay="500" dicomReader="::ioGdcm::SSeriesDBReader" dicomReaderConfig="config" />
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b pacsConfig [::fwPacsIO::data::PacsConfiguration]: PACS configuration data.
 *
 * @subsection In-Out In-Out:
 * - \b series [::fwMedData::DicomSeries]: DICOM series where to extract the images.
 * - \b image [::fwData::Image]: downloaded slice.
 *
 * @subsection Configuration Configuration:
 * - \b delay (optional, unsigned, default=500): delay to wait between each slice move.
 * - \b dicomReader (mandatory, string): reader type to use.
 * - \b dicomReaderConfig (optional, string, default=""): configuration for the DICOM Reader.
 */
class IOPACS_CLASS_API SSliceIndexDicomEditor final :
    public QObject,
    public ::fwGui::editor::IEditor,
    public ::fwServices::IHasServices
{

Q_OBJECT;

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SSliceIndexDicomEditor,  ::fwGui::editor::IEditor)

    /// Creates the service.
    IOPACS_API SSliceIndexDicomEditor() noexcept;

    /// Destroys the service.
    IOPACS_API virtual ~SSliceIndexDicomEditor() noexcept;

protected:

    /// Configures the service.
    IOPACS_API void configuring() override;

    /// Creates the slider.
    IOPACS_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwMedData::DicomSeries::s_MODIFIED_SIG of s_DICOMSERIES_INOUT to s_UPDATE_SLOT
     */
    IOPACS_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Updates slider informations and retrieve the image.
    IOPACS_API void updating() override;

    /// Destroys the slider.
    IOPACS_API void stopping() override;

private Q_SLOTS:

    /// Updates slider information and trigger the slice puller timer.
    void changeSliceIndex(int _value);

private:

    /// Fills editor information.
    void setSliderInformation(unsigned _value);

    /// Retrieves the local slice or pull it, then, read it.
    void retrieveSlice() const;

    /**
     * @brief Pulls the slice from the PACS.
     * @param _dicomSeries the dicom series instance.
     * @param _selectedSliceIndex index of the slice to pull.
     */
    void pullSlice(const ::fwData::mt::locked_ptr< ::fwMedData::DicomSeries >& _dicomSeries,
                   std::size_t _selectedSliceIndex) const;

    /**
     * @brief Reads a local slice.
     * @param _dicomSeries the dicom series instance.
     * @param _selectedSliceIndex index of the slice to read.
     */
    void readSlice(const ::fwData::mt::locked_ptr< ::fwMedData::DicomSeries >& _dicomSeries,
                   std::size_t _selectedSliceIndex) const;

    /// Contains the slider.
    QPointer< QSlider > m_slider { nullptr };

    /// Contains the slider informations.
    QPointer< QLineEdit > m_lineEdit { nullptr };

    /// Contains the timer used to trigger the new slice retrieving.
    ::fwThread::Timer::sptr m_sliceTriggerer { nullptr };

    /// Defines the delay to wait to trigger a slice retrieving.
    unsigned int m_delay { 500 };

    /// Defines the DICOM reader implementation.
    std::string m_dicomReaderImplementation { "" };

    /// Contains the optional configuration to set to reader implementation.
    std::string m_readerConfig { "" };

    /// Contains the DICOM reader.
    ::fwIO::IReader::sptr m_dicomReader;

    /// Contains the seriesDB where the DICOM reader sets its output.
    ::fwMedData::SeriesDB::sptr m_seriesDB;

};

} // namespace ioPacs.
