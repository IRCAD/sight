/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_SPLINE_HPP_
#define _FWDATA_SPLINE_HPP_

#include <vector>
#include <boost/cstdint.hpp>

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/Color.hpp"

namespace fwData
{
/**
 * @class   Spline
 * @brief   This class defines a spline object.
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API Spline : public Object
{
public :
    fwCoreClassDefinitionsWithFactoryMacro( (Spline)(::fwData::Object::Baseclass), (()), ::fwTools::Factory::New< Spline >) ;

    /**
     * @struct point
     * @brief Defines a 3D %point for a spline.
     */
    typedef struct{
        int id;            /**< @brief Identifier */
        double p[3];       /**< @brief Point coordinates */
        double normal[3];  /**< @brief Normal coordinates*/
        Color c;           /**< @brief Point color*/
        bool isVisible;    /**< @brief Flag if %point is visible */
    }point;

    /// 3D %point container
    typedef std::vector< point > Points ;


    /**
     * @brief constructor
     */
    FWDATA_API Spline();

    /**
     * @brief destructor
     */
    FWDATA_API virtual ~Spline() ;

    /**
     * @brief returns editable point container
     */
    FWDATA_API Points &points() ;

    fwGettersSettersDocMacro(Radius, radius, double, radius value);

    fwGettersSettersDocMacro(NbSides, nbSides, int, number of slices);

    fwGettersSettersDocMacro(IdSpline, idSpline, int, spline identifier);

protected :

    /// Points container
    Points      m_points ;
    double      m_radius;
    int         m_nbSides;
    int         m_idSpline;

} ;

}

#endif // _FWDATA_SPLINE_HPP_
