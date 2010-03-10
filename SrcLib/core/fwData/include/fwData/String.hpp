/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_STRING_HPP_
#define _FWDATA_STRING_HPP_


#include "fwData/Object.hpp"
#include "fwData/GenericField.hpp"

namespace fwData
{
/**
 * @class   String
 * @brief   This class contains an std::string value
 *
 * String object is essentially used as a field in other objects.
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API String : public GenericField< std::string >
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (String)(::fwData::Object::Baseclass), ( ((const std::string)("")) ), GenericFieldFactory< String >) ;
    /**
     * @brief Constructor.
     * @param[in] value The initial value.
     */
    FWDATA_API String(const std::string _value="" ) throw();

    /**
     * @brief Destructor.
     */
    FWDATA_API virtual ~String() throw();

    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( String::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( String::csptr _source );

};

}


#endif /*_FWDATA_STRING_HPP_*/
