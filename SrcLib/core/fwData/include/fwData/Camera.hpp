/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_CAMERA_HPP__
#define __FWDATA_CAMERA_HPP__

#include "fwData/Object.hpp"
#include "fwData/TransformationMatrix3D.hpp"
#include "fwData/factory/new.hpp"

#include <boost/cstdint.hpp>
#include <boost/array.hpp>

#include <utility>

fwCampAutoDeclareDataMacro((fwData)(Camera), FWDATA_API);

namespace fwData
{
/**
 * @brief This class defines a camera object.
 */
class FWDATA_CLASS_API Camera : public Object
{
public:
    fwCampMakeFriendDataMacro((fwData)(Camera));
    fwCoreClassDefinitionsWithFactoryMacro((Camera)(::fwData::Object),
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
    void setExtrinsicCameraTransformation(const ::fwData::TransformationMatrix3D::sptr& transMat);

    /// @brief set the intrinsic transformation matrix
    void setIntrinsicCameraTransformation(const ::fwData::TransformationMatrix3D::sptr& transMat);

    /// @brief get the extrinsic transformation matrix
    ::fwData::TransformationMatrix3D::sptr getExtrinsicCameraTransformation() const;

    /// @brief get the intrinsic transformation matrix
    ::fwData::TransformationMatrix3D::sptr getIntrinsicCameraTransformation() const;

    /// @return cx or u0 = x-coordinate of optical center
    ::fwData::TransformationMatrix3D::TM3DType getCx() const;

    /// @return cy or v0 = y-coordinate of optical center
    ::fwData::TransformationMatrix3D::TM3DType getCy() const;

    /// @brief set cx or u0 = x-coordinate of optical center
    void setCx(::fwData::TransformationMatrix3D::TM3DType cx);

    /// @brief set cy or v0 = y-coordinate of optical center
    void setCy(::fwData::TransformationMatrix3D::TM3DType cy);

    /// @return fx = x-coordinate of optical center
    ::fwData::TransformationMatrix3D::TM3DType getFx() const;

    /// @brief set fx = x-coordinate of optical center
    void setFx(::fwData::TransformationMatrix3D::TM3DType fx);

    /// @return fy = y-coordinate of optical center
    ::fwData::TransformationMatrix3D::TM3DType getFy() const;

    /// @brief set fy = y-coordinate of optical center
    void setFy(::fwData::TransformationMatrix3D::TM3DType fy);

    FWDATA_API void setDistortionCoefficient( double k1, double k2, double p1, double p2, double k3);
    const DistArray &getDistortionCoefficient () const;
    void setDistortionCoefficient (const DistArray &val);

    double getSkew() const;
    void setSkew(double val);

    /// @brief get flag if data is available
    bool &dataAvailable()
    {
        return m_dataAvailable;
    }

    /**
     * @brief get universal ID for the physic's camera
     */
    std::pair< bool, uint32HL > getCameraGUID() const;

    /**
     * @brief set universal ID for the physic's camera
     */
    FWDATA_API void setCameraGUID(uint32HL _cameraGUID);

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

protected:

    /// extrinsic matrix
    ::fwData::TransformationMatrix3D::sptr m_extrinsicMatrix;

    /// intrinsic matrix
    ::fwData::TransformationMatrix3D::sptr m_intrinsicMatrix;

    /**
     * Image distortion coefficients (radial and tangential distortions,
     * [k1, k2, p1, p2, k3])
     */
    DistArray m_distortionCoefficient;

    /// skew coefficient (angle between the x and y pixel axes)
    double m_skew;

    //! Universal ID for the physic's camera
    std::pair< bool, uint32HL > m_cameraGUID;

    //! flag if data is available
    bool m_dataAvailable;
};

//-----------------------------------------------------------------------------

inline void Camera::setExtrinsicCameraTransformation(const ::fwData::TransformationMatrix3D::sptr& transMat)
{
    m_extrinsicMatrix = transMat;
}

//-----------------------------------------------------------------------------

inline void Camera::setIntrinsicCameraTransformation(const ::fwData::TransformationMatrix3D::sptr& transMat)
{
    m_intrinsicMatrix = transMat;
}

//-----------------------------------------------------------------------------

inline ::fwData::TransformationMatrix3D::sptr Camera::getExtrinsicCameraTransformation() const
{
    return m_extrinsicMatrix;
}

//-----------------------------------------------------------------------------

inline ::fwData::TransformationMatrix3D::sptr Camera::getIntrinsicCameraTransformation() const
{
    return m_intrinsicMatrix;
}

//-----------------------------------------------------------------------------

inline ::fwData::TransformationMatrix3D::TM3DType Camera::getCx() const
{
    return m_intrinsicMatrix->getCoefficient(0, 2);
}

//-----------------------------------------------------------------------------

inline ::fwData::TransformationMatrix3D::TM3DType Camera::getCy() const
{
    return m_intrinsicMatrix->getCoefficient(1, 2);
}

//-----------------------------------------------------------------------------

inline void Camera::setCx(::fwData::TransformationMatrix3D::TM3DType cx)
{
    m_intrinsicMatrix->setCoefficient(0, 2, cx);
}

//-----------------------------------------------------------------------------

inline void Camera::setCy(::fwData::TransformationMatrix3D::TM3DType cy)
{
    m_intrinsicMatrix->setCoefficient(1, 2, cy);
}

//-----------------------------------------------------------------------------

inline ::fwData::TransformationMatrix3D::TM3DType Camera::getFx() const
{
    return m_intrinsicMatrix->getCoefficient(0, 0);
}

//-----------------------------------------------------------------------------

inline void Camera::setFx(::fwData::TransformationMatrix3D::TM3DType fx)
{
    m_intrinsicMatrix->setCoefficient(0, 0, fx);
}

//-----------------------------------------------------------------------------

inline ::fwData::TransformationMatrix3D::TM3DType Camera::getFy() const
{
    return m_intrinsicMatrix->getCoefficient(1, 1);
}

//-----------------------------------------------------------------------------

inline void Camera::setFy(::fwData::TransformationMatrix3D::TM3DType fy)
{
    m_intrinsicMatrix->setCoefficient(1, 1, fy);
}

//-----------------------------------------------------------------------------

inline const Camera::DistArray &Camera::getDistortionCoefficient () const
{                                                                                                                                                                                        \
    return m_distortionCoefficient;
}

//-----------------------------------------------------------------------------

inline void Camera::setDistortionCoefficient (const Camera::DistArray &val)
{
    m_distortionCoefficient = val;
}

//-----------------------------------------------------------------------------

inline double Camera::getSkew() const
{
    return m_skew;
}

//-----------------------------------------------------------------------------

inline void Camera::setSkew(double val)
{
    m_skew = val;
}

//-----------------------------------------------------------------------------

inline std::pair< bool, Camera::uint32HL > Camera::getCameraGUID() const
{
    return m_cameraGUID;
}

//-----------------------------------------------------------------------------

} // namespace fwData

#endif // __FWDATA_CAMERA_HPP__

