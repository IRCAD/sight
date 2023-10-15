/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

threshold::threshold() noexcept =
    default;

//-----------------------------------------------------------------------------

threshold::~threshold() noexcept =
    default;

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
    const service::config_t& srvConfig = this->get_config();

    SIGHT_ASSERT("You must have one <config/> element.", srvConfig.count("config") == 1);

    const service::config_t& config = srvConfig.get_child("config");

    SIGHT_ASSERT("You must have one <threshold/> element.", config.count("threshold") == 1);
    const service::config_t& thresholdCfg = config.get_child("threshold");
    m_threshold = thresholdCfg.get_value<double>();
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
struct ThresholdFilter
{
    struct Parameter
    {
        double thresholdValue {};   ///< threshold value.
        data::image::csptr imageIn; ///< image source
        data::image::sptr imageOut; ///< image target: contains the result of the filter
    };

    /**
     * @brief Applies the filter
     * @tparam PIXELTYPE image pixel type (uint8, uint16, int8, int16, float, double, ....)
     */
    template<class PIXELTYPE>
    void operator()(Parameter& param)
    {
        const auto thresholdValue  = static_cast<PIXELTYPE>(param.thresholdValue);
        data::image::csptr imageIn = param.imageIn;
        data::image::sptr imageOut = param.imageOut;
        SIGHT_ASSERT("Sorry, image must be 3D", imageIn->numDimensions() == 3);

        imageOut->copyInformation(imageIn); // Copy image size, type... without copying the buffer
        imageOut->resize(imageOut->size(), imageOut->getType(), imageOut->getPixelFormat());
        const auto lockin  = imageIn->dump_lock();
        const auto lockOut = imageOut->dump_lock();

        // Get iterators on image buffers
        auto it1          = imageIn->begin<PIXELTYPE>();
        const auto it1End = imageIn->end<PIXELTYPE>();
        auto it2          = imageOut->begin<PIXELTYPE>();
        const auto it2End = imageOut->end<PIXELTYPE>();

        const PIXELTYPE maxValue = std::numeric_limits<PIXELTYPE>::max();

        // Fill the target buffer considering the threshold
        for( ; it1 != it1End && it2 != it2End ; ++it1, ++it2)
        {
            *it2 = (*it1 < thresholdValue) ? 0 : maxValue;
        }
    }
};

//-----------------------------------------------------------------------------

void threshold::updating()
{
    ThresholdFilter::Parameter param; // filter parameters: threshold value, image source, image target

    // retrieve the input object
    auto input = m_source.lock();

    // try to dynamic cast to an image and an ImageSeries to know which type of data we use
    data::image_series::csptr imageSeriesSrc = std::dynamic_pointer_cast<const data::image_series>(input.get_shared());
    data::image::csptr imageSrc              = std::dynamic_pointer_cast<const data::image>(input.get_shared());
    data::object::sptr output;

    // Get source/target image
    if(imageSeriesSrc)
    {
        param.imageIn = imageSeriesSrc;
        data::image_series::sptr imageSeriesDest = data::image_series::copy(imageSeriesSrc);
        // define the input image series as the reference
        imageSeriesDest->setDicomReference(imageSeriesSrc->getDicomReference());

        // create the output image
        data::image::sptr imageOut = std::make_shared<data::image>();
        imageSeriesDest->image::shallow_copy(imageOut);
        param.imageOut = imageSeriesDest;
        output         = imageSeriesDest;
    }
    else if(imageSrc)
    {
        param.imageIn = imageSrc;
        // create the output image
        data::image::sptr imageOut = std::make_shared<data::image>();
        param.imageOut = imageOut;
        output         = imageOut;
    }
    else
    {
        SIGHT_THROW("Wrong type: source type must be an ImageSeries or an image");
    }

    param.thresholdValue = m_threshold;

    // get image type
    core::type type = param.imageIn->getType();

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

    core::tools::dispatcher<core::tools::supported_dispatcher_types, ThresholdFilter>::invoke(type, param);

    // register the output image to be accesible by the other service from the XML configuration
    m_target = output;
}

//-----------------------------------------------------------------------------

} // namespace sight::module::filter::image
