/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARDATA_CAMERA_HPP_
#define _ARDATA_CAMERA_HPP_

#include <utility>

#include <boost/cstdint.hpp>
#include <boost/array.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwData/macros.hpp>
#include <fwData/Object.hpp>
#include <fwData/factory/new.hpp>

#include "arData/config.hpp"


fwCampAutoDeclareDataMacro((arData)(Camera), ARDATA_API);

namespace arData
{
/**
 * @class   Camera
 * @brief   This class defines a camera object.
 */
class ARDATA_CLASS_API Camera : public ::fwData::Object
{
public:
    fwCampMakeFriendDataMacro((arData)(Camera));
    fwCoreClassDefinitionsWithFactoryMacro((Camera)(::fwData::Object), (()), ::fwData::factory::New< Camera >);

    typedef ::boost::array<double, 5> DistArrayType;
    typedef ::boost::array<double, 6> IntrinsecType;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    ARDATA_API Camera( ::fwData::Object::Key key );

    /// Destructor
    ARDATA_API virtual ~Camera() throw();


    /// Defines shallow copy
    ARDATA_API void shallowCopy( const ::fwData::Object::csptr& _source );

    /// Defines deep copy
    ARDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);


    /**@name Signals API
     * @{
     */
    ARDATA_API static const ::fwCom::Signals::SignalKeyType s_INTRINSIC_CALIBRATED_SIG;
    typedef ::fwCom::Signal<void ()> IntrinsicCalibratedSignalType;

    ARDATA_API static const ::fwCom::Signals::SignalKeyType s_ID_MODIFIED_SIG;
    typedef ::fwCom::Signal<void (std::string)> IdModifiedSignalType;
    /** @} */

    /// @return cx or u0 = x-coordinate of optical center
    double getCx() const
    {
        return m_intrinsic[2];
    }

    /// @return cy or v0 = y-coordinate of optical center
    double getCy() const
    {
        return m_intrinsic[3];
    }

    /// Sets cx or u0 = x-coordinate of optical center
    void setCx(double cx)
    {
        m_intrinsic[2] = cx;
    }

    /// Sets cy or v0 = y-coordinate of optical center
    void setCy(double cy)
    {
        m_intrinsic[3] = cy;
    }

    /// @return fx = x-coordinate of optical center
    double getFx() const
    {
        return m_intrinsic[0];
    }

    /// Sets fx = x-coordinate of optical center
    void setFx(double fx)
    {
        m_intrinsic[0] = fx;
    }

    /// @return fy = y-coordinate of optical center
    double getFy() const
    {
        return m_intrinsic[1];
    }

    /// Sets fy = y-coordinate of optical center
    void setFy(double fy)
    {
        m_intrinsic[1] = fy;
    }

    /// @return camera resolution in pixels
    double getWidth() const
    {
        return m_intrinsic[4];
    }

    /// Sets camera resolution in pixels
    void setWidth(double w)
    {
        m_intrinsic[4] = w;
    }

    /// @return camera resolution in pixels
    double getHeight() const
    {
        return m_intrinsic[5];
    }

    /// Sets camera resolution in pixels
    void setHeight(double h)
    {
        m_intrinsic[5] = h;
    }

    /// Sets the distortion coefficient
    ARDATA_API void setDistortionCoefficient(double k1, double k2, double p1, double p2, double k3);

    /// Gets/Sets the distortion coefficient
    fwDataGetSetCRefMacro(DistortionCoefficient, DistArrayType);

    /// Gets/Sets the skew coefficient (angle between the x and y pixel axes)
    fwDataGetSetMacro(Skew, double);

    /// Get/Set the flag is camera is calibrated
    fwDataGetSetMacro(IsCalibrated, bool);

    /// Gets camera description
    std::string getDescription() const
    {
        return m_description;
    }

    /// Sets camera description
    void setDescription(const std::string & description)
    {
        m_description = description;
    }

    /// Gets camera unique identifier
    std::string getCameraID() const
    {
        return m_cameraID;
    }

    /// Sets camera  unique identifier
    void setCameraID(const std::string & cameraID)
    {
        m_cameraID = cameraID;
    }


protected:


    /// intrinsic parameters
    IntrinsecType m_intrinsic;

    /// Image distortion coefficients (radial and tangential distortions, [k1, k2, p1, p2, k3])
    DistArrayType m_attrDistortionCoefficient;

    /// skew coefficient (angle between the x and y pixel axes)
    double m_attrSkew;

    //! flag if data is calibrated
    bool m_attrIsCalibrated;

    //! Camera description
    std::string m_description;

    //! Camera unique identifier
    std::string m_cameraID;

    /// Signal when camera is calibrated
    IntrinsicCalibratedSignalType::sptr m_sigIntrinsicCalibrated;

    /// Signal when camera id is modified
    IdModifiedSignalType::sptr m_sigIdModified;
};

} // namespace arData

#endif // _ARDATA_CAMERA_HPP_
