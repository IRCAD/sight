/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "modules/io/dicomweb/config.hpp"

#include <io/http/ClientQt.hpp>

#include <ui/base/IEditor.hpp>

#include <QLineEdit>
#include <QSlider>

#include <filesystem>

namespace sight
{
namespace core::thread
{
class Timer;
}

namespace io::base::service
{
class IReader;
}

namespace data
{
class Integer;
class SeriesDB;
}

namespace core::runtime
{
struct ConfigurationElement;
}
}

namespace sight::module::io::dicomweb
{

class MODULE_IO_DICOMWEB_CLASS_API SSliceIndexDicomPullerEditor : public QObject,
                                                                  public sight::ui::base::IEditor
{
Q_OBJECT;

public:

    SIGHT_DECLARE_SERVICE(SSliceIndexDicomPullerEditor,  sight::ui::base::IEditor )

    /**
     * @brief Constructor
     */
    MODULE_IO_DICOMWEB_API SSliceIndexDicomPullerEditor() noexcept;

    /**
     * @brief Destructor
     */
    MODULE_IO_DICOMWEB_API virtual ~SSliceIndexDicomPullerEditor() noexcept;

private Q_SLOTS:
    /**
     * @brief Slot called when the slider is moved
     * @param[in] value Slider value
     */
    void changeSliceIndex(int value);

protected:

    /// Gets the configurations and creates a timer on a worker.
    MODULE_IO_DICOMWEB_API void configuring() override;

    /// Creates the wigdets, connect the signal, register the DICOM reader and starts the callback timer.
    MODULE_IO_DICOMWEB_API void starting() override;

    /// Stops the timer, unregister the DICOM reader and destroy the created widgets.
    MODULE_IO_DICOMWEB_API void stopping() override;

    /// Does nothing.
    MODULE_IO_DICOMWEB_API void updating() override;

private:
    /// Function called when a new slice must be displayed.
    void triggerNewSlice();

    /**
     * @brief Read the selected image
     * @param[in] selectedSliceIndex Selected slice of the image that must be read
     */
    void readImage(size_t selectedSliceIndex);

    /// Pull the selected slice from the Pacs
    void pullInstance();

    /**
     * @brief Displays a dialog box with the error message
     */
    void displayErrorMessage(const std::string& message) const;

    /// Slice index slider
    QPointer< QSlider > m_sliceIndexSlider;

    /// Slice index line edit
    QPointer< QLineEdit > m_sliceIndexLineEdit;

    /// Number of instances
    size_t m_numberOfSlices;

    /// IODICOMWEB Reader
    std::string m_dicomReaderType;

    /// Reader
    WPTR(sight::io::base::service::IReader) m_dicomReader;

    /// Image Key
    std::string m_imageKey;

    /// Temporary SeriesDB
    SPTR(data::SeriesDB) m_tempSeriesDB;

    /// Axial slice index
    SPTR(data::Integer) m_axialIndex;
    /// Frontal slice index
    SPTR(data::Integer) m_frontalIndex;
    /// Sagittal slice index
    SPTR(data::Integer) m_sagittalIndex;

    /// Series enquirer
    sight::io::http::ClientQt m_clientQt;

    /// Timer used to generate the new slice selection delay
    SPTR(core::thread::Timer) m_delayTimer;

    /// Delay
    unsigned int m_delay;

    /// Optional configuration to set to reader implementation
    SPTR(core::runtime::ConfigurationElement) m_readerConfig;

    /// Server hostname preference key
    std::string m_serverHostnameKey;

    /// Server port preference key
    std::string m_serverPortKey;

    /// Server hostname
    std::string m_serverHostname{"localhost"};

    /// Server port
    int m_serverPort{4242};

};

} // namespace sight::module::io::dicomweb
