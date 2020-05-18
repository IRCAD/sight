/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "arData/GenericTL.hpp"
#include "arData/GenericTL.hxx"
#include "arData/timeline/GenericObject.hpp"
#include "arData/timeline/GenericObject.hxx"

#include <fwData/factory/new.hpp>

#include <fwTools/Type.hpp>

fwCampAutoDeclareDataMacro((arData)(FrameTL), ARDATA_API);

namespace arData
{
/**
 * @brief   This class defines a timeline of images.
 */
class ARDATA_CLASS_API FrameTL : public GenericTL< uint8_t >
{

public:
    fwCoreClassMacro(FrameTL, ::arData::TimeLine, ::fwData::factory::New< FrameTL >);
    fwCampMakeFriendDataMacro((arData)(FrameTL))

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    ARDATA_API FrameTL( ::fwData::Object::Key key );

    /// Destructor
    ARDATA_API virtual ~FrameTL();

    /// Defines deep copy
    ARDATA_API virtual void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /// Initializes the size of the pool buffer.
    ARDATA_API void initPoolSize(size_t width, size_t height,
                                 const ::fwTools::Type& type, size_t numberOfComponents = 1,
                                 unsigned int maxElementNum = 1);

    /// Returns the width of an image in the timeline
    size_t getWidth() const
    {
        return m_width;
    }

    /// Returns the height of an image in the timeline
    size_t getHeight() const
    {
        return m_height;
    }

    /// Returns the number of components of an image in the timeline
    size_t getNumberOfComponents() const
    {
        return m_numberOfComponents;
    }

    /// Returns the type of the frame pixel
    ::fwTools::Type getType() const
    {
        return m_type;
    }

private:

    /// Forbid the use of this inherited method.
    ARDATA_API virtual void initPoolSize(unsigned int maxElementNum) override;

    /// frame width
    size_t m_width;

    /// frame height
    size_t m_height;

    /// number of components
    size_t m_numberOfComponents;

    /// type of frame pixel
    ::fwTools::Type m_type;

}; // class FrameTL

} // namespace arData
