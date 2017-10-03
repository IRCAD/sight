/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_INTEGER_HPP__
#define __FWDATA_INTEGER_HPP__

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/GenericField.hpp"
#include "fwData/Object.hpp"

fwCampAutoDeclareDataMacro((fwData)(Integer), FWDATA_API);

namespace fwData
{

/**
 * @brief   This class contains an integer value.
 * Integer object is essentially used as a field in other objects.
 */
class FWDATA_CLASS_API Integer : public GenericField< int >
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Integer)(::fwData::Object), ( ((const int)(0)) ),
                                            GenericFieldFactory< Integer >);
    fwCampMakeFriendDataMacro((fwData)(Integer));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Integer( ::fwData::Object::Key key ) noexcept;

    /**
     * @brief Destructor.
     */
    FWDATA_API virtual ~Integer() noexcept;

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;
};

} // namespace fwData

#endif // __FWDATA_INTEGER_HPP__

