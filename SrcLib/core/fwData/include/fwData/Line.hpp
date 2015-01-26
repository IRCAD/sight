/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_LINE_HPP__
#define __FWDATA_LINE_HPP__

#include <vector>
#include <utility>

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Point.hpp"

fwCampAutoDeclareDataMacro((fwData)(Line), FWDATA_API);

namespace fwData
{
/**
 * @brief This class defines a Line defined by two points.
 */
class FWDATA_CLASS_API Line : public Object
{

public :
    fwCoreClassDefinitionsWithFactoryMacro( (Line)(::fwData::Object),
        (()), ::fwData::factory::New< Line >) ;


    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Line(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~Line();

    fwCampMakeFriendDataMacro((fwData)(Line));

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source );

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

    FWDATA_API void setValue( ::fwData::Point::sptr _position, ::fwData::Point::sptr  _direction);

    /// Points container
    fwGettersSettersDocMacro(Position, position, ::fwData::Point::sptr, a point positon);
    fwGettersSettersDocMacro(Direction, direction, ::fwData::Point::sptr, a point direction);

protected :

    //! Points container
    ::fwData::Point::sptr m_position;
    ::fwData::Point::sptr m_direction;

}; // class Line

} // namespace fwData

#endif // __FWDATA_LINE_HPP__

