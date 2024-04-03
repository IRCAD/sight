/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include <service/filter.hpp>

#include <boost/optional.hpp>

namespace sight::module::filter::image
{

/**
 * @brief Service extracting a binary mask from an image with labels.
 *
 * @warning This service only handles uint8 images as input but it could easily be extended to support more image types.
 *
 * Either use all non zero values to generate the mask or
 * use a list of labels attached to the input as a field if it exists.
 *
 * This service can be useful to generate masks for importance-driven volume rendering.
 *
 * @section Signals Signals
 * - \b computed() : Signal sent when the mask is extracted.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="sight::module::filter::image::label_image_to_binary_image">
       <in key="labelImage" uid="..." auto_connect="true" />
       <inout key="binaryMask" uid="..." />
       <config labelsField="maskLabels" />
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b labelImage [sight::data::image]: image from which we extract the mask.
 *
 * @subsection InOut InOut
 * - \b binaryMask [sight::data::image]: The binary mask, extracted from the set of labels or all non zero values.
 *
 * @subsection Configuration Configuration
 * If you decide to set your own parameters:
 * - \b config (optional):
 *     - \b labelsField (optional): name of the field holding the list of labels to add to the mask.
 */
class label_image_to_binary_image final : public service::filter
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(label_image_to_binary_image, service::filter);

    /// Does nothing.
    label_image_to_binary_image();

    /// Destroys the service.
    ~label_image_to_binary_image() override;

protected:

    /// Configures this service.
    void configuring() override;

    /// Does nothing.
    void starting() override;

    /// Computes the mask from the labeled image.
    void updating() override;

    /// Does nothing.
    void stopping() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::image::BUFFER_MODIFIED_SIG of s_LABEL_IMAGE_INPUT to service::slots::UPDATE
     * Connect data::image::MODIFIED_SIG of s_LABEL_IMAGE_INPUT to service::slots::UPDATE
     */
    connections_t auto_connections() const override;

private:

    boost::optional<std::string> m_label_set_field_name;

    static constexpr std::string_view LABEL_IMAGE_INPUT = "labelImage";
    static constexpr std::string_view BINARY_MASK_INOUT = "binaryMask";

    sight::data::ptr<sight::data::image, sight::data::access::in> m_label_image {this, LABEL_IMAGE_INPUT};
    sight::data::ptr<sight::data::image, sight::data::access::inout> m_binary_mask {this, BINARY_MASK_INOUT};
};

} // namespace sight::module::filter::image.
