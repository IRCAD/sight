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
#include "data/location/ILocation.hpp"

#include <filesystem>
#include <vector>

namespace sight::data
{
namespace location
{
/**
 * @brief This class defines a multi files location.
 */
class DATA_CLASS_API MultiFiles  : public ILocation
{
public:
    fwCoreClassMacro(MultiFiles, ILocation, data::factory::New< MultiFiles >)

    /// Constructor
    DATA_API MultiFiles( data::Object::Key key );

    /// Destructor
    DATA_API virtual ~MultiFiles();

    /// Set file system paths
    DATA_API void setPaths( VectPathType paths );

    /// Set file system paths
    DATA_API VectPathType getPaths();

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

protected:

    /// file system paths
    VectPathType m_paths;

};

/**
 * @struct enableMultiFiles
 * @brief This class is derived by reader/writer.
 *
 * Reader/Writer classes should only need to implement get/setLocation
 */
template<class RW> // reader or writer class should only need to implement get/setLocation
struct enableMultiFiles
{
    /**
     * @brief constructor
     * @param[in] rw reader or writer
     */
    enableMultiFiles(RW* rw) :
        m_rw(rw)
    {
        SLM_ASSERT("m_rw not instanced", m_rw);
    }

    /// Set file system paths
    void setFiles( ILocation::VectPathType paths )
    {
        getLocation<MultiFiles>(m_rw)->setPaths(paths);
    }

    /// Get file system paths
    ILocation::VectPathType getFiles()
    {
        return getLocation<MultiFiles>(m_rw)->getPaths();
    }

    private:
        /// Not implemented must use constructor with one parameter
        enableMultiFiles();
        RW* m_rw;
};

}
}
