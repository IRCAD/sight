/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "modules/filter/mesh/action/SMeshCreation.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>

#include <io/vtk/helper/Mesh.hpp>
#include <io/vtk/vtk.hpp>

#include <boost/lexical_cast.hpp>

#include <vtkDecimatePro.h>
#include <vtkDiscreteMarchingCubes.h>
#include <vtkGeometryFilter.h>
#include <vtkImageData.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkThreshold.h>
#include <vtkWindowedSincPolyDataFilter.h>

namespace sight::module::filter::mesh
{

namespace action
{

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

SMeshCreation::SMeshCreation() noexcept :
    m_reduction(0)
{
}

//-----------------------------------------------------------------------------

SMeshCreation::~SMeshCreation() noexcept
{
}

//-----------------------------------------------------------------------------

void SMeshCreation::starting()
{
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void SMeshCreation::stopping()
{
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void SMeshCreation::configuring()
{
    this->initialize();

    if(m_configuration->findConfigurationElement("percentReduction")
       && m_configuration->findConfigurationElement("percentReduction")->hasAttribute("value"))
    {
        std::string reduce = m_configuration->findConfigurationElement("percentReduction")->getExistingAttributeValue(
            "value"
        );
        m_reduction = boost::lexical_cast<unsigned int>(reduce);
    }

    SIGHT_INFO("Reduction value = " << m_reduction);
}

//-----------------------------------------------------------------------------

void SMeshCreation::updating()
{
    /// Retrieve objects
    const auto pImage = m_image.lock();
    SIGHT_ASSERT("'image' key not found", pImage);
    auto pMesh = m_mesh.lock();
    SIGHT_ASSERT("'mesh' key not found", pMesh);

    ///VTK Mesher

    // vtk img
    vtkSmartPointer<vtkImageData> vtkImage = vtkSmartPointer<vtkImageData>::New();
    io::vtk::toVTKImage(pImage.get_shared(), vtkImage);

    // contour filter
    vtkSmartPointer<vtkDiscreteMarchingCubes> contourFilter = vtkSmartPointer<vtkDiscreteMarchingCubes>::New();
    contourFilter->SetInputData(vtkImage);
    contourFilter->SetValue(0, 255);
    contourFilter->ComputeScalarsOn();
    contourFilter->ComputeNormalsOn();
    contourFilter->Update();

    // smooth filter
    vtkSmartPointer<vtkWindowedSincPolyDataFilter> smoothFilter =
        vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
    smoothFilter->SetInputConnection(contourFilter->GetOutputPort());
    smoothFilter->SetNumberOfIterations(50);
    smoothFilter->BoundarySmoothingOn();
    smoothFilter->SetPassBand(0.1);
    smoothFilter->SetFeatureAngle(120.0);
    smoothFilter->SetEdgeAngle(90);
    smoothFilter->FeatureEdgeSmoothingOn();
    smoothFilter->Update();

    // Get polyData
    vtkSmartPointer<vtkPolyData> polyData;

    // decimate filter
    unsigned int reduction = m_reduction;
    if(reduction > 0)
    {
        vtkSmartPointer<vtkDecimatePro> decimate = vtkSmartPointer<vtkDecimatePro>::New();
        decimate->SetInputConnection(smoothFilter->GetOutputPort());
        decimate->SetTargetReduction(reduction / 100.0);
        decimate->PreserveTopologyOff();
        decimate->SplittingOn();
        decimate->BoundaryVertexDeletionOn();
        decimate->SetSplitAngle(120);
        decimate->Update();
        polyData = decimate->GetOutput();
        io::vtk::helper::Mesh::fromVTKMesh(polyData, pMesh.get_shared());
    }
    else
    {
        polyData = smoothFilter->GetOutput();
        io::vtk::helper::Mesh::fromVTKMesh(polyData, pMesh.get_shared());
    }

    /// Notification
    data::Object::ModifiedSignalType::sptr sig;
    sig = pMesh->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

} // namespace action

} // namespace sight::module::filter::mesh
