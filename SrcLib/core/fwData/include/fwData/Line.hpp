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

#include <utility>
#include <vector>

fwCampAutoDeclareDataMacro((fwData)(Line), FWDATA_API);

namespace fwData
{
/**
 * @brief This class defines a Line defined by two points.
 */
class FWDATA_CLASS_API Line : public Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (Line)(::fwData::Object),
                                            (()), ::fwData::factory::New< Line >);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Line(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~Line();

    fwCampMakeFriendDataMacro((fwData)(Line));

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    FWDATA_API void setValue(const ::fwData::Point::sptr& _position, const ::fwData::Point::sptr&  _direction);

    /** @{
     *  @brief get/set point position
     */
    ::fwData::Point::sptr& getPosition();
    const ::fwData::Point::sptr& getPosition() const;
    void setPosition(const ::fwData::Point::sptr& _position);
    /// @}

    /** @{
     *  @brief get/set point direction
     */
    ::fwData::Point::sptr& getDirection();
    const ::fwData::Point::sptr& getDirection() const;
    void setDirection (const ::fwData::Point::sptr& _direction);
    /// @}

protected:

    //! Points container
    ::fwData::Point::sptr m_position;
    ::fwData::Point::sptr m_direction;

}; // class Line

//-----------------------------------------------------------------------------

inline ::fwData::Point::sptr& Line::getPosition()
{
    return this->m_position;
}

//-----------------------------------------------------------------------------

inline const ::fwData::Point::sptr& Line::getPosition() const
{
    return this->m_position;
}

//-----------------------------------------------------------------------------

inline void Line::setPosition(const ::fwData::Point::sptr& _position)
{
    this->m_position = _position;
}

//-----------------------------------------------------------------------------

inline ::fwData::Point::sptr& Line::getDirection()
{
    return this->m_direction;
}

//-----------------------------------------------------------------------------

inline const ::fwData::Point::sptr& Line::getDirection() const
{
    return this->m_direction;
}

//-----------------------------------------------------------------------------

inline void Line::setDirection (const ::fwData::Point::sptr& _direction)
{
    this->m_direction = _direction;
}

//-----------------------------------------------------------------------------

} // namespace fwData
