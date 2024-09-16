/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "service/base.hpp"

#include <core/object.hpp>

namespace sight::service
{

/**
 * @brief Service type for the construction of an object and associated services from an XML-based description
 *
 * This includes (data) object attributes initialization as well as XML declared service creation, attachment and
 * configuration.
 * As each specific (data) object has a specific structure (attributes), it must be specialized for each one.
 *
 * The parse() method of this base class parses the XML description: each object named XML children corresponds to a
 * field added to the object (see core::object).
 */
class SIGHT_SERVICE_CLASS_API object_parser : public service::base
{
public:

    SIGHT_DECLARE_CLASS(object_parser, service::base);

    // Contains created objects indexed by uid
    using objects_map_t = std::unordered_map<std::string, sight::data::object::sptr>;
    // Contains uids of objects
    using objects_set_t = std::set<std::string>;
    // Contains created objects, deferred objects and preferences objects
    struct objects_t
    {
        objects_map_t created;
        objects_set_t deferred;
        objects_set_t prefs;

        //------------------------------------------------------------------------------

        void merge(const objects_t& _other)
        {
            std::ranges::copy(_other.created, std::inserter(this->created, this->created.begin()));
            std::ranges::copy(_other.prefs, std::inserter(this->prefs, this->prefs.begin()));
        }
    };

    SIGHT_SERVICE_API virtual void parse(
        const service::config_t& _cfg_elem,
        core::object::sptr _obj,
        objects_t& _sub_objects
    ) = 0;

protected:

    /// Constructor. Does nothing.
    SIGHT_SERVICE_API object_parser() = default;

    /// Destructor. Does nothing.
    SIGHT_SERVICE_API ~object_parser() override = default;

    /// Does nothing
    SIGHT_SERVICE_API void configuring() final;

    /// Does nothing
    SIGHT_SERVICE_API void starting() final;

    /// Does nothing
    SIGHT_SERVICE_API void updating() final;

    /// Does nothing
    SIGHT_SERVICE_API void stopping() final;
};

} // namespace sight::service
