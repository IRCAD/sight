/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWDATA_FLOAT_HPP__
#define __FWDATA_FLOAT_HPP__

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/GenericField.hpp"
#include "fwData/Object.hpp"

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
    fwCoreClassDefinitionsWithFactoryMacro( (Float)(::fwData::Object), ( ((const float)(0.0f)) ),
                                            GenericFieldFactory< Float >);

    fwCampMakeFriendDataMacro((fwData)(Float));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Float( ::fwData::Object::Key key ) noexcept;

    /**
     * @brief Destructor.
     */
    FWDATA_API virtual ~Float() noexcept;

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

protected:

    FWDATA_API Float() noexcept;
};

} // namespace fwData

#endif // __FWDATA_FLOAT_HPP__
