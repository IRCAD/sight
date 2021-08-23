/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "modules/ui/qt/config.hpp"

#include <core/com/Signal.hpp>
#include <core/tools/Failed.hpp>

#include <data/Camera.hpp>
#include <data/CameraSeries.hpp>

#include <ui/base/IEditor.hpp>

#include <QComboBox>
#include <QObject>
#include <QPointer>

namespace sight::module::ui::qt
{

namespace video
{

/**
 * @brief This editor allows to select the device to use. It updates the data camera identifier.
 *
 * @section Signals Signals
 * - \b configuredCameras(): emitted when the cameras have been successfully configured.
 * - \b configuredDevice(): emitted when the user selects a device as the video source.
 * - \b configuredFile(): emitted when the user selects a file as the video source.
 * - \b configuredStream(): emitted when the user selects a stream as the video source.
 *
 * @section Slots Slots
 * - \b configureDevice(): configure the cameras as device sources.
 * - \b configureFile(): configure the cameras as file sources.
 * - \b configureStream(): configure the cameras as stream sources.
 *
 * @section XML XML Configuration
 *
 * Configure this service either with a single camera data:
 *
 * @code{.xml}
    <service uid="..." type="sight::module::ui::qt::video::SCamera" >
        <inout key="camera" uid="..."/>
        <videoSupport>true</videoSupport>
        <label>Video source: </label>
    </service>
   @endcode

 * Or with a camera series. The user will be prompted to select the source streams for each camera inside the series.
 * In this case, it is possible to configure the service to create empty cameras with the parameter
 * \b createCameraNumber. This may be useful to load/save camera data without an existing calibration.
 *
 * @code{.xml}
    <service uid="..." type="sight::module::ui::qt::video::SCamera" >
        <inout key="cameraSeries" uid="..."/>
        <createCameraNumber>2</createCameraNumber>
        <videoSupport>true</videoSupport>
        <label>Video source: </label>
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b camera [sight::data::Camera]: camera data.
 * - \b cameraSeries [sight::data::CameraSeries]: camera series thus containing several camera.
 *
 * @subsection Configuration Configuration
 * - \b videoSupport (optional, default="false"): if we can open a video file in addition with cameras.
 * - \b createCameraNumber (optional, default="0"): number of cameras to create. If the parameter is set and the
 * camera series already contains camera data, an assertion will be raised.
 * - \b label (optional, default="Video source: "): label of the selector.
 */
class MODULE_UI_QT_CLASS_API SCamera final : public QObject,
                                             public sight::ui::base::IEditor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(SCamera, sight::ui::base::IEditor);

    /// Initialize signals and slots.
    MODULE_UI_QT_API SCamera();

    /// Destroys the service.
    MODULE_UI_QT_API virtual ~SCamera() noexcept;

protected Q_SLOTS:

    /**
     * @brief Calls when user select another device.
     * @param _index the index of the selected device.
     */
    void onApply(int _index);

private:

    /// Type of the 'configured' signal.
    typedef core::com::Signal<void ()> ConfiguredSignalType;

    /// Configures the service.
    virtual void configuring() final;

    /// Installs the layout.
    virtual void starting() final;

    /// Destroys the layout.
    virtual void stopping() final;

    /// Does nothing.
    virtual void updating() final;

    /// Calls when user select a file.
    void onChooseFile();

    /// Calls when user select a stream.
    void onChooseStream();

    /// Calls when user select a device.
    void onChooseDevice();

    /// Retrieves camera objects according to the XML configuration.
    std::vector<data::Camera::sptr> getCameras() const;

    /// Combobox for camera selection.
    QPointer<QComboBox> m_devicesComboBox;

    /// Offer the possibility to select a video file.
    bool m_bVideoSupport {false};

    /// Number of cameras to create when using a camera series as input.
    size_t m_numCreateCameras {0};

    /// Signal emitted when the cameraSeries has been configured.
    ConfiguredSignalType::sptr m_sigConfiguredCameras;

    /// Label of the selector.
    std::string m_label {"Video source: "};

    static constexpr std::string_view s_CAMERA        = "camera";
    static constexpr std::string_view s_CAMERA_SERIES = "cameraSeries";

    data::ptr<data::Camera, data::Access::inout> m_camera {this, s_CAMERA, true};
    data::ptr<data::CameraSeries, data::Access::inout> m_cameraSeries {this, s_CAMERA_SERIES, true};
};

} // video

} // sight::module::ui::qt
