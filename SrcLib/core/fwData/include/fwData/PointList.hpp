/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Object.hpp"
#include "fwData/Point.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <vector>

fwCampAutoDeclareDataMacro((fwData)(PointList), FWDATA_API);

namespace fwData
{
/**
 * @brief   This class defines a list of points.
 * @see     ::fwData::Point
 */
class FWDATA_CLASS_API PointList : public Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (PointList)(::fwData::Object),
                                            (()), ::fwData::factory::New< PointList >);

    fwCampMakeFriendDataMacro((fwData)(PointList));

    typedef std::vector< ::fwData::Point::sptr > PointListContainer;

    /**
     * @brief Constructor
     * @param [::fwData::Object::Key key]: key Private construction key
     */
    FWDATA_API PointList(::fwData::Object::Key key);
    /**
     * @brief Destructor
     */
    FWDATA_API virtual ~PointList();
    /**
     * @brief Defines shallow copy
     */
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;
    /**
     * @brief Defines deep copy
     */
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;
    /**
     * @brief Gets point vector
     * @return [PointListContainer&]: the vector of points
     */
    PointListContainer& getPoints ();
    /**
     * @brief Gets point vector
     * @return [PointListContainer&]: the vector of points
     */
    const PointListContainer& getPoints () const;
    /**
     * @brief Sets point vector
     * @param [const PointListContainer&]: the vector of points to set
     */
    void setPoints (const PointListContainer& _vPoints);
    /**
     * @brief Adds a ::fwData::Point in the pointlist
     * @param [const ::fwData::Point::sptr&]: the point to push
     */
    void pushBack(const ::fwData::Point::sptr&);
    /**
     * @brief: Deletes a point at the specified index
     * @param: [size_t] Index of point to delete
     **/
    void remove(size_t);
    /**
     * @brief Clears the list
     */
    void clear();

    /**
     * @name Signals
     * @{
     */
    /**
     * @brief Signal emitted when a Point is added
     */
    typedef ::fwCom::Signal< void (::fwData::Point::sptr) > PointAddedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_POINT_ADDED_SIG;
    /**
     * @brief Signal emitted when a Point is removed
     */
    typedef ::fwCom::Signal< void (::fwData::Point::sptr) > PointRemovedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_POINT_REMOVED_SIG;
    /**
     * @}
     */

protected:

    /**
     * @brief Points container
     */
    PointListContainer m_vPoints;

}; // end class PointList

//-----------------------------------------------------------------------------

inline PointList::PointListContainer& PointList::getPoints ()
{
    return this->m_vPoints;
}

//-----------------------------------------------------------------------------

inline const PointList::PointListContainer& PointList::getPoints () const
{
    return this->m_vPoints;
}

//-----------------------------------------------------------------------------

inline void PointList::setPoints (const PointList::PointListContainer& _vPoints)
{
    this->m_vPoints = _vPoints;
}

//-----------------------------------------------------------------------------

inline void PointList::pushBack(const ::fwData::Point::sptr& p)
{
    this->m_vPoints.push_back(p);
}

//-----------------------------------------------------------------------------

inline void PointList::remove(size_t _index)
{
    const auto it = m_vPoints.begin() + static_cast<ptrdiff_t>(_index);
    this->m_vPoints.erase(it);
}

//-----------------------------------------------------------------------------

inline void PointList::clear()
{
    this->m_vPoints.clear();
}

} // end namespace fwData
