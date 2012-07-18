/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_CAMERA_HPP_
#define _FWDATA_CAMERA_HPP_

#include <utility>

#include <boost/cstdint.hpp>
#include <boost/array.hpp>

#include "fwData/Object.hpp"
#include "fwData/TransformationMatrix3D.hpp"
#include "fwData/factory/new.hpp"

namespace fwData
{
/**
 * @class   Camera
 * @brief   This class defines a camera object.
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API Camera : public Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro(
            (Camera)(::fwData::Object),
            (()),
            ::fwData::factory::New< Camera >);

    typedef struct
    {
        unsigned long Low;
        unsigned long High;
    } uint32HL;

    typedef ::boost::array<double, 5> DistArray;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Camera( ::fwData::Object::Key key );

    /**
     * @brief destructor
     */
    FWDATA_API virtual ~Camera() throw();

    /// @brief set the extrinsic transformation matrix
    void setExtrinsicCameraTransformation(
            ::fwData::TransformationMatrix3D::sptr transMat)
    { m_extrinsicMatrix = transMat; }

    /// @brief set the intrinsic transformation matrix
    void setIntrinsicCameraTransformation(
            ::fwData::TransformationMatrix3D::sptr transMat)
    { m_intrinsicMatrix = transMat; }

    /// @brief get the extrinsic transformation matrix
    ::fwData::TransformationMatrix3D::sptr
     getExtrinsicCameraTransformation() const
    { return m_extrinsicMatrix; };

    /// @brief get the intrinsic transformation matrix
    ::fwData::TransformationMatrix3D::sptr
     getIntrinsicCameraTransformation() const
    { return m_intrinsicMatrix; };

    /// @return cx or u0 = x-coordinate of optical center
    ::fwData::TransformationMatrix3D::TM3DType getCx() const
    { return m_intrinsicMatrix->getCoefficient(0, 2); };

    /// @return cy or v0 = y-coordinate of optical center
    ::fwData::TransformationMatrix3D::TM3DType getCy() const
    { return m_intrinsicMatrix->getCoefficient(1, 2); };

    /// @brief set cx or u0 = x-coordinate of optical center
    void setCx(::fwData::TransformationMatrix3D::TM3DType cx)
    { m_intrinsicMatrix->setCoefficient(0, 2, cx); };

    /// @brief set cy or v0 = y-coordinate of optical center
    void setCy(::fwData::TransformationMatrix3D::TM3DType cy)
    { m_intrinsicMatrix->setCoefficient(1, 2, cy); };

    /// @return fx = x-coordinate of optical center
    ::fwData::TransformationMatrix3D::TM3DType getFx() const
    { return m_intrinsicMatrix->getCoefficient(0, 0); };

    /// @brief set fx = x-coordinate of optical center
    void setFx(::fwData::TransformationMatrix3D::TM3DType fx)
    { m_intrinsicMatrix->setCoefficient(0, 0, fx); };

    /// @return fy = y-coordinate of optical center
    ::fwData::TransformationMatrix3D::TM3DType getFy() const
    { return m_intrinsicMatrix->getCoefficient(1, 1); };

    /// @brief set fy = y-coordinate of optical center
    void setFy(::fwData::TransformationMatrix3D::TM3DType fy)
    { m_intrinsicMatrix->setCoefficient(1, 1, fy); };

    void setDistortionCoefficient(
            double k1, double k2, double p1, double p2, double k3);
    fwDataGetSetCRefMacro(DistortionCoefficient, DistArray);

    fwDataGetSetMacro(Skew, double);

    /// @brief get flag if data is available
    bool &dataAvailable()
    { return m_dataAvailable; }

    /**
     * @brief get universal ID for the physic's camera
     */
    std::pair< bool, uint32HL > getCameraGUID() const
                { return m_cameraGUID; };

    /**
     * @brief set universal ID for the physic's camera
     */
    FWDATA_API void setCameraGUID(uint32HL _cameraGUID);

protected:

    /// extrinsic matrix
    ::fwData::TransformationMatrix3D::sptr m_extrinsicMatrix;

    /// intrinsic matrix
    ::fwData::TransformationMatrix3D::sptr m_intrinsicMatrix;

    /**
     * Image distortion coefficients (radial and tangential distortions,
     * [k1, k2, p1, p2, k3])
     */
    DistArray m_attrDistortionCoefficient;

    /// skew coefficient (angle between the x and y pixel axes)
    double m_attrSkew;

    //! Universal ID for the physic's camera
    std::pair< bool, uint32HL > m_cameraGUID;

    //! flag if data is available
    bool m_dataAvailable;
};

} // namespace fwData

#endif // _FWDATA_CAMERA_HPP_
