/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "videoCalibration/config.hpp"

#include <arData/MarkerMap.hpp>

#include <fwServices/IController.hpp>

#include <opencv2/core.hpp>

namespace videoCalibration
{
/**
 * @brief   SReprojectionError Class used to compute the mean error of reprojection between 3d object points
 * and 2d image points.
 *
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service uid="..." type="::videoCalibration::SReprojectionError">
        <in key="matrixTL" uid="..."/>
        <in key="markerTL" uid="..."/>
        <in key="camera" uid="..."/>
        <in key="extrinsic" uid="..."/>
        <inout key="frameTL" uid="..." />
        <out key="error" uid="..." />
        <patternWidth>80</patternWidth>
     </service>

     or

     <service uid="..." type="::registrationCV::SReprojectionError">
         <in group="matrix">
             <key uid="..." />
             <key uid="..." />
             <key uid="..." />
             <key uid="..." />
         </in>
         <in key="markerMap" uid="..." />
         <in key="camera" uid="..."/>
         <in key="extrinsic" uid="..." />
         <inout key="frame" uid="..." />
         <patternWidth>80</patternWidth>
     </service>
   @endcode
 * @subsection Input Input
 * - \b markerTL [::arData::MarkerTL]: timeline for markers.
 * - \b markerMap [::arData::MarkerMap]: list of markers maps where the tags.
 * - \b camera [::arData::Camera]: calibrated cameras.
 * - \b extrinsic [::fwData::TransformationMatrix3D]: extrinsic matrix, only used if you have two cameras configured.
 * - \b matrixTL [::arData::MatrixTL]: timeline of 3D transformation matrices.
 * - \b matrix [::fwData::TransformationMatrix3D]: list of matrices, one for each marker. You must also specify what is
 * the tag \b id in the markerMap you want to be associated with each matrix.
 * @subsection InOut InOut
 * - \b frameTL [::arData::FrameTL] : frame timeline used to draw reprojected points (optional)
 * - \b frame [::fwData::Image]: video frame.
 * @subsection Output Output
 * - \b error [::fwData::Float] : computed error
 * @subsection Configuration Configuration
 * - \b patternWidth : width of the tag.
 */
class VIDEOCALIBRATION_CLASS_API SReprojectionError : public ::fwServices::IController
{
public:
    fwCoreServiceClassDefinitionsMacro((SReprojectionError)(fwServices::IController))

    /// Double changed signal type
    typedef ::fwCom::Signal< void (double) > ErrorComputedSignalType;

    VIDEOCALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_COMPUTE_SLOT;

    VIDEOCALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT;
    VIDEOCALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_SET_COLOR_PARAMETER_SLOT;

    ///Constructor
    VIDEOCALIBRATION_API SReprojectionError();

    ///Destructor
    VIDEOCALIBRATION_API ~SReprojectionError();

    /// Connect MatrixTL::s_OBJECT_PUSHED_SIG to s_COMPUTE_SLOT
    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

protected:
    /**
     * @brief Configuring method : This method is used to configure the service.
     */
    VIDEOCALIBRATION_API void configuring() override;

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    VIDEOCALIBRATION_API void starting() override;

    /**
     * @brief Updating method : This method is used to restart the service.
     */
    VIDEOCALIBRATION_API void updating() override;

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    VIDEOCALIBRATION_API void stopping() override;

private:

    void compute(::fwCore::HiResClock::HiResClockType timestamp);

    ///Slot called when a color value is changed
    void setColorParameter(std::array< std::uint8_t, 4 > _val, std::string _key);
    ///Slot called when a boolean value is changed
    void setBoolParameter(bool _val, std::string _key);

    /// Last timestamp
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;

    /// Marker pattern width.
    double m_patternWidth;

    /// 3D object points
    std::vector< ::cv::Point3f > m_objectPoints;
    /// Camera Matrix (fx, fy, cx, cy)
    ::cv::Mat m_cameraMatrix;
    ///Distorsion coefficient
    ::cv::Mat m_distorsionCoef;
    /// Color of the reprojection circle
    ::cv::Scalar m_cvColor;
    /// if true: display circle centered at reprojection point.
    bool m_display;
    /// extrinsic matrix (can be identity)
    ::cv::Mat m_extrinsic;

    /// List of tags associated with each input matrix
    std::vector< ::arData::MarkerMap::KeyType> m_matricesTag;
};

}//namespace videoCalibration
