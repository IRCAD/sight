/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "ICamp.hpp"

#include "modules/data/config.hpp"

#include <core/base.hpp>

#include <service/IService.hpp>
#include <service/macros.hpp>

namespace sight::module::data
{

/**
 * @brief This service get objects from a source object and expose them as new objects.
 *
 * The output objects must be marked as "deferred" in the XML configuration.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::data::SExtractObj" >
           <inout key="source" uid="...">
               <extract from="@values.myobject1" />
               <extract from="@values.myobject2" />
           </inout>
           <out group="target">
               <key uid="..."/>
               <key uid="..."/>
           </out>
        </service>
       @endcode
 * @subsection In-Out In-Out
 * - \b source [sight::data::Object]: define the source object where objects are extracted from.
 *
 * @subsection Output Output
 * - \b target [sight::data::Object]: define the target objects. The number of \b target keys must match
 * the number of \b from tags.
 *
 * @subsection Configuration Configuration
 * - \b from (mandatory) : key to extract from the source object. The number of \b from tags must match
 * the number of \b target keys.
 */
class MODULE_DATA_CLASS_API SExtractObj : public module::data::ICamp
{
public:

    SIGHT_DECLARE_SERVICE(SExtractObj, ::sight::module::data::ICamp);

    typedef std::map<std::string, std::string> ExtractMapType;

    /// Constructor
    MODULE_DATA_API SExtractObj();

    /// Destructor
    MODULE_DATA_API ~SExtractObj();

protected:

    /// Does nothing
    MODULE_DATA_API void starting() override;

    /// Configure the service
    MODULE_DATA_API void configuring() override;

    /// Does nothing
    MODULE_DATA_API void stopping() override;

    /// Extract the object(s)
    MODULE_DATA_API void updating() override;

private:

    /// Map to associate \<from, to\> for object extraction
    ExtractMapType m_extract;

    /// Vector to associate source paths
    std::vector<std::string> m_sourcePaths;

    sight::data::ptr<sight::data::Object, sight::data::Access::inout> m_source {this, "source"};
    sight::data::ptr_vector<sight::data::Object, sight::data::Access::out> m_target {this, "target", false};
};

} // namespace sight::module::data
