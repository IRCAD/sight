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
#include "io/session/detail/data/IDataSerializer.hpp"

namespace sight::io::session
{

namespace detail::data
{

/// Class used to serialize composite object to a session
class EquipmentSerializer : public IDataSerializer
{
public:

    SIGHT_DECLARE_CLASS(EquipmentSerializer, IDataSerializer)

    /// Delete default copy constructors and assignment operators
    EquipmentSerializer(const EquipmentSerializer&)            = delete;
    EquipmentSerializer(EquipmentSerializer&&)                 = delete;
    EquipmentSerializer& operator=(const EquipmentSerializer&) = delete;
    EquipmentSerializer& operator=(EquipmentSerializer&&)      = delete;

    /// Default constructor
    EquipmentSerializer() = default;

    /// Default destructor
    ~EquipmentSerializer() override = default;

    /// Serialization function
    void serialize(
        const zip::ArchiveWriter::sptr& archive,
        boost::property_tree::ptree& tree,
        const sight::data::Object::csptr& object,
        std::map<std::string, sight::data::Object::csptr>& children,
        const core::crypto::secure_string& password = ""
    ) const override;
};

} // namespace detail::data

} // namespace sight::io::session
