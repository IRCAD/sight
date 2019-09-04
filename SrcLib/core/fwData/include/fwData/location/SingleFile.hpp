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

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/location/ILocation.hpp"

#include <boost/filesystem.hpp>

fwCampAutoDeclareDataMacro((fwData)(location)(SingleFile), FWDATA_API);

namespace fwData
{
namespace location
{
/**
 * @brief This class defines a single file %location.
 */
class FWDATA_CLASS_API SingleFile  : public ILocation
{
public:

    fwCoreClassDefinitionsWithFactoryMacro( (SingleFile)(ILocation), ::fwData::factory::New< SingleFile >);

    /// SingleFile factory
    FWDATA_API static sptr New(PathType path);

    /// Constructor
    FWDATA_API SingleFile( ::fwData::Object::Key key );

    /// Destructor
    FWDATA_API virtual ~SingleFile();

    fwCampMakeFriendDataMacro((fwData)(location)(SingleFile));

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /// Set file system path
    FWDATA_API void setPath( PathType path);

    /// Get file system path
    FWDATA_API PathType getPath() const;

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
