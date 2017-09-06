/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_BOOLEAN_HPP__
#define __FWDATA_BOOLEAN_HPP__

#include "fwData/GenericField.hpp"
#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"

fwCampAutoDeclareDataMacro((fwData)(Boolean), FWDATA_API);
namespace fwData
{

/**
 * @brief This class contains a boolean value.
 *
 * Boolean object is essentially used as a field in other objects.
 */
class FWDATA_CLASS_API Boolean : public GenericField< bool >
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Boolean)(::fwData::Object),( ((const bool)(false)) ),
                                            GenericFieldFactory< Boolean >);
    fwCampMakeFriendDataMacro((fwData)(Boolean));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Boolean( ::fwData::Object::Key key ) noexcept;

    /**
     * @brief Destructor.
     */
    FWDATA_API virtual ~Boolean() noexcept;

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source );

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

};

} // namespace fwData

#endif // __FWDATA_BOOLEAN_HPP__

