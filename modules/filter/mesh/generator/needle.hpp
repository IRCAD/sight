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

#include "modules/filter/mesh//config.hpp"

#include <data/mesh.hpp>

#include <service/generator.hpp>

#include <vtkAlgorithmOutput.h>
#include <vtkPolyData.h>

namespace sight::module::filter::mesh::generator
{

/**
 * @brief Generates a cylinder shape into a data::mesh using VTK.
 *
 * @section Slots Slots
 * - \b updateHeight(): Update the height of the cylinder.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service uid="cylinderGenerator" type="opVTKMesh::needle" >
            <inout key="mesh" uid="..." />
            <config radius="5.0" height="10.0" resolution="100" />
            <needle color="#ffffff">
                <minorSteps color="afafaf" length="10.0"/>
                <majorSteps color="ff0000" steps="5"/>
            </needle>
        </service>
   @endcode
 *
 * @subsection Configuration:
 * - \b radius (optional) (double, default=1.0): half of the diameter.
 * - \b height (optional) (double, default=10.0): needle length.
 * - \b resolution (optional) (integer, default=42): number of facets of the cylinder.
 * - \b offsetToOrigin (optional) (default=false): move the cylinder so that the end is the origin
 * - \b needle (optional): if you need to have cylinder looking like a needle (i.e. with a pointed tip).
 *     - \b color (optional) (hexadecimal string, default="#ffffff"): color of the needle.
 *     - \b minorSteps (optional): tag to get visible steps on the needle (every centimeters by default)
 *         - \b color (optional) (hexadecimal string, default="#afafaf"): steps color.
 *         - \b length (optional) (double, default="10.0"): steps length.
 *     - \b majorSteps (optional): tag to get visible major steps on the needle (every 5 centimeters by default)
 *         - \b color (optional) (hexadecimal string, default="#ff0000"): major steps color.
 *         - \b steps (optional) (integer, default="5"): mark a major step on every 'steps' (i.e. 5 * minorSteps*length
 * mm by default).
 *
 * @subsection In-Out In-Out:
 * - \b mesh [sight::data::mesh]: generated mesh.
 */
class MODULE_FILTER_MESH_CLASS_API needle final : public sight::service::generator
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(needle, sight::service::generator);

    /// Initializes slots.
    MODULE_FILTER_MESH_API needle() noexcept;

    /// Destroys the service.
    MODULE_FILTER_MESH_API ~needle() noexcept override;

protected:

    /// Configures the service's parameters.
    MODULE_FILTER_MESH_API void configuring() override;

    /// Does nothing.
    MODULE_FILTER_MESH_API void starting() override;

    /// Does nothing.
    MODULE_FILTER_MESH_API void stopping() override;

    /// Generate the cylinder.
    MODULE_FILTER_MESH_API void updating() override;

private:

    /// Updates the height of the cylinder.
    void updateHeight(double /*height*/);

    /// radius of the cylinder.
    double m_radius {1.0};

    /// height of the cylinder.
    double m_height {10.0};

    /// number of facets.
    int m_resolution {42};

    /// offset.
    bool m_offsetToOrigin {false};

    /// Needle mode or cylinder mode.
    bool m_needleMode {false};

    /// Needle base color.
    std::array<unsigned char, 4> m_needleColor {255, 255, 255, 255};

    /// Needle minor steps color.
    std::array<unsigned char, 4> m_needleMinorStepsColor {175, 175, 175, 255};

    /// Needle minor steps length.
    double m_needleMinorStepsLength {10.0};

    /// Needle major steps color.
    std::array<unsigned char, 4> m_needleMajorStepsColor {255, 0, 0, 255};

    /// Needle major steps, i.e. put a major step every 5 minor steps by default.
    unsigned int m_needleMajorSteps {5};

    /**
     * \brief Construct a needle if in needle mode.
     *
     * \return vtkSmartPointer on a vtkPolyData that handles the geometry of the constructed needle.
     */
    vtkSmartPointer<vtkPolyData> constructNeedle();

    /**
     * \brief Construct a source object regarding the class template T (works with vtkCylinderSource and vtkConeSource).
     *
     * \param _height Height of the object that will be generated.
     * \param _center Center of the object to move it in the 3D scene.
     *
     * \return vtkSmartPointer on a class T type source object (i.e. vtkCylinderSource or vtkConeSource).
     */
    template<class T>
    vtkSmartPointer<T> constructSourceObject(double _height, double _center);

    /**
     * \brief Filter and color an object through its source algorithm (i.e. source->GetOutputPort()).
     *
     * \param _sourceAlgorithm Source algorithm of the object that needs to be filtered and colored.
     * \param _rgba Needed RGBA color to apply on the object.
     *
     * \return vtkSmartPointer on a vtkPolyData that handles the geometry of the filtered and colored object.
     */
    static vtkSmartPointer<vtkPolyData> filterAndColorSourceObject(
        vtkAlgorithmOutput* _source_algorithm,
        const std::array<unsigned char, 4>& _rgba
    );

    /**
     * \brief Generates a torus in the right needle axis.
     *
     * \param _center Center of the object to move it in the 3D scene.
     * \param _rgba Needed RGBA color to apply on the torus.
     *
     * \return vtkSmartPointer on a vtkPolyData that handles the geometry of the torus.
     */
    vtkSmartPointer<vtkPolyData> generateTorus(double _center, const std::array<unsigned char, 4>& _rgba) const;

    sight::data::ptr<sight::data::mesh, sight::data::Access::inout> m_mesh {this, "mesh"};
};

} // namespace sight::module::filter::mesh::generator
