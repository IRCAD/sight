/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_CAMERA_HPP_
#define _FWDATA_CAMERA_HPP_

#include <utility>

#include <boost/cstdint.hpp>

#include "fwData/Object.hpp"
#include "fwData/TransformationMatrix3D.hpp"

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
    fwCoreClassDefinitionsWithFactoryMacro( (Camera)(::fwData::Object), (()), ::fwTools::Factory::New< Camera >) ;
    typedef struct{ unsigned long Low; unsigned long High; }uint32HL;

    /// @brief set the extrinsic transformation matrix
    FWDATA_API void setExtrinsicCameraTransformation(::fwData::TransformationMatrix3D::sptr transMat);

    /// @brief set the intrinsic transformation matrix
    FWDATA_API void setIntrinsicCameraTransformation(::fwData::TransformationMatrix3D::sptr transMat);

    /// @brief get the extrinsic transformation matrix
    FWDATA_API ::fwData::TransformationMatrix3D::sptr getExtrinsicCameraTransformation()const { return m_extrinsicMatrix; };

    /// @brief get the intrinsic transformation matrix
    FWDATA_API ::fwData::TransformationMatrix3D::sptr getIntrinsicCameraTransformation()const { return m_intrinsicMatrix; };

    /// @return cx or u0 = x-coordinate of optical center
    FWDATA_API ::fwData::TransformationMatrix3D::TM3DType getCx()const { return m_intrinsicMatrix->getCoefficient(0,2); };

    /// @return cy or v0 = y-coordinate of optical center
    FWDATA_API ::fwData::TransformationMatrix3D::TM3DType getCy()const { return m_intrinsicMatrix->getCoefficient(1,2); };

    /// @return fx = x-coordinate of optical center
    FWDATA_API ::fwData::TransformationMatrix3D::TM3DType getFx()const { return m_intrinsicMatrix->getCoefficient(0,0); };

    /// @return fy = y-coordinate of optical center
    FWDATA_API ::fwData::TransformationMatrix3D::TM3DType getFy()const { return m_intrinsicMatrix->getCoefficient(1,1); };

    /// @brief set cx or u0 = x-coordinate of optical center
    FWDATA_API void setCx(::fwData::TransformationMatrix3D::TM3DType cx) {  m_intrinsicMatrix->setCoefficient(0,2,cx); };

    /// @brief set cy or v0 = y-coordinate of optical center
    FWDATA_API void setCy(::fwData::TransformationMatrix3D::TM3DType cy) {  m_intrinsicMatrix->setCoefficient(1,2,cy); };

    /// @brief set fx = x-coordinate of optical center
    FWDATA_API void setFx(::fwData::TransformationMatrix3D::TM3DType fx) {  m_intrinsicMatrix->setCoefficient(0,0,fx); };

    /// @brief set fy = y-coordinate of optical center
    FWDATA_API void setFy(::fwData::TransformationMatrix3D::TM3DType fy) {  m_intrinsicMatrix->setCoefficient(1,1,fy); };

    /// @brief get flag if data is available
    FWDATA_API bool &dataAvailable();
    //
    /**
    * @brief get universal ID for the physic's camera
    */
    FWDATA_API std::pair< bool, uint32HL > getCameraGUID() const { return m_cameraGUID; };

    /**
     * @brief set universal ID for the physic's camera
     */
    FWDATA_API void setCameraGUID(uint32HL _cameraGUID);

protected :

    /**
     * @brief constructor
     */
    FWDATA_API Camera();

    /**
     * @brief destructor
     */
    FWDATA_API virtual ~Camera() throw();

    /// extrinsic matrix
    ::fwData::TransformationMatrix3D::sptr m_extrinsicMatrix;

    /// intrinsic matrix
    ::fwData::TransformationMatrix3D::sptr m_intrinsicMatrix;

    //! cx or u0 = x-coordinate of optical center
    ::fwData::TransformationMatrix3D::TM3DType cx;

    //! cy or v0 = y-coordinate of optical center
    ::fwData::TransformationMatrix3D::TM3DType cy;

    //! fx = x-coordinate of optical center
    ::fwData::TransformationMatrix3D::TM3DType fcx;

    //! fy = y-coordinate of optical center
    ::fwData::TransformationMatrix3D::TM3DType fcy;

    //! Universal ID for the physic's camera
    std::pair< bool, uint32HL > m_cameraGUID;

    //! flag if data is available
    bool m_dataAvailable ;

};

} // namespace fwData

#endif // _FWDATA_CAMERA_HPP_
