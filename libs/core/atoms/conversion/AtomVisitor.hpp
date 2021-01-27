/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#pragma once

#include "atoms/config.hpp"

#include <atoms/Object.hpp>

#include <core/tools/UUID.hpp>

#include <map>

namespace sight::data
{
class Object;
}

namespace sight::atoms::conversion
{

/**
 * @brief This class is used to convert a fwAtoms to a fwData.
 */
class ATOMS_CLASS_API AtomVisitor
{

public:

    /**
     * @brief Visitor UUID Management policies
     * @{
     */
    struct IReadPolicy
    {
        virtual SPTR(data::Object) operator()(const std::string& uuid, const std::string& classname) const = 0;
    };

    /**
     * @brief This policy reuses the data associated with an existing uuid
     */
    struct ReusePolicy : IReadPolicy
    { ATOMS_API virtual SPTR(data::Object) operator()(const std::string& uuid,
                                                      const std::string& classname) const; };

    /**
     * @brief This policy changes data's uuid if it already exists
     */
    struct ChangePolicy : IReadPolicy
    { ATOMS_API virtual SPTR(data::Object) operator()(const std::string& uuid,
                                                      const std::string& classname) const; };

    /**
     * @brief This policy throws an exception if the loaded uuid is not available
     */
    struct StrictPolicy : IReadPolicy
    { ATOMS_API virtual SPTR(data::Object) operator()(const std::string& uuid,
                                                      const std::string& classname) const; };
    /** @} */

    typedef std::map< core::tools::UUID::UUIDType, SPTR(data::Object) > DataCacheType;

    /// Constructors. Initializes parameters.
    ATOMS_API AtomVisitor( const atoms::Object::sptr& atomObj, DataCacheType& cache,
                           const IReadPolicy& uuidPolicy );

    /// Destructor. Does nothing.
    ATOMS_API virtual ~AtomVisitor();

    /**
     * @brief Visits the atom information to create the data object and store it in the cache.
     *
     * Creates a new data::Object from classname store in meta info CLASSNAME_METAINFO.
     * Creates this new object with UUID store in atoms::Object ID.
     *
     * @throw atoms::conversion::exception::DataFactoryNotFound if the data class is not found in data::factory
     * @throw atoms::conversion::exception::DuplicatedDataUUID if the data uuid already exists in the system.
     */
    ATOMS_API void visit();

    /// Returns the data object. Calls this method after visit().
    ATOMS_API SPTR(data::Object) getDataObject() const;

private:

    /// Creates an empty data object from the classname in atom info and store it in the cache.
    void processMetaInfos( const atoms::Object::MetaInfosType& metaInfos );

    /// Visits data object to fill it from atom (use AtomToDataMappingVisitor).
    void processAttributes( const atoms::Object::AttributesType& attributes );

    /// Atom object to convert
    atoms::Object::sptr m_atomObj;

    /// Converted data object
    SPTR(data::Object) m_dataObj;

    /// Cache to register the atoms already converted, used when an atom is referenced multiple times.
    DataCacheType& m_cache;

    /// Atom visitor uuids policy
    const IReadPolicy& m_uuidPolicy;
};

} // end namespace sight::atoms::conversion
