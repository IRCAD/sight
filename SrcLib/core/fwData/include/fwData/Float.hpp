/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_FLOAT_HPP_
#define _FWDATA_FLOAT_HPP_

#include "fwData/GenericField.hpp"

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"

fwCampAutoDeclareDataMacro((fwData)(Float), FWDATA_API);


namespace fwData
{

/**
 * @brief   This class contains an float value.
 *
 * Float object is essentially used as a field in other objects.
 */
class FWDATA_CLASS_API Float : public GenericField< float >
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Float)(::fwData::Object),( ((const float)(0.0f)) ), GenericFieldFactory< Float >) ;


    fwCampMakeFriendDataMacro((fwData)(Float));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Float( ::fwData::Object::Key key ) throw();

    /**
     * @brief Destructor.
     */
    FWDATA_API virtual ~Float() throw();


    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source );

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

protected:

    FWDATA_API Float() throw();
};

} // namespace fwData

#endif // _FWDATA_FLOAT_HPP_
