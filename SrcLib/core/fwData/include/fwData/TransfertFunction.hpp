/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_TRANSFERTFUNCTION_HPP_
#define _FWDATA_TRANSFERTFUNCTION_HPP_


#include <vector>
#include <boost/cstdint.hpp>

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/Factory.hpp"
#include "fwData/Color.hpp"
#include "fwData/Image.hpp"
#include "fwData/TransfertFunctionPoint.hpp"
#include "fwData/DownCastIterator.hpp"

namespace fwData
{
/**
 * @class   TransfertFunction
 * @brief   This class defines a transfert function.
 *
 * A transfert function is composed of some points which have a color and a value.
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API TransfertFunction : public Object
{

public :
    fwCoreClassDefinitionsWithFactoryMacro( (TransfertFunction)(::fwData::Object), (()), ::fwData::Factory::New< TransfertFunction >) ;

    /// Maccro for deep and shallow copies
    fwDataObjectMacro();

    /// Shallow copy method
    FWDATA_API void shallowCopy( TransfertFunction::csptr _source );

    /// Deep copy method
    FWDATA_API void deepCopy( TransfertFunction::csptr _source );

    // TransfertFunctionPoints ---------------------------------------------------------------------
    /// Field identifier for transfert function points
    static const Object::FieldID ID_TRANSFERTFUNCTIONPOINTS;

    typedef ContainerCaster< TransfertFunctionPoint >::iterator      TransfertFunctionPointIterator;
    typedef ContainerCaster< TransfertFunctionPoint >::const_iterator TransfertFunctionPointConstIterator;

    /**
     * @brief Get the number of points in the transfert function
     */
    FWDATA_API boost::uint32_t  getTransfertFunctionPointSize() const;

    /**@{
     * Get iterator on the first and the last points. Use to browse all points (::fwData::TransfertFunctionPoint)
     * @return std::pair( TransfertFunctionPoint.begin(), TransfertFunctionPoint.end() )
     */
    FWDATA_API std::pair< TransfertFunctionPointIterator, TransfertFunctionPointIterator > getTransfertFunctionPoints();
    FWDATA_API std::pair< TransfertFunctionPointConstIterator, TransfertFunctionPointConstIterator > getTransfertFunctionPoints() const;
    // @}

    /// insert a new point in graph, remove previous entry if any
    FWDATA_API void insert( ::fwData::TransfertFunctionPoint::sptr );

    /**
     * @brief Get the color of the point whith given value.
     *
     * @note if value doesn't exist a default point is created then its color is returned
     * @param[in] _i32Value point value
     * @return point color
     */
    FWDATA_API ::fwData::Color::sptr getColor( ::fwData::TransfertFunctionPoint::TFValueType  _i32Value );

    /**
     * @brief erase point
     *
     * @param[in] _i32Value value of the point to erase
     */
    FWDATA_API void erase( ::fwData::TransfertFunctionPoint::TFValueType  _i32Value );

    /// @brief erase all points
    FWDATA_API void clear();

    //  Encoding -----------------------------------------------------------------------------------------------
    fwGettersSettersDocMacro(Encoding, sEncoding, std::string, the encoding)

    //  Name -----------------------------------------------------------------------------------------------
    fwGettersSettersDocMacro(Name, sName, std::string, transfert function name)


    //  Helpers ----------------------------------------------------------------

    typedef std::pair< ::fwData::TransfertFunctionPoint::TFValueType, ::fwData::TransfertFunctionPoint::TFValueType > MinMaxType;
    typedef std::pair< double, ::fwData::TransfertFunctionPoint::TFValueType > CenterWidthType;

    /// Default transfert function name
    FWDATA_API static const std::string defaultTransfertFunctionName;
    FWDATA_API static const std::string squareTransfertFunctionName;

    /**
     * @brief Create the default transfert function with two points
     *
     * Use the window and level of the image to calculate points values.
     *
     * @param[in] _pImage image used to create transfert function
     */
    FWDATA_API static TransfertFunction::sptr createDefaultTransfertFunction( ::fwData::Image::sptr _pImage );

    /// @brief Get transfert function center-width
    FWDATA_API CenterWidthType getCenterWidth() const;
    /// @brief Get transfert function Min-Max
    FWDATA_API MinMaxType getMinMax() const;

    /**
     * @brief Rescale transfert function with the given min and max
     *
     * The first and last points get min and max value, the other point value are calculate proportionately.
     *
     * @param[in] _min minimal value (first point value)
     * @param[in] _max maximal value (last point value)
     */
    FWDATA_API void setMinMax( ::fwData::TransfertFunctionPoint::TFValueType _min, ::fwData::TransfertFunctionPoint::TFValueType _max );

protected :

    /// Constructor
    FWDATA_API TransfertFunction();
    /// Destructor
    FWDATA_API virtual ~TransfertFunction();

    //! Encoding
    std::string m_sEncoding;

    //! Name
    std::string m_sName;

}; // end class TransfertFunctionPoint

} // end namespace fwData

#endif // _FWDATA_TRANSFERTFUNCTION_HPP_
