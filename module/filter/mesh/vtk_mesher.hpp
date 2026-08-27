/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <core/notification/has_monitors.hpp>

#include <data/boolean.hpp>
#include <data/image_series.hpp>
#include <data/integer.hpp>
#include <data/model_series.hpp>
#include <data/real.hpp>

#include <service/filter.hpp>

#include <vtkImageData.h>
#include <vtkPolyData.h>

namespace sight::module::filter::mesh
{

/**
 * @brief This service generates meshes from an image series using VTK library.
 *
 * This filter is composed of three steps:
 * - a mesher, by default a Marching Cubes algorithm,
 * - a smoother, that allows to soften edges,
 * - a decimator, that reduces the number of triangles. This is the only phase that is not multithreaded and
 * unfortunately it is quite slow.
 *
 * The service can either mesh a single value when passed as a property, or it can mesh multiple values if a <config>
 * tag is provided, allowing to specify different settings for each organ.
 *
 * @section Signals Signals
 * - \b completed(): When the mesher succeeded.
 * - \b failed(): When the mesher failed.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="sight::module::filter::mesh::vtk_mesher" >
       <data input_image="${image}" output_model="${result_image}" />
       <config mode="add" />
           <organ name="Vessels" type="Liver" value="1" color="#ff00ff" />
           <organ name="Kidney" type="" value="254" color="#ff00ffe0" />
       </config>
       <smoothing pass_band="0.0001" />
       <decimation reduction="0" />
    </service>
    @endcode
 *
 * @subsection Input Input
 * - \b data.input_image [sight::data::image_series] : image used to generate the output mesh
 * @subsection InOut InOut
 * - \b data.output_model [sight::data::model_series]: mesh generated from ImageSeries
 * @subsection Properties Properties
 * - \b mesh.value: Voxel value used to determine what should considered as a mesh, unless working in config
 * - \b algorithm.flying_edges: Use flying edges algorithm instead of marching cubes.
 * - \b smoothing.pass_band: Set the passband value for the windowed sinc filter.
 * - \b smoothing.boundary: Turn on/off the smoothing of points on the boundary of the mesh.
 * - \b smoothing.non_manifold: Smooth non-manifold points.
 * - \b smoothing.feature: Turn on/off smoothing points along sharp interior edges.
 * - \b smoothing.feature_angle: Specify the feature angle for sharp edge identification (requires feature_smoothing to
 * be on)
 * - \b smoothing.iterations: Specify the number of iterations (i.e., the degree of the polynomial approximating the
 * windowed
 * sinc function).
 * - \b decimation.reduction: Specify the desired reduction in the total number of polygons (e.g., if
 *      TargetReduction is set to 90, this filter will try to reduce the data set to 10% of its original size).
 *      Beware, this operation can be quite slow and proportional to this setting.
 * - \b decimation.preserve_topology: Turn on/off whether to preserve the topology of the original mesh when reducing
 * the number
 * - \b decimation.quadric: Improve decimation quality but increases the time by around 15%. However, it also acts as
 * wonderful smoothing tool and if used with the faster flying_edges mesher, overall, this can provide faster results
 * for the same quality.
 * @subsection Configuration Configuration
 * - \b label : This means that voxels outside the value are converted
 *      to black (bit value of zero), and pixels of the given value are converted to white (a bit value of one).
 * - \b mode(add,replace): Specify if the reconstruction should be added or replace the existing ones.
 * - \b organ: allows to mesh configure each segmentation
 *   - \b name: name of the organ
 *   - \b value: the value to use from the mask to mesh
 *   - \b color (optional, hexadecimal, default=#FFFFFF): color of the mesh
 *   - \b split (optional, boolean): Specify if the reconstructions should be split if they are not geometrically
 * connected.
 *   - \b selected (optional, boolean): Specify if the reconstruction is marked as selected, which will can be used
 * later for instance to highlight it in the visualisation.
 *   - \b material (optional, string): name of the Ogre material used for rendering.
 *   - \b uniforms (optional, string): "|" separated list of uniform keys/values, where values can be interpreted as a
 * sight::data::string_serializable i.e. "u_float=1.0|u_vec3=1.0;5.0;6.0".
 */

class vtk_mesher : public service::filter,
                   public sight::core::notification::has_monitors
{
public:

    SIGHT_DECLARE_SERVICE(vtk_mesher, sight::service::filter);

    vtk_mesher() noexcept;
    ~vtk_mesher() noexcept override = default;

protected:

    connections_t auto_connections() const override;

    /// Set add or replace mode
    void configuring(const config_t& _config) override;

    /// Does nothing
    void starting() override;

    /// Does nothing
    void stopping() override;

    /// Performs the meshing operation
    void updating() override;

private:

    vtkSmartPointer<vtkPolyData> reconstruct(vtkSmartPointer<vtkImageData> _image, int _value);

    enum class mode_t : std::uint8_t
    {
        add,
        replace
    };
    mode_t m_mode {mode_t::add};

    /// Input image mask
    sight::data::ptr<sight::data::image_series, sight::data::access::in> m_input_image {this, "data.input_image"};
    /// Output segmentation
    sight::data::ptr<sight::data::model_series, sight::data::access::inout> m_output_model {this, "data.output_model"};

    sight::data::ptr<data::integer, sight::data::access::in> m_value {this, "label.value", 1};
    sight::data::ptr<data::boolean, sight::data::access::in> m_flying_edges {this, "algorithm.flying_edges", false};
    sight::data::ptr<data::boolean, sight::data::access::in> m_boundary_smoothing {this, "smoothing.boundary", false};
    sight::data::ptr<data::boolean, sight::data::access::in> m_non_manifold_smoothing {this, "smoothing.non_manifold",
                                                                                       true
    };
    sight::data::ptr<data::boolean, sight::data::access::in> m_feature_smoothing {this, "smoothing.feature", false};
    sight::data::ptr<data::real, sight::data::access::in> m_pass_band {this, "smoothing.pass_band", 0.01};
    sight::data::ptr<data::real, sight::data::access::in> m_feature_angle {this, "smoothing.feature_angle", 120.};
    sight::data::ptr<data::integer, sight::data::access::in> m_iterations {this, "smoothing.iterations", 50};

    sight::data::ptr<data::boolean, sight::data::access::in> m_preserve_topology {this, "decimation.preserve_topology",
                                                                                  false
    };
    sight::data::ptr<data::boolean, sight::data::access::in> m_quadric_reduction {this, "decimation.quadric", true};
    sight::data::ptr<data::real, sight::data::access::in> m_percent_reduction {this, "decimation.reduction", 0.0};
};

} // namespace sight::module::filter::mesh
