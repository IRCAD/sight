/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include <core/com/signal.hpp>
#include <core/tools/failed.hpp>

#include <data/camera.hpp>
#include <data/camera_set.hpp>

#include <ui/__/editor.hpp>

#include <QComboBox>
#include <QObject>
#include <QPointer>

namespace sight::module::ui::qt::video
{

/**
 * @brief This editor allows to select the device to use. It updates the data camera identifier.
 *
 * @section Signals Signals
 * - \b configured_cameras(): emitted when the cameras have been successfully configured.
 * - \b configured_device(): emitted when the user selects a device as the video source.
 * - \b configured_file(): emitted when the user selects a file as the video source.
 * - \b configured_stream(): emitted when the user selects a stream as the video source.
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
    <service uid="..." type="sight::module::ui::qt::video::camera" >
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
    <service uid="..." type="sight::module::ui::qt::video::camera" >
        <inout key="cameraSet" uid="..."/>
        <createCameraNumber>2</createCameraNumber>
        <videoSupport>true</videoSupport>
        <useAbsolutePath>false</useAbsolutePath>
        <label>Video source: </label>
        <resolution>min/max/preferences/prompt/640x480</resolution>
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b camera [sight::data::camera]: camera data.
 * - \b cameraSet [sight::data::camera_set]: camera series thus containing several camera.
 *
 * @subsection Configuration Configuration
 * - \b videoSupport (optional, default="false"): if we can open a video file in addition with cameras.
 * - \b useAbsolutePath (optional, default="false"): when using a file input, tells if the path should be stored as
 * absolute or relative to the video preferences directory.
 * - \b createCameraNumber (optional, default="0"): number of cameras to create. If the parameter is set and the
 * camera series already contains camera data, an assertion will be raised.
 * - \b label (optional, default="Video source: "): label of the selector.
 * - \b resolution (optional, default="preferences"): Camera resolution. If 'preferences' is set, the camera resolution
 * will
 * be extracted from the preferences, otherwise, `min`, `max` or a specific value (eg: 640x480) are computed from the
 * camera's supported resolutions. When `prompt` option is set, the camera selection dialog will be always displayed.
 *
 * @section remarks remarks
 * In order to launch directly the video after selecting and configuring the device, remember to connect the
 *`configuredDevice()` signal with your grabber's `startCamera` slot.
 */

class camera final : public QObject,
                     public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(camera, sight::ui::editor);

    /// Initialize signals and slots.
    camera();

    /// Destroys the service.
    ~camera() noexcept override = default;

    // Key saved in the preference file
    static const std::string RESOLUTION_PREF_KEY;

protected Q_SLOTS:

    /**
     * @brief Calls when user select another device.
     * @param _index the index of the selected device.
     */
    void on_apply(int _index);

    /**
     * @brief Calls when user select another device.
     * @param _index the index of the selected device.
     */
    void on_activated(int _index);

    /// Allows setting the camera resolution preference.
    void set_preference();

private:

    /// Type of the 'configured' signal.
    using configured_signal_t = core::com::signal<void ()>;

    /// Configures the service.
    void configuring() final;

    /// Installs the layout.
    void starting() final;

    /// Destroys the layout.
    void stopping() final;

    /// Does nothing.
    void updating() final;

    /// Calls when user select a file.
    void on_choose_file();

    /// Calls when user select a stream.
    void on_choose_stream();

    /// Calls when user select a device.
    void on_choose_device();

    /// Retrieves camera objects according to the XML configuration.
    std::vector<data::camera::sptr> get_cameras() const;

    /// Combobox for camera selection.
    QPointer<QComboBox> m_devices_combo_box;

    /// Offer the possibility to select a video file.
    bool m_b_video_support {false};

    /// Number of cameras to create when using a camera series as input.
    std::size_t m_num_create_cameras {0};

    // Sets the file path as absolute ones
    bool m_use_absolute_path {false};

    /// Signal emitted when the cameraSet has been configured.
    configured_signal_t::sptr m_sig_configured_cameras;

    /// Label of the selector.
    std::string m_label {"Video source: "};

    /// Requested resolution in xml configuration of the service
    std::string m_resolution;

    /// Value extracted from the preferences
    std::string m_camera_resolution_preference;

    bool m_preference_mode {false};

    static constexpr std::string_view CAMERA     = "camera";
    static constexpr std::string_view CAMERA_SET = "cameraSet";

    data::ptr<data::camera, data::access::inout> m_camera {this, CAMERA, false, true};
    data::ptr<data::camera_set, data::access::inout> m_camera_set {this, CAMERA_SET, false, true};

    int m_old_index {};
};

} // namespace sight::module::ui::qt::video
