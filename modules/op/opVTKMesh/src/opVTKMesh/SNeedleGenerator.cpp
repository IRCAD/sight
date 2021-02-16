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

#include "opVTKMesh/SNeedleGenerator.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Mesh.hpp>

#include <fwDataTools/Color.hpp>
#include <fwDataTools/Mesh.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/helper/Mesh.hpp>

#include <vtkAppendPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricTorus.h>
#include <vtkPointData.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkUnsignedCharArray.h>

namespace opVTKMesh
{

fwServicesRegisterMacro( ::opVTKMesh::IGenerator, ::opVTKMesh::SNeedleGenerator, ::fwData::Mesh)

const ::fwCom::Slots::SlotKeyType s_UPDATE_HEIGHT = "updateHeight";

// ------------------------------------------------------------------------------

SNeedleGenerator::SNeedleGenerator() noexcept
{
    newSlot(s_UPDATE_HEIGHT, &SNeedleGenerator::updateHeight, this);
}

// ------------------------------------------------------------------------------

SNeedleGenerator::~SNeedleGenerator() noexcept
{
}

// ------------------------------------------------------------------------------

void SNeedleGenerator::configuring()
{
    const auto configTree = this->getConfigTree();

    const auto config = configTree.get_child("config.<xmlattr>");

    // Default cylinder configuration
    m_radius     = config.get<double>("radius", m_radius);
    m_height     = config.get<double>("height", m_height);
    m_resolution = config.get<int>("resolution", m_resolution);
    const std::string offsetToOriginStr = config.get<std::string>("offsetToOrigin", "false");
    m_offsetToOrigin = (offsetToOriginStr == "yes" || offsetToOriginStr == "true" || offsetToOriginStr == "on");

    // Full optional needle configuration
    const auto needleConfigTree = configTree.get_child_optional("needle");
    if(needleConfigTree.is_initialized())
    {
        m_needleMode = true;

        const auto needleConfig = configTree.get_child("needle.<xmlattr>");

        const std::string needleColor = needleConfig.get<std::string>("color", "");
        if(!needleColor.empty())
        {
            SLM_ASSERT( "Color string should start with '#' and followed by 6 ou 8 "
                        "hexadecimal digits. Given color : " << needleColor,
                        needleColor [0] == '#'
                        && ( needleColor.length() == 7 || needleColor.length() == 9)
                        );
            ::fwDataTools::Color::hexaStringToRGBA(needleColor, m_needleColor);
        }

        const auto minorStepsConfigTree = (*needleConfigTree).get_child_optional("minorSteps");
        if(minorStepsConfigTree.is_initialized())
        {
            const auto minorStepsConfig = (*needleConfigTree).get_child("minorSteps.<xmlattr>");

            const std::string minorStepsColor = minorStepsConfig.get<std::string>("color", "");
            if(!minorStepsColor.empty())
            {
                SLM_ASSERT( "Color string should start with '#' and followed by 6 ou 8 "
                            "hexadecimal digits. Given color : " << minorStepsColor,
                            minorStepsColor [0] == '#'
                            && ( minorStepsColor.length() == 7 || minorStepsColor.length() == 9)
                            );
                ::fwDataTools::Color::hexaStringToRGBA(minorStepsColor, m_needleMinorStepsColor);
            }

            m_needleMinorStepsLength = minorStepsConfig.get<double>("length", m_needleMinorStepsLength);
        }

        const auto majorStepsConfigTree = (*needleConfigTree).get_child_optional("majorSteps");
        if(majorStepsConfigTree.is_initialized())
        {
            const auto majorStepsConfig = (*needleConfigTree).get_child("majorSteps.<xmlattr>");

            const std::string majorStepsColor = majorStepsConfig.get<std::string>("color", "");
            if(!majorStepsColor.empty())
            {
                SLM_ASSERT( "Color string should start with '#' and followed by 6 ou 8 "
                            "hexadecimal digits. Given color : " << majorStepsColor,
                            majorStepsColor [0] == '#'
                            && ( majorStepsColor.length() == 7 || majorStepsColor.length() == 9)
                            );
                ::fwDataTools::Color::hexaStringToRGBA(majorStepsColor, m_needleMajorStepsColor);
            }

            m_needleMajorSteps = majorStepsConfig.get<unsigned int>("steps", m_needleMajorSteps);
        }
    }
}

// ------------------------------------------------------------------------------

void SNeedleGenerator::starting()
{
}

// ------------------------------------------------------------------------------

void SNeedleGenerator::stopping()
{
}

// ------------------------------------------------------------------------------

void SNeedleGenerator::updating()
{
    vtkSmartPointer<vtkPolyData> vtkMesh;

    if(m_needleMode)
    {
        vtkMesh = constructNeedle();
    }
    else // Default cylinder construction
    {
        double center                               = (m_offsetToOrigin ? m_height/2.0 : 0.0);
        vtkSmartPointer<vtkCylinderSource> cylinder = constructSourceObject<vtkCylinderSource>(m_height, center);

        vtkSmartPointer<vtkTriangleFilter> triangleFilter = vtkSmartPointer< vtkTriangleFilter >::New();
        triangleFilter->SetInputConnection(cylinder->GetOutputPort());
        triangleFilter->Update();

        vtkMesh = triangleFilter->GetOutput();
    }

    ::fwData::Mesh::sptr mesh = this->getInOut< ::fwData::Mesh >("mesh");
    ::fwVtkIO::helper::Mesh::fromVTKMesh(vtkMesh, mesh);

    ::fwData::Object::ModifiedSignalType::sptr sig;
    sig = mesh->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
}

// ------------------------------------------------------------------------------

vtkSmartPointer<vtkPolyData> SNeedleGenerator::constructNeedle()
{
    // Number of cylinder regarding their needed length
    const double nbOfCylinders = m_height/m_needleMinorStepsLength;
    // Entire part
    const int nbOfEntireParts = floor(nbOfCylinders);
    // Decimal part
    const double restOfCylinder = m_needleMinorStepsLength * (nbOfCylinders - nbOfEntireParts);

    // Appender object to append cylinders, torus and cone to generate a needle
    vtkSmartPointer<vtkAppendPolyData> appender = vtkSmartPointer< vtkAppendPolyData >::New();

    double center;
    // Sweep the needle along its axis without the tip cylinder that will be replaced by a cone
    for(int cylinderIndex = (nbOfEntireParts-2); cylinderIndex >= 0; --cylinderIndex)
    {
        center = (m_needleMinorStepsLength/2.0) + (m_needleMinorStepsLength * cylinderIndex);

        // Put a torus every "m_needleMajorSteps" minor steps (for example every 5 minor steps by default) and not on
        // the end of the needle
        if(((nbOfEntireParts - cylinderIndex ) % m_needleMajorSteps) == 0 && cylinderIndex != 0)
        {
            // Move the center from half the minor step length to get the torus on the edge of two cylinders
            const double torusCenter              = center - m_needleMinorStepsLength/2.0;
            vtkSmartPointer<vtkPolyData> polyData = generateTorus(torusCenter, m_needleMajorStepsColor);

            appender->AddInputData(polyData);
        }

        vtkSmartPointer<vtkCylinderSource> cylinder = constructSourceObject<vtkCylinderSource>(m_needleMinorStepsLength,
                                                                                               center);

        vtkSmartPointer<vtkPolyData> polyData;

        if((cylinderIndex % 2) == 0)
        {
            polyData = filterAndColorSourceObject(cylinder->GetOutputPort(), m_needleColor);
        }
        else
        {
            polyData = filterAndColorSourceObject(cylinder->GetOutputPort(), m_needleMinorStepsColor);
        }

        appender->AddInputData(polyData);
    }

    // Replace the last cylinder by a cone to mimic a needle
    // Compute the rest of the needle to get the cone length
    const double height = m_needleMinorStepsLength + restOfCylinder;
    // Compute the new center of this cone
    center = (m_needleMinorStepsLength * (nbOfEntireParts-1)) + (restOfCylinder/2.0) + (m_needleMinorStepsLength/2.0);

    vtkSmartPointer<vtkConeSource> cone = constructSourceObject<vtkConeSource>(height, center);
    // Put the cone in the right direction and remove its bottom cap (avoiding ugly effects because of wrong normal
    // computations)
    cone->SetDirection(0, 1, 0);
    cone->SetCapping(false);
    vtkSmartPointer<vtkPolyData> polyData = filterAndColorSourceObject(cone->GetOutputPort(), m_needleColor);
    appender->AddInputData(polyData);
    appender->Update();

    // The needle is generated as if the offsetToOrigin parameter was set to true. If the offset is not needed,
    // translate it
    // along its axis and half its height distance
    if(!m_offsetToOrigin)
    {
        vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
        transform->Translate(0.0, -m_height/2.0, 0.0);

        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
            vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        transformFilter->SetInputConnection(appender->GetOutputPort());
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        return transformFilter->GetOutput();
    }

    return appender->GetOutput();
}

// ------------------------------------------------------------------------------

template<class T> vtkSmartPointer<T> SNeedleGenerator::constructSourceObject(double _height, double _center)
{
    vtkSmartPointer<T> source = vtkSmartPointer<T>::New();
    source->SetRadius(m_radius);
    source->SetHeight(_height);
    source->SetResolution(m_resolution);
    source->SetCenter(0.0, _center, 0.0);

    return source;
}

// ------------------------------------------------------------------------------

vtkSmartPointer<vtkPolyData> SNeedleGenerator::filterAndColorSourceObject(vtkAlgorithmOutput* _sourceAlgorithm,
                                                                          const unsigned char _rgba[4])
{
    // vtkXxxSource give us a polyData with a POLYGON cell type
    // Thus we use a vtkTriangleFilter
    vtkSmartPointer<vtkTriangleFilter> triangleFilter = vtkSmartPointer< vtkTriangleFilter >::New();
    triangleFilter->SetInputConnection(_sourceAlgorithm);
    triangleFilter->Update();

    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->ShallowCopy(triangleFilter->GetOutput());

    vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer< vtkUnsignedCharArray >::New();
    colors->SetName("Colors");
    colors->SetNumberOfComponents(4);
    colors->SetNumberOfTuples(polyData->GetNumberOfPoints());
    colors->Fill(0);
    for(std::uint8_t i = 0; i < 4; ++i)
    {
        colors->FillTypedComponent(i, _rgba[i]);
    }

    polyData->GetPointData()->SetScalars(colors);

    return polyData;
}

// ------------------------------------------------------------------------------

vtkSmartPointer<vtkPolyData> SNeedleGenerator::generateTorus(double _center, const unsigned char _rgba[4])
{
    vtkSmartPointer<vtkParametricTorus> torus = vtkSmartPointer<vtkParametricTorus>::New();

    torus->SetRingRadius(m_radius);
    torus->SetCrossSectionRadius(m_radius/4);

    vtkSmartPointer<vtkParametricFunctionSource> parametricFunctionSource =
        vtkSmartPointer<vtkParametricFunctionSource>::New();
    parametricFunctionSource->SetParametricFunction(torus);

    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->RotateX(-90.0);
    transform->Translate(0.0, 0.0, _center);

    vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformFilter->SetInputConnection(parametricFunctionSource->GetOutputPort());
    transformFilter->SetTransform(transform);
    transformFilter->Update();

    vtkSmartPointer<vtkPolyData> polyData = filterAndColorSourceObject(transformFilter->GetOutputPort(), _rgba);

    return polyData;
}

// ------------------------------------------------------------------------------

void SNeedleGenerator::updateHeight(double height)
{
    m_height = height;
    this->updating();
}

// ------------------------------------------------------------------------------

} // namespace opVTKMesh.
