/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "modules/filter/image/threshold.hpp"

#include <core/com/signal.hxx>
#include <core/tools/dispatcher.hpp>

#include <data/image.hpp>
#include <data/image_series.hpp>

namespace sight::module::filter::image
{

//-----------------------------------------------------------------------------

threshold::threshold() noexcept :
    filter(m_signals)
{
}

//-----------------------------------------------------------------------------

void threshold::starting()
{
}

//-----------------------------------------------------------------------------

void threshold::stopping()
{
    m_target.reset();
}

//-----------------------------------------------------------------------------

void threshold::configuring()
{
    const service::config_t& srv_config = this->get_config();

    SIGHT_ASSERT("You must have one <config/> element.", srv_config.count("config") == 1);

    const service::config_t& config = srv_config.get_child("config");

    SIGHT_ASSERT("You must have one <threshold/> element.", config.count("threshold") == 1);
    const service::config_t& threshold_cfg = config.get_child("threshold");
    m_threshold = threshold_cfg.get_value<double>();
}

//-----------------------------------------------------------------------------

/**
 * Functor to apply a threshold filter.
 *
 * The pixel with the value less than the threshold value will be set to 0, else the value is set to the maximum
 * value of the image pixel type.
 *
 * The functor provides a template method operator(param) to apply the filter
 */
struct threshold_filter
{
    struct parameter
    {
        double threshold_value {};   ///< threshold value.
        data::image::csptr image_in; ///< image source
        data::image::sptr image_out; ///< image target: contains the result of the filter
    };

    /**
     * @brief Applies the filter
     * @tparam PIXELTYPE image pixel type (uint8, uint16, int8, int16, float, double, ....)
     */
    template<class PIXELTYPE>
    void operator()(parameter& _param)
    {
        const auto threshold_value  = static_cast<PIXELTYPE>(_param.threshold_value);
        data::image::csptr image_in = _param.image_in;
        data::image::sptr image_out = _param.image_out;
        SIGHT_ASSERT("Sorry, image must be 3D", image_in->num_dimensions() == 3);

        image_out->copy_information(image_in); // Copy image size, type... without copying the buffer
        image_out->resize(image_out->size(), image_out->type(), image_out->pixel_format());
        const auto lockin   = image_in->dump_lock();
        const auto lock_out = image_out->dump_lock();

        // Get iterators on image buffers
        auto it1           = image_in->begin<PIXELTYPE>();
        const auto it1_end = image_in->end<PIXELTYPE>();
        auto it2           = image_out->begin<PIXELTYPE>();
        const auto it2_end = image_out->end<PIXELTYPE>();

        const PIXELTYPE max_value = std::numeric_limits<PIXELTYPE>::max();

        // Fill the target buffer considering the threshold
        for( ; it1 != it1_end && it2 != it2_end ; ++it1, ++it2)
        {
            *it2 = (*it1 < threshold_value) ? 0 : max_value;
        }
    }
};

//-----------------------------------------------------------------------------

void threshold::updating()
{
    threshold_filter::parameter param; // filter parameters: threshold value, image source, image target

    // retrieve the input object
    auto input = m_source.lock();

    // try to dynamic cast to an image and an ImageSeries to know which type of data we use
    data::image_series::csptr image_series_src =
        std::dynamic_pointer_cast<const data::image_series>(input.get_shared());
    data::image::csptr image_src = std::dynamic_pointer_cast<const data::image>(input.get_shared());
    data::object::sptr output;

    // Get source/target image
    if(image_series_src)
    {
        param.image_in = image_series_src;
        data::image_series::sptr image_series_dest = data::image_series::copy(image_series_src);
        // define the input image series as the reference
        image_series_dest->set_dicom_reference(image_series_src->get_dicom_reference());

        // create the output image
        data::image::sptr image_out = std::make_shared<data::image>();
        image_series_dest->image::shallow_copy(image_out);
        param.image_out = image_series_dest;
        output          = image_series_dest;
    }
    else if(image_src)
    {
        param.image_in = image_src;
        // create the output image
        data::image::sptr image_out = std::make_shared<data::image>();
        param.image_out = image_out;
        output          = image_out;
    }
    else
    {
        SIGHT_THROW("Wrong type: source type must be an ImageSeries or an image");
    }

    param.threshold_value = m_threshold;

    // get image type
    core::type type = param.image_in->type();

    /* The dispatcher allows to apply the filter on any type of image.
     * It invokes the template functor ThresholdFilter using the image type.
     * - template parameters:
     *   - core::tools::supported_dispatcher_types defined all the supported type of the functor, here all the type
     *     supported by core::type(std::int8_t, std::uint8_t, std::int16_t, std::uint16_t, std::int32_t,
     *     std::uint32_t, std::int64_t, std::uint64_t, float, double)
     *   - ThresholdFilter: functor struct or class
     * - parameters:
     *   - type: core::type of the image
     *   - param: struct containing the functor parameters (here the input and output images and the threshold value)
     */

    core::tools::dispatcher<core::tools::supported_dispatcher_types, threshold_filter>::invoke(type, param);

    // register the output image to be accesible by the other service from the XML configuration
    m_target = output;
}

//-----------------------------------------------------------------------------

} // namespace sight::module::filter::image
