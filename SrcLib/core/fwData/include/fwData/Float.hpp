/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_FLOAT_HPP_
#define _FWDATA_FLOAT_HPP_

#include "fwData/GenericField.hpp"

#include "fwData/config.hpp"
#include "fwData/Object.hpp"

namespace fwData
{

/**
 * @class   Float
 * @brief   This class contains an float value
 *
 * Float object is essentially used as a field in other objects.
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API Float : public GenericField< float >
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Float)(::fwData::Object),( ((const float)(0.0f)) ), GenericFieldFactory< Float >) ;
    /**
     * @brief Constructor.
     * @param[in] value The initial value.
     */
    FWDATA_API Float( const float value = 0.0f ) throw();

    /**
     * @brief Destructor.
     */
    FWDATA_API virtual ~Float() throw();

    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( Float::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( Float::csptr _source );
};

} // namespace fwData

#endif // _FWDATA_FLOAT_HPP_
