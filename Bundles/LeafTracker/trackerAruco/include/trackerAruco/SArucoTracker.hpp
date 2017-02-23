/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __TRACKERARUCO_SARUCOTRACKER_HPP__
#define __TRACKERARUCO_SARUCOTRACKER_HPP__

#include "trackerAruco/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwCore/HiResClock.hpp>

#include <fwData/Composite.hpp>

#include <fwServices/macros.hpp>

#include <tracker/ITracker.hpp>

#include <aruco/markerdetector.h>

namespace trackerAruco
{

/**
 * @brief   Class used to track multiple tags with ArUco.
 *
 * @see ::tracker::ITracker
 *
 * @section Signals Signals
 * - \b detectionDone(::fwCore::HiResClock::HiResClockType) : This signal is emitted when the tracker find tags.
 *
 * @section Slots Slots
 * @subsection Inherited Inherited slots (from ITracker)
 * - \b track(timestamp) : Slot to fills the timeline with the new positions of the grid
 * - \b startTracking() : Slot called when the user wants to start tracking
 * - \b stopTracking() : Slot called when the user wants to stop tracking
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::trackerAruco::SArucoTracker" >
            <in key="timeline" uid="frameTLUid" autoConnect="yes"/>
            <in key="camera" uid="cameraUid" />
            <inout group="tagTL" >
                <key uid="WireTimeline" />
                <key uid="PatientTimeline" />
                <key uid="TableTimeline" />
            </inout>
            <config>
                <track>
                    <markers id="42,1,100,54" />
                    <markers id="32,10" />
                    <markers id="52,45" />
                </track>
                <threshold>
                    <method>ADPT_THRES</method>
                    <blockSize>7</blockSize>
                    <constant>7</constant>
                </threshold>
                <patternWidth>106</patternWidth>
                <debugMarkers>yes</debugMarkers>
            </config>
            <dropObj>true</dropObj>
        </service>
   @endcode
 * @subsection Input Input
 * - \b timeline [::arData::FrameTL]: camera used to display video. It is the main timeline used for the tracking.
 * - \b camera [::arData::Camera]: camera calibration.
 *
 * @subsection In-Out In-Out
 * - \b tagTL [::arData::MarkerTL]: list of markers timelines where to extract the tags. The number of tagTL inout keys
 * must match the number of \b markers entries in the config below.
 *
 * @subsection Configuration Configuration
 *  - \b track (mandatory)
 *      - \b markers (mandatory) : list of the tracked markers.
 *           - \b id (mandatory) : ids of the markers to detect.
 *  - \b threshold (optional)
 *      - \b method (mandatory): can be ADPT_THRES, FIXED_THRES or CANNY.
 *      - \b blockSize : parameter of the chosen method
 *      - \b constant : parameter of the chosen method
 *  - \b patternWidth (optional): width of the pattern(s).
 *  - \b debugMarkers : if value is yes markers debugging mode is activated.
 *  - \b dropObj : defines if the tracker should drop few objects from the timeline (and always get the last one) or
 *  not.
 */
class TRACKERARUCO_CLASS_API SArucoTracker : public ::tracker::ITracker
{
public:

    fwCoreServiceClassDefinitionsMacro((SArucoTracker)(tracker::ITracker));

    typedef ::fwCom::Signal< void (::fwCore::HiResClock::HiResClockType timestamp) > DetectionDoneSignalType;
    typedef ::fwCom::Signal< void (bool) > MarkerDetectedSignalType;

    /**
     * @name Signal API
     * @{
     */
    /// Key in m_signals map of signal m_sigDetectionDone
    TRACKERARUCO_API static const ::fwCom::Signals::SignalKeyType s_DETECTION_DONE_SIG;
    /// Signal always emitted with boolean true if a least a maker from id list is found, false otherwise.
    TRACKERARUCO_API static const ::fwCom::Signals::SignalKeyType s_MARKER_DETECTED_SIG;
    /** @} */
    /**
     * @name Slots API
     * @{
     */
    TRACKERARUCO_API static const ::fwCom::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT;
    TRACKERARUCO_API static const ::fwCom::Slots::SlotKeyType s_SET_INT_PARAMETER_SLOT;
    TRACKERARUCO_API static const ::fwCom::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT;
    TRACKERARUCO_API static const ::fwCom::Slots::SlotKeyType s_SET_ENUM_PARAMETER_SLOT;
    /** @} */

    typedef std::vector< int >          MarkerIDType;
    typedef std::vector< MarkerIDType > MarkerIDVectorType;

    /**
     * @brief Constructor.
     */
    TRACKERARUCO_API SArucoTracker() throw ();

    /**
     * @brief Destructor.
     */
    TRACKERARUCO_API virtual ~SArucoTracker() throw ();

protected:
    /**
     * @brief Configuring method : This method is used to configure the service.
     */
    TRACKERARUCO_API void configuring() throw (fwTools::Failed);

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    TRACKERARUCO_API void starting() throw (fwTools::Failed);

    /**
     * @brief Updating method : This method is used to update the service.
     */
    TRACKERARUCO_API void updating() throw (fwTools::Failed);

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    TRACKERARUCO_API void stopping() throw (fwTools::Failed);

    /// Detect marker
    TRACKERARUCO_API virtual void tracking(::fwCore::HiResClock::HiResClockType& timestamp);

private:

    ///Slot called when a integer value is changed
    void setIntParameter(int _val, std::string _key);
    ///Slot called when a double value is changed
    void setDoubleParameter(double _val, std::string _key);
    ///Slot called when a boolean value is changed
    void setBoolParameter(bool _val, std::string _key);
    ///Slot called when a enum value is changed
    void setEnumParameter(std::string _val, std::string _key);

    /// Marker vector [[0,1,2],[4,5,6]]
    MarkerIDVectorType m_markers;
    /// Marker detector
    ::aruco::MarkerDetector* m_arUcoTracker;
    /// arUCO camera parameters
    ::aruco::CameraParameters* m_camParameters;
    /// Threshold, value must be a double or "auto"
    std::string m_threshold;
    /// Marker border width.
    double m_borderWidth;
    /// Marker pattern width.
    double m_patternWidth;

    /// True if tracker is initialized
    bool m_isInitialized;

    /// BlockSize of the pixel neighborhood that is used to calculate a threshold value for the pixel
    double m_blockSize;

    /// The constant subtracted from the mean or weighted mean
    double m_constant;

    /// Display markers in the image or not
    bool m_debugMarkers;

    /// Speed of detection
    unsigned int m_speed;

    /// The threshold method
    ::aruco::MarkerDetector::ThresholdMethods m_thresholdMethod;
    ::aruco::MarkerDetector::CornerRefinementMethod m_cornerRefinement;

    /// Signal to emit when
    DetectionDoneSignalType::sptr m_sigDetectionDone;
};

} // namespace trackerAruco

#endif /* __TRACKERARUCO_SARUCOTRACKER_HPP__ */
