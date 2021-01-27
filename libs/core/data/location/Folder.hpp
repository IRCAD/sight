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

fwCampAutoDeclareDataMacro((sight)(data)(location)(Folder));

namespace sight::data
{
namespace location
{
/**
 * @brief This class defines a folder location.
 */
class DATA_CLASS_API Folder : public ILocation
{
public:

    fwCoreClassMacro(Folder, ILocation, data::factory::New< Folder >)

    fwCampMakeFriendDataMacro((sight)(data)(location)(Folder));

    /// Folder factory
    DATA_API static sptr New(PathType path, bool recursive = false );

    /// Constructor
    DATA_API Folder( data::Object::Key key );

    /// Destructor
    DATA_API virtual ~Folder();

    /// Defines shallow copy
    DATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /// @brief Set folder filesystem path
    DATA_API void setFolder( PathType folder);

    /// @brief Get folder filesystem path
    DATA_API PathType getFolder() const;

    /// @brief Set the flag if folder location is recursive
    DATA_API void setRecursive( bool recursive);

    /// @brief Get the flag if folder location is recursive
    DATA_API bool getRecursive();

protected:

    /// %Folder path
    PathType m_folder;

    /// Flag if folder is recursive
    bool m_isRecursive;

};

/**
 * @struct enableFolder
 * @brief This class is derived by reader/writer.
 *
 * Reader/Writer classes should only need to implement get/setLocation
 */
template<class RW> // reader or writer class should only need to implement get/setLocation
struct enableFolder
{
    /**
     * @brief constructor
     * @param[in] rw reader or writer
     */
    enableFolder(RW* rw) :
        m_rw(rw)
    {
        SLM_ASSERT("m_rw not instanced", m_rw);
    }

    /// @brief Set folder filesystem path
    void setFolder(ILocation::PathType folder)
    {
        getLocation<Folder>(m_rw)->setFolder(folder);
    }

    /// @brief Get folder filesystem path
    ILocation::PathType getFolder()
    {
        return getLocation<Folder>(m_rw)->getFolder();
    }

    /// @brief Set the flag if folder location is recursive
    void setRecursive( bool val)
    {
        getLocation<Folder>(m_rw)->setRecursive(val);
    }

    /// @brief Get the flag if folder location is recursive
    bool getRecursive()
    {
        return getLocation<Folder>(m_rw)->getRecursive();
    }

    private:
        /// Not implemented must use constructor with one parameter
        enableFolder();

        /// Reader or writer
        RW* m_rw;

};

}
}
