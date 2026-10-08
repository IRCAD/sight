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

#include <data/image.hpp>
#include <data/map.hpp>
#include <service/filter.hpp>

namespace sight::module::filter::image
{

/**
 * @brief Alter a mask, replacing the values by another regarding a given dictionary.
 * It can be configured using an input dictionary or an XML configuration.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="..." type="sight::module::filter::image::relabel">
       <input dictionary="${...}" />
       <output image="${...}" />
   </service>
   <service uid="..." type="sight::module::filter::image::relabel">
       <output image="${...}" />
       <config>
           <map from="0" to="42" />
           <map from="2" to="5" />
           <map from="*" to="0" />
       </config>
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b dictionary [sight::data::map]: the dictionary. Optional if XML configuration is used.
 *
 * @subsection In-Out In-Out
 * - \b mask [sight::data::image]: the mask to alter
 */

class relabel final : public sight::service::filter
{
public:

    SIGHT_DECLARE_SERVICE(relabel, sight::service::filter);

    /// Constructor. Does nothing.
    relabel() noexcept;

    /// Destructor. Does nothing.
    ~relabel() noexcept final = default;

protected:

    /**
     * @brief Auto connections
     *
     * - Update service when the mask is modified.
     * - Update service when the dictionary is modified.
     */
    [[nodiscard]] sight::service::connections_t auto_connections() const final;

    /// Does nothing.
    void configuring() final;

    /// Does nothing.
    void starting() final;

    /// Alter input mask using the dictionary
    void updating() final;

    /// Does nothing.
    void stopping() final;

private:

    sight::data::ptr<sight::data::image, sight::data::access::inout> m_image_inout {this, "output.image"};
    sight::data::ptr<sight::data::map, sight::data::access::in> m_dictionary_in {this, "input.dictionary"};
};

} // namespace sight::module::filter::image
