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

#include "needle.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/color.hpp>
#include <data/tools/color.hpp>

#include <geometry/data/mesh.hpp>

#include <io/vtk/helper/mesh.hpp>

#include <service/macros.hpp>

#include <vtkAppendPolyData.h>
#include <vtkCellData.h>
#include <vtkCleanPolyData.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricTorus.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkUnsignedCharArray.h>

#include <cmath>

namespace sight::module::filter::mesh::generator
{

const core::com::slots::key_t UPDATE_HEIGHT = "updateHeight";

// ------------------------------------------------------------------------------

needle::needle() noexcept
{
    new_slot(UPDATE_HEIGHT, &needle::update_height, this);
}

// ------------------------------------------------------------------------------

needle::~needle() noexcept =
    default;

// ------------------------------------------------------------------------------

void needle::configuring()
{
    const auto config_tree = this->get_config();

    const auto config = config_tree.get_child("config.<xmlattr>");

    // Default cylinder configuration
    m_radius           = config.get<double>("radius", m_radius);
    m_height           = config.get<double>("height", m_height);
    m_resolution       = config.get<int>("resolution", m_resolution);
    m_offset_to_origin = config.get<bool>("offsetToOrigin", false);

    // Full optional needle configuration
    const auto needle_config_tree = config_tree.get_child_optional("needle");
    if(needle_config_tree.is_initialized())
    {
        m_needle_mode = true;

        const auto needle_config = config_tree.get_child("needle.<xmlattr>");

        const std::string needle_color = needle_config.get<std::string>("color", "");
        if(!needle_color.empty())
        {
            SIGHT_ASSERT(
                "Color string should start with '#' and followed by 6 ou 8 "
                "hexadecimal digits. Given color : " << needle_color,
                needle_color[0] == '#'
                && (needle_color.length() == 7 || needle_color.length() == 9)
            );
            data::tools::color::hexa_string_to_rgba(needle_color, m_needle_color);
        }

        const auto minor_steps_config_tree = (*needle_config_tree).get_child_optional("minorSteps");
        if(minor_steps_config_tree.is_initialized())
        {
            const auto minor_steps_config = (*needle_config_tree).get_child("minorSteps.<xmlattr>");

            const std::string minor_steps_color = minor_steps_config.get<std::string>("color", "");
            if(!minor_steps_color.empty())
            {
                SIGHT_ASSERT(
                    "Color string should start with '#' and followed by 6 ou 8 "
                    "hexadecimal digits. Given color : " << minor_steps_color,
                    minor_steps_color[0] == '#'
                    && (minor_steps_color.length() == 7 || minor_steps_color.length() == 9)
                );
                data::tools::color::hexa_string_to_rgba(minor_steps_color, m_needle_minor_steps_color);
            }

            m_needle_minor_steps_length = minor_steps_config.get<double>("length", m_needle_minor_steps_length);
        }

        const auto major_steps_config_tree = (*needle_config_tree).get_child_optional("majorSteps");
        if(major_steps_config_tree.is_initialized())
        {
            const auto major_steps_config = (*needle_config_tree).get_child("majorSteps.<xmlattr>");

            const std::string major_steps_color = major_steps_config.get<std::string>("color", "");
            if(!major_steps_color.empty())
            {
                SIGHT_ASSERT(
                    "Color string should start with '#' and followed by 6 ou 8 "
                    "hexadecimal digits. Given color : " << major_steps_color,
                    major_steps_color[0] == '#'
                    && (major_steps_color.length() == 7 || major_steps_color.length() == 9)
                );
                data::tools::color::hexa_string_to_rgba(major_steps_color, m_needle_major_steps_color);
            }

            m_needle_major_steps = major_steps_config.get<unsigned int>("steps", m_needle_major_steps);
        }
    }
}

// ------------------------------------------------------------------------------

void needle::starting()
{
}

// ------------------------------------------------------------------------------

void needle::stopping()
{
}

// ------------------------------------------------------------------------------

void needle::updating()
{
    vtkSmartPointer<vtkPolyData> vtk_mesh;

    if(m_needle_mode)
    {
        vtk_mesh = construct_needle();
    }
    else // Default cylinder construction
    {
        double center                               = (m_offset_to_origin ? m_height / 2.0 : 0.0);
        vtkSmartPointer<vtkCylinderSource> cylinder = construct_source_object<vtkCylinderSource>(m_height, center);

        vtkSmartPointer<vtkTriangleFilter> triangle_filter = vtkSmartPointer<vtkTriangleFilter>::New();
        triangle_filter->SetInputConnection(cylinder->GetOutputPort());
        triangle_filter->Update();

        vtk_mesh = triangle_filter->GetOutput();
    }

    auto mesh = m_mesh.lock();
    SIGHT_ASSERT("No 'mesh' found.", mesh);
    io::vtk::helper::mesh::from_vtk_mesh(vtk_mesh, mesh.get_shared());

    data::object::modified_signal_t::sptr sig;
    sig = mesh->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    sig->async_emit();
}

// ------------------------------------------------------------------------------

vtkSmartPointer<vtkPolyData> needle::construct_needle()
{
    // Number of cylinder regarding their needed length
    const double nb_of_cylinders = m_height / m_needle_minor_steps_length;
    // Entire part
    const int nb_of_entire_parts = int(nb_of_cylinders);
    // Decimal part
    const double rest_of_cylinder = m_needle_minor_steps_length * (nb_of_cylinders - nb_of_entire_parts);

    // Appender object to append cylinders, torus and cone to generate a needle
    vtkSmartPointer<vtkAppendPolyData> appender = vtkSmartPointer<vtkAppendPolyData>::New();

    double center = NAN;
    // Sweep the needle along its axis without the tip cylinder that will be replaced by a cone
    for(int cylinder_index = (nb_of_entire_parts - 2) ; cylinder_index >= 0 ; --cylinder_index)
    {
        center = (m_needle_minor_steps_length / 2.0) + (m_needle_minor_steps_length * cylinder_index);

        // Put a torus every "m_needleMajorSteps" minor steps (for example every 5 minor steps by default) and not on
        // the end of the needle
        if((unsigned(nb_of_entire_parts - cylinder_index) % m_needle_major_steps) == 0 && cylinder_index != 0)
        {
            // Move the center from half the minor step length to get the torus on the edge of two cylinders
            const double torus_center              = center - m_needle_minor_steps_length / 2.0;
            vtkSmartPointer<vtkPolyData> poly_data = generate_torus(torus_center, m_needle_major_steps_color);

            appender->AddInputData(poly_data);
        }

        vtkSmartPointer<vtkCylinderSource> cylinder = construct_source_object<vtkCylinderSource>(
            m_needle_minor_steps_length,
            center
        );

        vtkSmartPointer<vtkPolyData> poly_data;

        if((cylinder_index % 2) == 0)
        {
            poly_data = filter_and_color_source_object(cylinder->GetOutputPort(), m_needle_color);
        }
        else
        {
            poly_data = filter_and_color_source_object(cylinder->GetOutputPort(), m_needle_minor_steps_color);
        }

        appender->AddInputData(poly_data);
    }

    // Replace the last cylinder by a cone to mimic a needle
    // Compute the rest of the needle to get the cone length
    const double height = m_needle_minor_steps_length + rest_of_cylinder;
    // Compute the new center of this cone
    center = (m_needle_minor_steps_length * (nb_of_entire_parts - 1)) + (rest_of_cylinder / 2.0)
             + (m_needle_minor_steps_length / 2.0);

    vtkSmartPointer<vtkConeSource> cone = construct_source_object<vtkConeSource>(height, center);
    // Put the cone in the right direction and remove its bottom cap (avoiding ugly effects because of wrong normal
    // computations)
    cone->SetDirection(0, 1, 0);
    cone->SetCapping(0);
    vtkSmartPointer<vtkPolyData> poly_data = filter_and_color_source_object(cone->GetOutputPort(), m_needle_color);
    appender->AddInputData(poly_data);
    appender->Update();

    // The needle is generated as if the offsetToOrigin parameter was set to true. If the offset is not needed,
    // translate it
    // along its axis and half its height distance
    if(!m_offset_to_origin)
    {
        vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
        transform->Translate(0.0, -m_height / 2.0, 0.0);

        vtkSmartPointer<vtkTransformPolyDataFilter> transform_filter =
            vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        transform_filter->SetInputConnection(appender->GetOutputPort());
        transform_filter->SetTransform(transform);
        transform_filter->Update();

        return transform_filter->GetOutput();
    }

    return appender->GetOutput();
}

// ------------------------------------------------------------------------------

template<class T>
vtkSmartPointer<T> needle::construct_source_object(double _height, double _center)
{
    vtkSmartPointer<T> source = vtkSmartPointer<T>::New();
    source->SetRadius(m_radius);
    source->SetHeight(_height);
    source->SetResolution(m_resolution);
    source->SetCenter(0.0, _center, 0.0);

    return source;
}

// ------------------------------------------------------------------------------

vtkSmartPointer<vtkPolyData> needle::filter_and_color_source_object(
    vtkAlgorithmOutput* _source_algorithm,
    const std::array<unsigned char, 4>& _rgba
)
{
    // vtkXxxSource give us a polyData with a POLYGON cell type
    // Thus we use a vtkTriangleFilter
    vtkSmartPointer<vtkTriangleFilter> triangle_filter = vtkSmartPointer<vtkTriangleFilter>::New();
    triangle_filter->SetInputConnection(_source_algorithm);
    triangle_filter->Update();

    vtkSmartPointer<vtkPolyData> poly_data = vtkSmartPointer<vtkPolyData>::New();
    poly_data->ShallowCopy(triangle_filter->GetOutput());

    vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colors->SetName("Colors");
    colors->SetNumberOfComponents(4);
    colors->SetNumberOfTuples(poly_data->GetNumberOfCells());
    colors->FillComponent(0, 0.);
    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        colors->FillComponent(i, _rgba[i]);
    }

