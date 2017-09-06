/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __REGISTRATIONCV_SPOSEFROM2D_HPP__
#define __REGISTRATIONCV_SPOSEFROM2D_HPP__

#include "registrationCV/config.hpp"

#include <arServices/IRegisterer.hpp>

#include <fwCore/HiResClock.hpp>
#include <fwCore/mt/types.hpp>

#include <fwServices/macros.hpp>

#include <opencv2/core.hpp>

namespace registrationCV
{

/**
 * @brief   SPoseFrom2d Class used to compute the 3d pose of a object using 2d points.
 *
 * @section Slots Slots
 * - \b computeRegistration(::fwCore::HiResClock::HiResClockType timestamp) : computes the registration.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service uid="..." type="::registrationCV::SPoseFrom2d">
         <in group="markerTL" autoConnect="yes">
             <key uid="markerTL1" />
             <key uid="markerTL2" />
         </in>
         <in group="camera">
             <key uid="cam1" />
             <key uid="cam2" />
         </in>
         <in key="extrinsic" uid="matrix1" />
         <inout key="matrixTL" uid="matrixTL1" />
         <patternWidth>80</patternWidth>
     </service>
   @endcode
 * @subsection Input Input
 * - \b markerTL [::arData::MarkerTL]: timeline for markers.
 * - \b camera [::arData::Camera]: calibrated cameras.
 * - \b extrinsic [::fwData::TransformationMatrix3D]: extrinsic matrix, only used if you have two cameras configured.
 * @subsection In-Out In-Out
 * - \b matrixTL [::arData::MatrixTL]: timeline of 3D transformation matrices.
 * @subsection Configuration Configuration
 * - \b patternWidth : width of the tag.
 */
class REGISTRATIONCV_CLASS_API SPoseFrom2d : public ::arServices::IRegisterer
{
public:
    fwCoreServiceClassDefinitionsMacro((SPoseFrom2d)(::arServices::IRegisterer));

    typedef std::vector<std::string> VectKeyType;

    /**
     * @brief Constructor.
     */
    REGISTRATIONCV_API SPoseFrom2d() noexcept;

    /**
     * @brief Destructor.
     */
    REGISTRATIONCV_API virtual ~SPoseFrom2d() noexcept;

    /// Connect MarkerTL::s_OBJECT_PUSHED_SIG to s_REGISTER_SLOT
    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;

protected:
    /**
     * @brief Configuring method : This method is used to configure the service.
     */
    REGISTRATIONCV_API void configuring();

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    REGISTRATIONCV_API void starting();

    /**
     * @brief Updating method : This method is used to update the service.
     */
    REGISTRATIONCV_API void updating();

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    REGISTRATIONCV_API void stopping();

    /// Register matrix slot
    void computeRegistration(::fwCore::HiResClock::HiResClockType timestamp);

private:

    /**
     * @brief The Marker struct:
     * to handle marker object, with 4 corners 2D, and optionnaly a centroïd
     */
    struct Marker
    {
        std::vector< ::cv::Point2f > corners2D;
        ::cv::Point3f centroid;
    };

    /**
     * @brief The Camera struct : to handle intrinsic parameters and distorsion coeficient.
     */
    struct Camera
    {
        ::cv::Mat intrinsicMat;
        ::cv::Mat distCoef;
    };
    /**
     * @brief The Extrinsic struct : to handle several format of extrinsic matrix
     */
    struct Extrinsic
    {
        ::cv::Mat rotation;
        ::cv::Mat translation;
        ::cv::Mat Matrix4x4;
    };

    /// Initialize cameras
    void initialize();

    /**
     * @brief : Compute the camera position from a maker detected on two separated view
     * @param : Marker points in each view
     *
     **/
    const ::cv::Matx44f cameraPoseFromStereo(const Marker& _markerCam1, const Marker& _markerCam2) const;

    /**
     * @brief :Compute the camera position from a marker detected in one view
     **/
    const ::cv::Matx44f cameraPoseFromMono(const Marker& _markerCam1) const;

    /// Last timestamp
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;

    /// Marker pattern width.
    double m_patternWidth;

    /// True if the service is initialized (timelines and cameras)
    bool m_isInitialized;

    /// 3d model
    std::vector< ::cv::Point3f > m_3dModel;

    ///std::vector of cameras
    std::vector < Camera > m_cameras;

    ///Extrinsic matrix
    Extrinsic m_extrinsic;

};

} // namespace registrationCV

#endif /* __REGISTRATIONCV_SPOSEFROM2D_HPP__ */
