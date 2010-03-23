/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_LINE_HPP_
#define _FWDATA_LINE_HPP_

#include <vector>
#include <utility>

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/Point.hpp"


namespace fwData
{
/**
 * @class   Line
 * @brief   This class defines a Line defined by two points.
 * @see     ::fwData::Line
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API Line : public Object
{

public :
    fwCoreClassDefinitionsWithFactoryMacro( (Line)(::fwData::Object::Baseclass),
        (()), ::fwTools::Factory::New< Line >) ;

    /// Constructor
    FWDATA_API Line();

    /// Build a Line from 2 points.
    FWDATA_API Line(::fwData::Point::sptr _position, ::fwData::Point::sptr  _direction);


    /// Destructor
    FWDATA_API virtual ~Line();


    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( Line::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( Line::csptr _source );

    /// Clone method
    //FWDATA_API Line::sptr clone() const;

    /// Copy method
    //FWDATA_API Line &operator=( const Line & _Line ) ;

    FWDATA_API void setValue( ::fwData::Point::sptr _position, ::fwData::Point::sptr  _direction);

    /// Points container
    fwGettersSettersDocMacro(Position, position, ::fwData::Point::sptr, a point positon);
    fwGettersSettersDocMacro(Direction, direction, ::fwData::Point::sptr, a point direction);

protected :

    //! Points container
    ::fwData::Point::sptr m_position;
    ::fwData::Point::sptr m_direction;

}; // end class Line

} // end namespace fwData

#endif // _FWDATA_LINE_HPP_