    poly_data->GetCellData()->SetScalars(colors);

    return poly_data;
}

// ------------------------------------------------------------------------------

vtkSmartPointer<vtkPolyData> needle::generate_torus(double _center, const std::array<unsigned char, 4>& _rgba) const
{
    vtkSmartPointer<vtkParametricTorus> torus = vtkSmartPointer<vtkParametricTorus>::New();

    torus->SetRingRadius(m_radius);
    torus->SetCrossSectionRadius(m_radius / 4);

    vtkSmartPointer<vtkParametricFunctionSource> parametric_function_source =
        vtkSmartPointer<vtkParametricFunctionSource>::New();
    parametric_function_source->SetParametricFunction(torus);

    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->RotateX(-90.0);
    transform->Translate(0.0, 0.0, _center);

    vtkSmartPointer<vtkTransformPolyDataFilter> transform_filter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transform_filter->SetInputConnection(parametric_function_source->GetOutputPort());
    transform_filter->SetTransform(transform);
    transform_filter->Update();

    vtkSmartPointer<vtkPolyData> poly_data = filter_and_color_source_object(transform_filter->GetOutputPort(), _rgba);

    return poly_data;
}

// ------------------------------------------------------------------------------

void needle::update_height(double _height)
{
    m_height = _height;
    this->updating();
}

// ------------------------------------------------------------------------------

} // namespace sight::module::filter::mesh::generator
