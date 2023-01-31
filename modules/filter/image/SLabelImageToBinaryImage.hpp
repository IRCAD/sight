/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "modules/filter/image/config.hpp"

#include <data/Image.hpp>

#include <service/IFilter.hpp>

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
   <service type="sight::module::filter::image::SLabelImageToBinaryImage">
       <in key="labelImage" uid="..." autoConnect="true" />
       <inout key="binaryMask" uid="..." />
       <config labelsField="maskLabels" />
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b labelImage [sight::data::Image]: Image from which we extract the mask.
 *
 * @subsection InOut InOut
 * - \b binaryMask [sight::data::Image]: The binary mask, extracted from the set of labels or all non zero values.
 *
 * @subsection Configuration Configuration
 * If you decide to set your own parameters:
 * - \b config (optional):
 *     - \b labelsField (optional): name of the field holding the list of labels to add to the mask.
 */
class MODULE_FILTER_IMAGE_CLASS_API SLabelImageToBinaryImage final : public service::IFilter
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SLabelImageToBinaryImage, service::IFilter);

    /// Does nothing.
    MODULE_FILTER_IMAGE_CLASS_API SLabelImageToBinaryImage();

    /// Destroys the service.
    MODULE_FILTER_IMAGE_CLASS_API ~SLabelImageToBinaryImage() override;

protected:

    /// Configures this service.
    MODULE_FILTER_IMAGE_API void configuring() override;

    /// Does nothing.
    MODULE_FILTER_IMAGE_API void starting() override;

    /// Computes the mask from the labeled image.
    MODULE_FILTER_IMAGE_API void updating() override;

    /// Does nothing.
    MODULE_FILTER_IMAGE_API void stopping() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Image::s_BUFFER_MODIFIED_SIG of s_LABEL_IMAGE_INPUT to IService::slots::s_UPDATE
     * Connect data::Image::s_MODIFIED_SIG of s_LABEL_IMAGE_INPUT to IService::slots::s_UPDATE
     */
    MODULE_FILTER_IMAGE_API KeyConnectionsMap getAutoConnections() const override;

private:

    boost::optional<std::string> m_labelSetFieldName;

    static constexpr std::string_view s_LABEL_IMAGE_INPUT = "labelImage";
    static constexpr std::string_view s_BINARY_MASK_INOUT = "binaryMask";

    sight::data::ptr<sight::data::Image, sight::data::Access::in> m_labelImage {this, s_LABEL_IMAGE_INPUT};
    sight::data::ptr<sight::data::Image, sight::data::Access::inout> m_binaryMask {this, s_BINARY_MASK_INOUT};
};

} // namespace sight::module::filter::image.
