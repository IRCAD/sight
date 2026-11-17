/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <data/boolean.hpp>
#include <data/model_series.hpp>
#include <data/string.hpp>

#include <service/controller.hpp>

namespace sight::module::data
{

/**
 * @brief Shows or hides reconstructions in a model series by structure type and/or organ name.
 *
 * Each <organ> entry is a rule. Its non-empty type and name regular expressions are combined with AND;
 * multiple entries are combined with OR.
 *
 * @section XML XML Configuration
 * @code{.xml}
 * <service uid="..." type="sight::module::data::show_mesh">
 *     <data model="${...}" visible="${...}" />
 *     <organ type="Liver" name="(.*)surface(.*)" />
 *     <organ type="Kid.*" name="(.*)surface(.*)" />
 * </service>
 * @endcode
 *
 * @subsection In-Out In-Out
 * - \b data.model [sight::data::model_series]: model series containing the reconstructions to update.
 *
 * @subsection Input Input
 * - \b organ.type [sight::data::string]: optional regular expression for the reconstruction structure type.
 * - \b organ.name [sight::data::string]: optional regular expression for the reconstruction organ name.
 * - \b visible [sight::data::boolean]: visibility applied to matching reconstructions.
 */
class show_mesh final : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(show_mesh, service::controller);

    show_mesh()        = default;
    ~show_mesh() final = default;

protected:

    service::connections_t auto_connections() const final;

    void configuring() final;
    void starting() final;
    void stopping() final;
    void updating() final;

private:

    ptr_inout<sight::data::model_series> m_model {this, "data.model"};
    ptr_in<sight::data::boolean> m_visible {this, "data.visible"};
    ptr_vector_in<sight::data::string> m_organ_types {this, "organ.type", {}};
    ptr_vector_in<sight::data::string> m_organ_names {this, "organ.name", {}};
};

} // namespace sight::module::data
