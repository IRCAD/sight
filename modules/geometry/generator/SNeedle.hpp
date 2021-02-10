/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "modules/geometry/generator/config.hpp"

#include <services/IGenerator.hpp>

#include <vtkAlgorithmOutput.h>
#include <vtkPolyData.h>

namespace sight::modules::geometry
{
namespace generator
{

/**
 * @brief Generates a cylinder shape into a data::Mesh using VTK.
 *
 * @section Slots Slots
 * - \b updateHeight(): Update the height of the cylinder.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service uid="cylinderGenerator" type="::opVTKMesh::SNeedle" >
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
 * - \b mesh [sight::data::Mesh]: generated mesh.
 */
class MODULE_GEOMETRY_GENERATOR_CLASS_API SNeedle final : public services::IGenerator
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SNeedle, services::IGenerator)

    /// Initializes slots.
    MODULE_GEOMETRY_GENERATOR_API SNeedle() noexcept;

    /// Destroys the service.
    MODULE_GEOMETRY_GENERATOR_API ~SNeedle() noexcept override;

protected:

    /// Configures the service's parameters.
    MODULE_GEOMETRY_GENERATOR_API void configuring() override;

    /// Does nothing.
    MODULE_GEOMETRY_GENERATOR_API void starting() override;

    /// Does nothing.
    MODULE_GEOMETRY_GENERATOR_API void stopping() override;

    /// Generate the cylinder.
    MODULE_GEOMETRY_GENERATOR_API void updating() override;

private:

    /// Updates the height of the cylinder.
    void updateHeight(double);

    /// radius of the cylinder.
    double m_radius{1.0};

    /// height of the cylinder.
    double m_height{10.0};

    /// number of facets.
    int m_resolution{42};

    /// offset.
    bool m_offsetToOrigin{false};

    /// Needle mode or cylinder mode.
    bool m_needleMode{false};

    /// Needle base color.
    unsigned char m_needleColor[4]{255, 255, 255, 255};

    /// Needle minor steps color.
    unsigned char m_needleMinorStepsColor[4]{175, 175, 175, 255};

    /// Needle minor steps length.
    double m_needleMinorStepsLength{10.0};

    /// Needle major steps color.
    unsigned char m_needleMajorStepsColor[4]{255, 0, 0, 255};

    /// Needle major steps, i.e. put a major step every 5 minor steps by default.
    unsigned int m_needleMajorSteps{5};

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
    template<class T> vtkSmartPointer<T> constructSourceObject(double _height, double _center);

    /**
     * \brief Filter and color an object through its source algorithm (i.e. source->GetOutputPort()).
     *
     * \param _sourceAlgorithm Source algorithm of the object that needs to be filtered and colored.
     * \param _rgba Needed RGBA color to apply on the object.
     *
     * \return vtkSmartPointer on a vtkPolyData that handles the geometry of the filtered and colored object.
     */
    vtkSmartPointer<vtkPolyData> filterAndColorSourceObject(vtkAlgorithmOutput* _sourceAlgorithm,
                                                            const unsigned char _rgba[4]);

    /**
     * \brief Generates a torus in the right needle axis.
     *
     * \param _center Center of the object to move it in the 3D scene.
     * \param _rgba Needed RGBA color to apply on the torus.
     *
     * \return vtkSmartPointer on a vtkPolyData that handles the geometry of the torus.
     */
    vtkSmartPointer<vtkPolyData> generateTorus(double _center, const unsigned char _rgba[4]);
};

} // namespace generator

} // namespace sight::modules::geometry
