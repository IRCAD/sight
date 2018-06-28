
#pragma once

#include "videoOrbbec/config.hpp"

#include <arData/Camera.hpp>

#include <arServices/IRGBDGrabber.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <OpenNI.h>
#include <QtGlobal>

#include <memory>

namespace videoOrbbec
{

class ColorFrameWorker;

/**
 * @brief Base class for Orbbec Astra Pro grabbers.
 *
 * @section Signals Signals
 * - \b positionModified(std::int64_t): Unimplemented.
 * - \b durationModified(std::int64_t): Unimplemented.
 *
 * @section Slots Slots
 * - \b startCamera(): Start capturing frames from the camera.
 * - \b stopCamera(): Stop capturing frames from the camera.
 * - \b pauseCamera(): Unimplemented.
 * - \b toggleLoopMode(): Unimplemented.
 * - \b setPosition(int): Unimplemented.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="videoGrabber" type="...">
            <videoMode>720p</videoMode>
        </service>
 * @endcode
 *
 * @subsection Configuration Configuration
 * - \b videoMode (optional) (default: `VGA`): Video mode selection. Accepted values are `720p` (720p@10) and `VGA`
 *(640x480@30)
 */
class VIDEOORBBEC_CLASS_API SScanBase : public ::arServices::IRGBDGrabber
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro((SScanBase)(::arServices::IRGBDGrabber))

    /// Base name of the camera as detected by UVC APIs.
    static const std::string s_QT_CAMERA_STRING;

    /**
     * @brief SScanBase constructor.
     */
    VIDEOORBBEC_API SScanBase() noexcept;

    /**
     * @brief SScanBase destructor.
     */
    VIDEOORBBEC_API ~SScanBase() noexcept;

    /// OpenNI2 device pointer type with unique ownership.
    using OpenniDevicePtr = std::unique_ptr< ::openni::Device>;

protected:
    /**
     * @brief Configures the service.
     *
     * @throw fwTools::Failed
     */
    VIDEOORBBEC_API void configuring() override;

    /**
     * @brief Stops the service
     *
     * @throw ::fwTools::Failed
     */
    VIDEOORBBEC_API void stopping() override;

    /**
     * @brief Does nothing
     *
     * @throw ::fwTools::Failed
     */
    VIDEOORBBEC_API void updating() override;

    /**
     * @brief Not implemented
     */
    VIDEOORBBEC_API void pauseCamera() override;

    /**
     * @brief Not implemented
     */
    VIDEOORBBEC_API void toggleLoopMode() override;

    /**
     * @brief Not implemented
     */
    VIDEOORBBEC_API void setPosition(int64_t position) override;

    /**
     * @brief Enumerate connected OpenNI devices and search for the Orbbec Astra Pro.
     */
    VIDEOORBBEC_API void detectCameraOpenni();

    /**
     * @brief Enumerate connected UVC devices and search for the Orbbec Astra Pro.
     *
     * @return ::arData::Camera object representing the Orbbec Astra Pro RGB sensor.
     */
    VIDEOORBBEC_API ::arData::Camera::sptr detectCameraQt();

    /**
     * @brief Qt message handler
     */
    VIDEOORBBEC_API static void qtMessageHandler(QtMsgType, QMessageLogContext const&, QString const&);

    /**
     * @brief Display a Qt error popup displaying the error string passed as argument.
     *
     * @param msg
     */
    VIDEOORBBEC_API static void errorPopup(std::string const& msg);

    /// OpenNI device representing the Orbbec Astra Pro.
    OpenniDevicePtr m_oniDevice;

    // Video mode settings
    bool m_720p;
};

} // videoOrbbec