/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "data/config.hpp"
#include "data/factory/new.hpp"
#include "data/location/ILocation.hpp"

#include <filesystem>

fwCampAutoDeclareDataMacro((sight)(data)(location)(SingleFile));

namespace sight::data
{
namespace location
{
/**
 * @brief This class defines a single file %location.
 */
class DATA_CLASS_API SingleFile  : public ILocation
{
public:

    fwCoreClassMacro(SingleFile, ILocation, data::factory::New< SingleFile >)

    /// SingleFile factory
    DATA_API static sptr New(PathType path);

    /// Constructor
    DATA_API SingleFile( data::Object::Key key );

    /// Destructor
    DATA_API virtual ~SingleFile();

    fwCampMakeFriendDataMacro((sight)(data)(location)(SingleFile));

    /// Defines shallow copy
    DATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /// Set file system path
    DATA_API void setPath( PathType path);

    /// Get file system path
    DATA_API PathType getPath() const;

protected:

    /// file system path
    PathType m_path;

};

/**
 * @struct enableSingleFile
 * @brief This class is derived by reader/writer.
 *
 * Reader/Writer classes should only need to implement get/setLocation
 */
template<typename RW> // reader or writer class should only need to implement get/setLocation
struct enableSingleFile
{
    /**
     * @brief constructor
     * @param[in] rw reader or writer
     */
    enableSingleFile(RW* rw) :
        m_rw(rw)
    {
        SLM_ASSERT("m_rw not instanced", m_rw);
    }

    /// Set file system path
    void setFile(ILocation::PathType path)
    {
        getLocation<SingleFile>(m_rw)->setPath(path);
    }

    /// Get file system path
    ILocation::PathType getFile()
    {
        return (getLocation<SingleFile>(m_rw))->getPath();
    }

    private:
        /// Not implemented must use constructor with one parameter
        enableSingleFile();
        RW* m_rw;

};

}
}
