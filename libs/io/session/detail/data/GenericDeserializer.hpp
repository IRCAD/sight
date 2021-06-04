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

#include "io/session/config.hpp"
#include "io/session/detail/data/IDataDeserializer.hpp"

namespace sight::io::session
{

namespace detail::data
{

/// Class used to deserialize a generic object from a session
template<typename T>
class GenericDeserializer : public IDataDeserializer
{
public:

    SIGHT_DECLARE_CLASS(GenericDeserializer<T>, IDataDeserializer);

    /// Delete default copy constructors and assignment operators
    GenericDeserializer(const GenericDeserializer&)            = delete;
    GenericDeserializer(GenericDeserializer&&)                 = delete;
    GenericDeserializer& operator=(const GenericDeserializer&) = delete;
    GenericDeserializer& operator=(GenericDeserializer&&)      = delete;

    /// Default constructor
    GenericDeserializer() = default;

    /// Default destructor
    ~GenericDeserializer() override = default;

    /// Deserialization function
    inline sight::data::Object::sptr deserialize(
        const zip::ArchiveReader::sptr& archive,
        const boost::property_tree::ptree& tree,
        const std::map<std::string, sight::data::Object::sptr>& children,
        const sight::data::Object::sptr& object,
        const core::crypto::secure_string& password = ""
    ) const override;
};

//------------------------------------------------------------------------------

template<typename T>
sight::data::Object::sptr GenericDeserializer<T>::deserialize(
    const zip::ArchiveReader::sptr& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string& password
) const
{
    // Create or reuse the object
    const typename T::sptr& newObject = object ? T::dynamicCast(object) : T::New();
    SIGHT_ASSERT("Object '" << newObject->getClassname() << "' is not a '" << T::classname() << "'", newObject);

    // Assign the value
    newObject->setValue(tree.get<typename T::ValueType>("Value"));

    return newObject;
}

} // namespace detail::data

} // namespace sight::io::session
