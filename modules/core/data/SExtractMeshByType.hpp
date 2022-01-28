/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>

#include <service/IService.hpp>
#include <service/macros.hpp>

namespace sight::module::data
{

/**
 * @brief This service extract meshes of a specific structure type from data::ModelSeries.
 * If the model series have some meshes with the same structure type, it will extract the first found.
 * Additionally, it can also match the organ name with a regular expression.
 *
 * The output objects must be marked as "deferred" in the XML configuration.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::data::SExtractMeshByType" >
           <inout key="source" uid="...">
               <extract type="organ1" />
               <extract type="organ2" matching="(.*)surface(.*)"/>
           </inout>
           <out group="target">
               <key uid="..."/>
               <key uid="..."/>
           </out>
        </service>
       @endcode

 * @subsection In-Out In-Out
 * - \b source [sight::data::ModelSeries]: define the source models where meshes are extracted from.
 *
 * @subsection Output Output
 * - \b target [sight::data::Mesh]: define the target meshes. The number of \b target keys must match
 * the number of \b from tags.
 *
 * @subsection Configuration Configuration
 * - \b type (mandatory) : structure type of the mesh to extract from the source model. The number of \b from tags must
 * match the number of \b target keys.
 * - \b matching (optional) : regex to match the organ name, that can be used for instance when you have several
 * organs with the same structure type.
 */
class MODULE_DATA_CLASS_API SExtractMeshByType : public module::data::ICamp
{
public:

    SIGHT_DECLARE_SERVICE(SExtractMeshByType, sight::module::data::ICamp);

    typedef std::map<std::string, std::tuple<std::string, std::string, std::string> > ExtractMapType;
    typedef std::vector<std::pair<std::string, std::string> > ExtractVectorType;

    /// Constructor
    MODULE_DATA_API SExtractMeshByType();

    /// Destructor
    MODULE_DATA_API ~SExtractMeshByType();

protected:

    /// Does nothing
    MODULE_DATA_API void starting() override;

    /// Configure the service
    MODULE_DATA_API void configuring() override;

    /// Does nothing
    MODULE_DATA_API void stopping() override;

    /// Does nothing
    MODULE_DATA_API void updating() override;

private:

    /// Map to associate \<from, to\> for object extraction
    /// @deprecated
    ExtractMapType m_extractOld;

    /// Vector to associate \<from, to\> for object extraction
    ExtractVectorType m_extract;

    static constexpr std::string_view s_SOURCE = "source";
    static constexpr std::string_view s_TARGET = "target";

    sight::data::ptr<sight::data::ModelSeries, sight::data::Access::inout> m_source {this, s_SOURCE, false};
    sight::data::ptr_vector<sight::data::Mesh, sight::data::Access::out> m_target {this, s_TARGET, false};
};

} // namespace sight::module::data
