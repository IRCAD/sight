/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOQT_EDITOR_SCAMERA_HPP__
#define __VIDEOQT_EDITOR_SCAMERA_HPP__

#include "videoQt/config.hpp"

#include <arData/Camera.hpp>

#include <fwCom/Signal.hpp>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include <QComboBox>
#include <QObject>
#include <QPointer>

namespace videoQt
{
namespace editor
{

/**
 * @brief   This editor allows to select the device to use. It updates the data camera identifier.
 *
 * @section Signals Signals
 * - \b configuredCameras(): emitted when the cameras have been successfully configured.
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
    <service uid="..." type="::videoQt::editor::SCamera" autoConnect="no">
        <inout key="camera" uid="..."/>
        <videoSupport>yes</videoSupport>
    </service>
   @endcode

 * Or with a camera series. The user will be prompted to select the source streams for each camera inside the series.
 * In this case, it is possible to configure the service to create empty cameras with the parameter
 * \b createCameraNumber. This may be useful to load/save camera data without an existing calibration.
 *
 * @code{.xml}
    <service uid="..." type="::videoQt::editor::SCamera" autoConnect="no">
        <inout key="cameraSeries" uid="..."/>
        <createCameraNumber>2</createCameraNumber>
        <videoSupport>yes</videoSupport>
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b camera [::arData::Camera]: camera data
 * - \b cameraSeries [::arData::CameraSeries]: camera series, thus containing several camera.
 *
 * @subsection Configuration Configuration
 * - \b videoSupport (optional, by default "no") : if we can open a video file in addition with cameras.
 * - \b createCameraNumber (optional, by default "0") : number of cameras to create. If the parameter is set and the
 * camera series already contains camera data, an assertion will be raised.
 */
class VIDEOQT_CLASS_API SCamera : public QObject,
                                  public ::gui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro( (SCamera)(::gui::editor::IEditor) );

    /// Constructor. Do nothing.
    VIDEOQT_API SCamera() noexcept;

    /// Destructor. Do nothing.
    VIDEOQT_API virtual ~SCamera() noexcept;

    /// Type of the 'configuredCameras' signal
    using ConfiguredCamerasSignalType = ::fwCom::Signal<void()>;

    /// Key of the 'configuredCameras' signal
    VIDEOQT_API static const ::fwCom::Signals::SignalKeyType s_CONFIGURED_CAMERAS_SIG;

    /// Key of the 'configureDevice' slot
    VIDEOQT_API static const ::fwCom::Slots::SlotKeyType s_CONFIGURE_DEVICE_SLOT;
    /// Key of the 'configureFile' slot
    VIDEOQT_API static const ::fwCom::Slots::SlotKeyType s_CONFIGURE_FILE_SLOT;
    /// Key of the 'configureStream' slot
    VIDEOQT_API static const ::fwCom::Slots::SlotKeyType s_CONFIGURE_STREAM_SLOT;

protected:

    /// Configure the service
    virtual void configuring() override;

    /// Installs the layout
    virtual void starting() override;

    /// Destroys the layout
    virtual void stopping() override;

    /// Does nothing
    virtual void updating() override;

    /// Does nothing
    virtual void swapping() override;

protected Q_SLOTS:

    /// Calls when user select another device
    void onApply(int index);

private:

    void onChooseFile();
    void onChooseStream();
    void onChooseDevice();

    /// Retrieve camera objects according to the XML configuration
    std::vector< ::arData::Camera::sptr > getCameras() const;

    /// Combobox for camera selection
    QPointer<QComboBox> m_devicesComboBox;

    /// Do we offer the possibility to select a video file (no by default) ?
    bool m_bVideoSupport;

    /// Number of cameras to create when using a camera series as input
    size_t m_numCreateCameras;

    /// Signal emitted when the cameraSeries has been configured
    ConfiguredCamerasSignalType::sptr m_sigConfiguredCameras;

    using ConfigureDeviceSlotType = ::fwCom::Slot<void()>;
    using ConfigureFileSlotType   = ::fwCom::Slot<void()>;
    using ConfigureStreamSlotType = ::fwCom::Slot<void()>;
};

} // editor
} // videoQt

#endif /*__VIDEOQT_EDITOR_SCAMERA_HPP__*/

