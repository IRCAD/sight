/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWATOMSPATCH_STRUCTURALCREATORDB_HPP__
#define __FWATOMSPATCH_STRUCTURALCREATORDB_HPP__

#include <string>
#include <map>
#include <utility>

#include <fwCore/base.hpp>
#include <fwCore/mt/types.hpp>

#include "fwAtomsPatch/config.hpp"

namespace fwAtoms
{
class Object;
}

namespace fwAtomsPatch
{

class IStructuralCreator;

/**
 * @brief Structural creator database.
 * All structural creators are stored in this database.
 */
class FWATOMSPATCH_CLASS_API StructuralCreatorDB
{

public:
    typedef SPTR (StructuralCreatorDB) sptr;
    ///Typedef used to store the pair classname/version of a data structure
    typedef std::pair< std::string, std::string > VersionIDType;
    ///Typedef used to store all the structural creators
    typedef std::map< VersionIDType, SPTR(::fwAtomsPatch::IStructuralCreator) > CreatorsType;

    /**
     * @brief Registers a new creator.
     * @param creator the new creator.
     * @note This method is thread safe.
     */
    FWATOMSPATCH_API void registerCreator(SPTR(::fwAtomsPatch::IStructuralCreator) creator);

    /**
     * @brief Retrieves creator to create data revision.
     * @param classname the structure classname.
     * @param version the structure version.
     * @return creator used to generate the data structure.
     * @note This method is thread safe.
     */
    FWATOMSPATCH_API SPTR(::fwAtomsPatch::IStructuralCreator) getCreator(const std::string& classname,
                                                                         const std::string& version);

    /**
     * @brief Creates a new object instance with given classname and version.
     * @note This method is thread safe.
     */

    FWATOMSPATCH_API SPTR(::fwAtoms::Object) create(const std::string& classname, const std::string& version);

    /// Returns the number of creators.
    FWATOMSPATCH_API size_t size() const;

    /// Returns the default instance of StructuralCreatorDB.
    static StructuralCreatorDB::sptr getDefault()
    {
        return s_default;
    }

    /// Constructor
    StructuralCreatorDB()
    {
    }

    /// Destructor
    ~StructuralCreatorDB()
    {
    }

private:

    /// Copy constructor
    StructuralCreatorDB( const StructuralCreatorDB &cpy )
    {
    }

    /// Mutex to protect concurrent access for m_creators
    mutable ::fwCore::mt::ReadWriteMutex m_mutex;

    /// Structural creators
    CreatorsType m_creators;

    FWATOMSPATCH_API static StructuralCreatorDB::sptr s_default;

};


} // fwAtomsPatch



#endif /* __FWATOMSPATCH_STRUCTURALCREATORDB_HPP__ */
