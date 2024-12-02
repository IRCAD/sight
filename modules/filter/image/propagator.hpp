/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include <core/jobs/base.hpp>

#include <data/boolean.hpp>
#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/real.hpp>
#include <data/string.hpp>

#include <filter/image/min_max_propagation.hpp>

#include <service/filter.hpp>

namespace sight::module::filter::image
{

/**
 * @brief Service propagating in an image from seeds.
 *
 * The update() slot is called when the image changes in order to resize the output mask.
 *
 * @section Signals Signals
 * - \b computed() : Signal sent after propagation when the mouse has been released.
 *
 * @section Slots Slots
 * - \b propagate() : launch the propagation.
 * - \b clear() : reset the mask and the output samples images.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="sight::module::filter::image::propagator">
       <in key="image_in" uid="..." />
       <in key="seeds" uid="..." />
       <inout key="image_out" uid="..." />
       <inout key="samples_out" uid="..." />
       <config value="1" radius="50" overwrite="true" mode="min" />
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b image_in [sight::data::image]: The background image, whose values we read from during propagation.
 * - \b seeds [sight::data::point_list]: List of world coordinates points used to start the propagation.
 *
 * @subsection In-Out In-Out
 * - \b image_out [sight::data::image]: The output image, in which we will draw.
 * - \b samples_out [sight::data::image]: A 1D output image, in which the raw voxels values traversed during the
 * propagation are stored. It may be used to perform statistics on the collected samples, for instance.
 *
 * @subsection Properties Properties
 * - \b value (optional) : The initial value used in the output image. Default 1.
 * - \b radius(optional) : The maximum propagation distance. Infinity by default.
 * - \b overwrite (optional) : The overwrite mode. true by default.
 * - \b mode (optional) : Propagation mode. Possible values are 'min', 'max', 'minmax' and 'stddev'. 'min' by default.
 *  'stddev' stands for standar deviation, in this case the min is set to mean - stddev and mex to mean + stddev.
 */
class propagator : public service::filter
{
public:

    SIGHT_DECLARE_SERVICE(propagator, sight::service::filter);

    struct signals final
    {
        using job_created_t = sight::core::com::signal<void (sight::core::jobs::base::sptr)>;
        static inline const sight::core::com::signals::key_t JOB_CREATED = "job_created";
    };

    struct slots final
    {
        static inline const sight::core::com::slots::key_t CLEAR     = "clear";
        static inline const sight::core::com::slots::key_t PROPAGATE = "propagate";
    };

    /// Initializes slots signals and member variables.
    propagator();

    /// Destroys the service.
    ~propagator() override = default;

protected:

    ///Configures the services' parameters.
    void configuring(const config_t& _config) override;

    /// Initializes line drawer and propagator.
    void starting() override;

    /// Destroys line drawer and propagator.
    void stopping() override;

    /// Allocate the output image and launch the propagation.
    void updating() override;

    /// Launch the propagation.
    void propagate();

    /// Clear the mask and the voxels output
    void clear();

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::image::MODIFIED_SIG of s_IMAGE_IN to service::slots::UPDATE
     * Connect data::point_list::MODIFIED_SIG of s_SEEDS_IN to slots::PROPAGATE
     */
    connections_t auto_connections() const override;

private:

    using coordinates_t = sight::filter::image::min_max_propagation::coordinates_t;

    static constexpr std::string_view IMAGE_IN      = "image_in";
    static constexpr std::string_view SEEDS_IN      = "seeds";
    static constexpr std::string_view IMAGE_INOUT   = "image_out";
    static constexpr std::string_view SAMPLES_INOUT = "samples_out";

    sight::data::ptr<sight::data::image, sight::data::access::in> m_image_in {this, IMAGE_IN};
    sight::data::ptr<sight::data::point_list, sight::data::access::in> m_seeds_in {this, SEEDS_IN};
    sight::data::ptr<sight::data::image> m_image_out {this, IMAGE_INOUT, false};
    sight::data::ptr<sight::data::image> m_samples_out {this, SAMPLES_INOUT, true};
    sight::data::ptr<sight::data::boolean> m_mask_filled_out {this, "mask_filled_out", true};

    sight::data::property<sight::data::real> m_radius {this, "radius", 25.0};
    sight::data::property<sight::data::integer> m_value {this, "value", 1};
    sight::data::property<sight::data::boolean> m_overwrite {this, "overwrite", true};
    sight::data::property<sight::data::string> m_mode {this, "mode", std::string("minmax")};
};

} // namespace sight::module::filter::image.
