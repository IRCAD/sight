/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "modules/data/config.hpp"

#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>

#include <service/controller.hpp>

namespace sight::module::data
{

/**
 * @brief This service is used to  extracts mesh in the models series
 *
 * The output objects must be marked as "deferred" in the XML configuration.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service uid="..." type="sight::module::data::SGetMesh" >
    <in key="modelSeries" uid="..." />
    <out group="mesh">
        <key index="3" uid="..."/>
        <key type="organ1" uid="..."/>
        <key type="organ2" matching="(.*)surface(.*)" uid="..."/>
    </out>
   </service>

       @endcode
 * @subsection Input Input
 * - \b modelSeries [sight::data::ModelSeries]: model series from which the mesh is extracted.
 *
 * @subsection Output Output
 * - \b mesh [sight::data::Mesh]: output mesh extracted from the model series.
 * *
 * @subsection Configuration Configuration
 *  Regarding the ouput configuration, either index or type is required. It is possible to mix both of them and their
 * order are not relevant.
 *  Both index and type specify which mesh will be found from the reconstructionDB. The given mesh will then be put in
 * the associated output var.
 * - \b index (mandatory) : index in the modelSeries of the mesh to extract. Note that the index starts at 0 for the
 * first element in the modelSeries.
 * - \b type (mandatory) : structure type of the mesh to extract from the source model.
 * - \b matching (optional) : regex to match the organ name, that can be used for instance when you have several
 * organs with the same structure type. type cannot be applied to an index constraint.
 */
class MODULE_DATA_CLASS_API SGetMesh : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(SGetMesh, service::controller);

    /// Constructor
    MODULE_DATA_API SGetMesh() = default;

    /// Destructor
    MODULE_DATA_API ~SGetMesh() override = default;

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

    /// structure which contains the element of an index configuration:
    /// inputIndex: index of the mesh in the modelSeries
    /// outputIndex: index of the output variable
    struct indexConfig
    {
        std::size_t inputIndex;
        std::size_t outputIndex;
    };

    /// structure which contains the element of a type configuration:
    /// type: the mesh type requested
    /// matching: regex for the mesh name
    /// outputIndex: index of the output variable
    struct typeConfig
    {
        std::string type;
        std::string matching;
        std::size_t outputIndex;
    };

    /// The list of index oriented configurations.
    std::vector<indexConfig> m_indexConfigurations;
    /// The list of type-name configurations.
    std::vector<typeConfig> m_typeConfiguration;

    /// The in/out variables
    sight::data::ptr<sight::data::ModelSeries, sight::data::Access::in> m_modelSeries {this, "modelSeries"};
    sight::data::ptr_vector<sight::data::Mesh, sight::data::Access::out> m_mesh {this, "mesh"};
};

} // namespace sight::module::data
