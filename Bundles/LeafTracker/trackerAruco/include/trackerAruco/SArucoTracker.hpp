/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __TRACKERARUCO_SARUCOTRACKER_HPP__
#define __TRACKERARUCO_SARUCOTRACKER_HPP__

#include "trackerAruco/config.hpp"

#include <tracker/ITracker.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwData/Composite.hpp>

#include <fwCore/HiResClock.hpp>

#include <fwServices/Base.hpp>

#include <aruco/markerdetector.h>

namespace trackerAruco
{

/**
 * @class   SArucoTracker
 * @brief   Class used to track multiple tags with ArUco.
 */

class TRACKERARUCO_CLASS_API SArucoTracker : public ::tracker::ITracker
{
public:

    fwCoreServiceClassDefinitionsMacro((SArucoTracker)(tracker::ITracker));

    typedef ::fwCom::Signal< void (::fwCore::HiResClock::HiResClockType timestamp) > DetectionDoneSignalType;

    /// Key in m_signals map of signal m_sigDetectionDone
    TRACKERARUCO_API static const ::fwCom::Signals::SignalKeyType s_DETECTION_DONE_SIG;

    TRACKERARUCO_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_METHOD_SLOT;

    TRACKERARUCO_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_BLOCKSIZE_SLOT;

    TRACKERARUCO_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_CONSTANT_SLOT;

    TRACKERARUCO_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_BORDERWIDTH_SLOT;

    TRACKERARUCO_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_PATTERNWIDTH_SLOT;

    TRACKERARUCO_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_CORNERREFINEMENT_SLOT;

    TRACKERARUCO_API static const ::fwCom::Slots::SlotKeyType s_DISPLAY_TAGS_SLOT;

    TRACKERARUCO_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_SPEED_SLOT;

    TRACKERARUCO_API static const ::fwCom::Slots::SlotKeyType s_DETECT_MARKER_SLOT;

    typedef ::fwCom::Slot<void (::fwCore::HiResClock::HiResClockType)> DetectMarkerSlotType;

    typedef ::fwCom::Slot< void (unsigned int) >       ChangeMethodSlotType;
    typedef ::fwCom::Slot< void (double) >             ChangeBlockSizeSlotType;
    typedef ::fwCom::Slot< void (double) >             ChangeConstantSlotType;
    typedef ::fwCom::Slot< void (double) >             ChangeBorderWidthSlotType;
    typedef ::fwCom::Slot< void (double) >             ChangePatternWidthSlotType;
    typedef ::fwCom::Slot< void (unsigned int) >       ChangeCornerRefinementSlotType;
    typedef ::fwCom::Slot< void (unsigned int) >       ChangeSpeedSlotType;
    typedef ::fwCom::Slot< void (bool) >               DisplayTagsSlotType;



    typedef std::map<int, std::string> MarkerMapType;

    typedef std::pair< std::string, std::vector< int > >   MarkerPairType;
    typedef std::vector< MarkerPairType >                  TimelineVectorType;

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
     *
     * The method verifies if the configuration is well written and retrieves user parameter values.
     *
     * @verbatim
        <service uid="..." impl="::trackerAruco::SArucoTracker" autoConnect="no">
            <config>
                <timelineVideo>timelineA</timelineVideo>
                <camera>camera1</camera>
                <track>
                    <markers id="42,1,100,54" timeline="WireTimeline" />
                    <markers id="32,10" timeline="PatientTimeline" />
                    <markers id="52,45" timeline="TableTimeline" />
                </track>
                <threshold>
                    <method>ADPT_THRES</method>
                    <blockSize>7</blockSize>
                    <constant>7</constant>
                </threshold>
                <patternWidth>106</patternWidth>
                <debugMarkers>yes</debugMarkers>
            </config>
        </service>
       @endverbatim
     * With :
     * None of these parameters are mandatory.
     *  - \b timelineVideo (mandatory) : input data.
     *  - \b camera (mandatory) camera key
     *  - \b track (mandatory)
     *      - \b markers (mandatory) : list of the tracked markers and its associated timeline.
     *           - \b id (mandatory) : ids of the markers to detect.
     *           - \b timeline (mandatory) : timeline to store result.
     *  - \b threshold (optional)
     *      - \b method (mandatory): can be ADPT_THRES, FIXED_THRES or CANNY.
     *      - \b blockSize : parameter of the chosen method
     *      - \b constant : parameter of the chosen method
     *  - \b patternWidth (optional): width of the pattern(s).
     *  - \b debugMarkers : if value is yes markers debugging mode is activated.
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

    /// Detect marker slot
    void detectMarker(::fwCore::HiResClock::HiResClockType timestamp);

    /// Threshold method slot
    void setMethod(unsigned int);
    /// 'Constant' parameters slot
    void setConstant(double);
    /// Block size slot
    void setBlockSize(double);
    /// BorderWidth slot (not used)
    void setBorderWidth(double);
    /// PatternWidth slot (not used)
    void setPatternWidth(double);
    /// Method for corner refinement slot
    void setCornerRefinement(unsigned int);
    /// Method that change speed of detection (the lower the better but also the slower)
    void setSpeed(unsigned int);
    /// Method that display tags
    void displayTags(bool);

private:

    /// Video timeline key
    std::string m_timelineVideoKey;

    /// Marker map ['id':'timeline_key']
    MarkerMapType m_markers;

    /// Video camera key
    std::string m_cameraKey;

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

    /// Last timestamp
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;

    /// True if tracker is initialized
    bool m_isInitialized;

    /// Slots used when the frame have been refreshed
    DetectMarkerSlotType::sptr m_slotDetectMarker;

    /// Connections
    ::fwServices::helper::SigSlotConnection::sptr m_connections;

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

    /// The timeline vector. containing a MarkerPairType object;
    TimelineVectorType m_timelineVector;

    /// Signal to emit when
    DetectionDoneSignalType::sptr m_sigDetectionDone;

    ///Slot called when threshold method changed
    ChangeMethodSlotType::sptr m_slotChangeMethod;
    ///Slot called when block size changed
    ChangeBlockSizeSlotType::sptr m_slotChangeBlockSize;
    ///Slot called when constant parameters changed
    ChangeConstantSlotType::sptr m_slotChangeConstant;
    ///Slot called when border width changed
    ChangeBorderWidthSlotType::sptr m_slotChangeBorderWidth;
    ///Slot called when pattern width changed
    ChangePatternWidthSlotType::sptr m_slotChangePatternWidth;
    ///Slot called when method for corner refinemement changed
    ChangeCornerRefinementSlotType::sptr m_slotChangeCornerRefinement;
    ///Slot called when speed of detection changed
    ChangeSpeedSlotType::sptr m_slotChangeSpeed;
    ///Slot called when debug mode is enabled/disabled
    DisplayTagsSlotType::sptr m_slotDisplayTags;


};

} //namespace trackerAruco

#endif /* __TRACKERARUCO_SARUCOTRACKER_HPP__ */
