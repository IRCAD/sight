/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Point.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <vector>

fwCampAutoDeclareDataMacro((fwData)(Plane), FWDATA_API);
namespace fwData
{
/**
 * @brief   This class defines a plane defined by tree points.
 * @see     ::fwData::Point
 */
class FWDATA_CLASS_API Plane : public Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (Plane)(::fwData::Object), (()), ::fwData::factory::New< Plane >);

    fwCampMakeFriendDataMacro((fwData)(Plane));

    typedef std::array< ::fwData::Point::sptr, 3> PointContainer;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Plane(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~Plane();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /// Re-initialize the plane with 3 points
    FWDATA_API void setValue(::fwData::Point::sptr _point1, ::fwData::Point::sptr _point2,
                             ::fwData::Point::sptr _point3);

    /** @{
     *  @brief get/set points container
     */
    PointContainer& getPoints();
    const PointContainer& getPoints() const;
    void setPoints(const PointContainer& _vPoints);
    /// @}

    /** @{
     *  @brief get/set flag if the plane is an intersection one (else an union one)
     */
    bool getIsIntersection () const;
    void setIsIntersection(bool _isIntersection);
    /// @}

    /**
     * @name Signals
     * @{
     */
    /// Signal emitted when plane is selected/deselected
    typedef ::fwCom::Signal< void (bool) > SelectedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_SELECTED_SIG;
    /**
     * @}
     */

protected:

    //! Points container
    PointContainer m_vPoints;

    //! flag if the plane is an intersection (else an union)
    bool m_isIntersection;

}; // end class Plane

//-----------------------------------------------------------------------------

inline Plane::PointContainer& Plane::getPoints()
{
    return this->m_vPoints;
}

//-----------------------------------------------------------------------------

inline const Plane::PointContainer& Plane::getPoints() const
{
    return this->m_vPoints;
}

//-----------------------------------------------------------------------------

inline void Plane::setPoints(const Plane::PointContainer& _vPoints)
{
    this->m_vPoints = _vPoints;
}

//-----------------------------------------------------------------------------

inline bool Plane::getIsIntersection () const
{
    return m_isIntersection;
}

//-----------------------------------------------------------------------------

inline void Plane::setIsIntersection(bool _isIntersection)
{
    this->m_isIntersection = _isIntersection;
}

//-----------------------------------------------------------------------------

} // end namespace fwData
