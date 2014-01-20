/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_INTEGER_HPP_
#define _FWDATA_INTEGER_HPP_

#include "fwData/config.hpp"
#include "fwData/GenericField.hpp"
#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"

fwCampAutoDeclareDataMacro((fwData)(Integer), FWDATA_API);

namespace fwData
{

/**
 * @class   Integer
 * @brief   This class contains an integer value
 *
 * Integer object is essentially used as a field in other objects.
 *
 * 
 * @date    2007-2009.
 */
class FWDATA_CLASS_API Integer : public GenericField< int >
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Integer)(::fwData::Object),( ((const int)(0)) ), GenericFieldFactory< Integer >) ;
    fwCampMakeFriendDataMacro((fwData)(Integer));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Integer( ::fwData::Object::Key key ) throw();

    /**
     * @brief Destructor.
     */
    FWDATA_API virtual ~Integer() throw();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source );

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);
};

} // namespace fwData



#endif // _FWDATA_INTEGER_HPP_
