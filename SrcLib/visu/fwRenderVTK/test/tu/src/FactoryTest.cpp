/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "FactoryTest.hpp"

#include <fwRenderVTK/vtk/FixedInteractorStyle.hpp>
#include <fwRenderVTK/vtk/fwHandleRepresentation3D.hpp>
#include <fwRenderVTK/vtk/fwVtkAxesActor.hpp>
#include <fwRenderVTK/vtk/fwVtkBoxRepresentation.hpp>
#include <fwRenderVTK/vtk/fwVtkCellPicker.hpp>
#include <fwRenderVTK/vtk/fwVtkPicker.hpp>
#include <fwRenderVTK/vtk/fwVtkWheelRepresentation.hpp>
#include <fwRenderVTK/vtk/fwVtkWheelWidget.hpp>
#include <fwRenderVTK/vtk/fwVtkWindowLevelLookupTable.hpp>
#include <fwRenderVTK/vtk/Instantiator.hpp>
#include <fwRenderVTK/vtk/InteractorStyle2DForNegato.hpp>
#include <fwRenderVTK/vtk/InteractorStyle3DForNegato.hpp>
#include <fwRenderVTK/vtk/MarkedSphereHandleRepresentation.hpp>

#include <vtkCellPicker.h>
#include <vtkImageBlend.h>
#include <vtkImageCheckerboard.h>
#include <vtkObjectFactory.h>
#include <vtkPlaneCollection.h>
#include <vtkTransform.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwRenderVTK::ut::FactoryTest );

namespace fwRenderVTK
{

namespace ut
{
// ----------------------------------------------------------------------------

void FactoryTest::setUp()
{
    // Register our factory.
    m_factory = ::fwRenderVTK::vtk::Instantiator::New();
    vtkObjectFactory::RegisterFactory(m_factory);
}

// ----------------------------------------------------------------------------

void FactoryTest::tearDown()
{
    vtkObjectFactory::UnRegisterFactory(m_factory);
    m_factory->Delete();
}

// ----------------------------------------------------------------------------

void FactoryTest::instantiationTest()
{
    // Obviously this needs to be updated each time a class is registered into the factory.
    const size_t trueNbOverrides = 17;

    // Check how many "overrides" are registered into the factory
    const auto nb  = m_factory->GetNumberOfOverrides(); // VTK function.
    const auto vec = m_factory->getClassOverrides(); // Sight function.

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of overrides doesn't match", trueNbOverrides, static_cast<size_t>(nb) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of overrides doesn't match", trueNbOverrides, vec.size() );

    // Try to instantiate our vtk object by names.
    vtkObject* o;
    o = vtkObjectFactory::CreateInstance("fwVtkCellPicker");
    CPPUNIT_ASSERT_MESSAGE("Unable to instantiate a fwVtkCellPicker", o);
    auto fwVtkCellPicker = fwRenderVTK::vtk::fwVtkCellPicker::SafeDownCast(o);
    CPPUNIT_ASSERT_MESSAGE("Unable to downCast to fwVtkCellPicker", fwVtkCellPicker);
    o->Delete();

    o = vtkObjectFactory::CreateInstance("InteractorStyle2DForNegato");
    CPPUNIT_ASSERT_MESSAGE("Unable to instantiate a InteractorStyle2DForNegato", o);
    auto interactorStyle2DForNegato = fwRenderVTK::vtk::InteractorStyle2DForNegato::SafeDownCast(o);
    CPPUNIT_ASSERT_MESSAGE("Unable to downCast to InteractorStyle2DForNegato", interactorStyle2DForNegato);
    o->Delete();

    o = vtkObjectFactory::CreateInstance("InteractorStyle3DForNegato");
    CPPUNIT_ASSERT_MESSAGE("Unable to instantiate a InteractorStyle3DForNegato", o);
    auto interactorStyle3DForNegato = fwRenderVTK::vtk::InteractorStyle3DForNegato::SafeDownCast(o);
    CPPUNIT_ASSERT_MESSAGE("Unable to downCast to InteractorStyle3DForNegato", interactorStyle3DForNegato);
    o->Delete();

    o = vtkObjectFactory::CreateInstance("FixedInteractorStyle");
    CPPUNIT_ASSERT_MESSAGE("Unable to instantiate a FixedInteractorStyle", o);
    auto fixedInteractorStyle = fwRenderVTK::vtk::FixedInteractorStyle::SafeDownCast(o);
    CPPUNIT_ASSERT_MESSAGE("Unable to downCast to FixedInteractorStyle", fixedInteractorStyle);
    o->Delete();

    o = vtkObjectFactory::CreateInstance("fwHandleRepresentation3D");
    CPPUNIT_ASSERT_MESSAGE("Unable to instantiate a fwHandleRepresentation3D", o);
    auto fwHandleRepresentation3D = fwRenderVTK::vtk::fwHandleRepresentation3D::SafeDownCast(o);
    CPPUNIT_ASSERT_MESSAGE("Unable to downCast to fwHandleRepresentation3D", fwHandleRepresentation3D);
    o->Delete();

    o = vtkObjectFactory::CreateInstance("fwVtkAxesActor");
    CPPUNIT_ASSERT_MESSAGE("Unable to instantiate a fwVtkAxesActor", o);
    auto fwVtkAxesActor = fwRenderVTK::vtk::fwVtkAxesActor::SafeDownCast(o);
    CPPUNIT_ASSERT_MESSAGE("Unable to downCast to fwVtkAxesActor", fwVtkAxesActor);
    o->Delete();

    o = vtkObjectFactory::CreateInstance("fwVtkBoxRepresentation");
    CPPUNIT_ASSERT_MESSAGE("Unable to instantiate a fwVtkBoxRepresentation", o);
    auto fwVtkBoxRepresentation = fwRenderVTK::vtk::fwVtkBoxRepresentation::SafeDownCast(o);
    CPPUNIT_ASSERT_MESSAGE("Unable to downCast to fwVtkBoxRepresentation", fwVtkBoxRepresentation);
    o->Delete();

    o = vtkObjectFactory::CreateInstance("fwVtkPicker");
    CPPUNIT_ASSERT_MESSAGE("Unable to instantiate a fwVtkPicker", o);
    auto fwVtkPicker = fwRenderVTK::vtk::fwVtkPicker::SafeDownCast(o);
    CPPUNIT_ASSERT_MESSAGE("Unable to downCast to fwVtkPicker", fwVtkPicker);
    o->Delete();

    o = vtkObjectFactory::CreateInstance("fwVtkWheelRepresentation");
    CPPUNIT_ASSERT_MESSAGE("Unable to instantiate a fwVtkWheelRepresentation", o);
    auto fwVtkWheelRepresentation = fwRenderVTK::vtk::fwVtkWheelRepresentation::SafeDownCast(o);
    CPPUNIT_ASSERT_MESSAGE("Unable to downCast to fwVtkWheelRepresentation", fwVtkWheelRepresentation);
    o->Delete();

    o = vtkObjectFactory::CreateInstance("fwVtkWheelWidget");
    CPPUNIT_ASSERT_MESSAGE("Unable to instantiate a fwVtkWheelWidget", o);
    auto fwVtkWheelWidget = fwRenderVTK::vtk::fwVtkWheelWidget::SafeDownCast(o);
    CPPUNIT_ASSERT_MESSAGE("Unable to downCast to fwVtkWheelWidget", fwVtkWheelWidget);
    o->Delete();

    o = vtkObjectFactory::CreateInstance("fwVtkWindowLevelLookupTable");
    CPPUNIT_ASSERT_MESSAGE("Unable to instantiate a fwVtkWindowLevelLookupTable", o);
    auto fwVtkWindowLevelLookupTable = fwRenderVTK::vtk::fwVtkWindowLevelLookupTable::SafeDownCast(o);
    CPPUNIT_ASSERT_MESSAGE("Unable to downCast to fwVtkWindowLevelLookupTable", fwVtkWindowLevelLookupTable);
    o->Delete();

    o = vtkObjectFactory::CreateInstance("MarkedSphereHandleRepresentation");
    CPPUNIT_ASSERT_MESSAGE("Unable to instantiate a MarkedSphereHandleRepresentation", o);
    auto MarkedSphereHandleRepresentation = fwRenderVTK::vtk::MarkedSphereHandleRepresentation::SafeDownCast(o);
    CPPUNIT_ASSERT_MESSAGE("Unable to downCast to MarkedSphereHandleRepresentation", MarkedSphereHandleRepresentation);
    o->Delete();

    // Try also to instantiate vtk class that we need in our code.
    o = vtkObjectFactory::CreateInstance("vtkImageBlend");
    CPPUNIT_ASSERT_MESSAGE("Unable to instantiate a vtkImageBlend", o);
    auto vtkImageBlend = vtkImageBlend::SafeDownCast(o);
    CPPUNIT_ASSERT_MESSAGE("Unable to downCast to vtkImageBlend", vtkImageBlend);
    o->Delete();

    o = vtkObjectFactory::CreateInstance("vtkImageCheckerboard");
    CPPUNIT_ASSERT_MESSAGE("Unable to instantiate a vtkImageCheckerboard", o);
    auto vtkImageCheckerboard = vtkImageCheckerboard::SafeDownCast(o);
    CPPUNIT_ASSERT_MESSAGE("Unable to downCast to vtkImageCheckerboard", vtkImageCheckerboard);
    o->Delete();

    o = vtkObjectFactory::CreateInstance("vtkTransform");
    CPPUNIT_ASSERT_MESSAGE("Unable to instantiate a vtkTransform", o);
    auto vtkTransform = vtkTransform::SafeDownCast(o);
    CPPUNIT_ASSERT_MESSAGE("Unable to downCast to vtkTransform", vtkTransform);
    o->Delete();

    o = vtkObjectFactory::CreateInstance("vtkPlaneCollection");
    CPPUNIT_ASSERT_MESSAGE("Unable to instantiate a vtkPlaneCollection", o);
    auto vtkPlaneCollection = vtkPlaneCollection::SafeDownCast(o);
    CPPUNIT_ASSERT_MESSAGE("Unable to downCast to vtkPlaneCollection", vtkPlaneCollection);
    o->Delete();

    o = vtkObjectFactory::CreateInstance("vtkCellPicker");
    CPPUNIT_ASSERT_MESSAGE("Unable to instantiate a vtkCellPicker", o);
    auto vtkCellPicker = vtkCellPicker::SafeDownCast(o);
    CPPUNIT_ASSERT_MESSAGE("Unable to downCast to vtkCellPicker", vtkCellPicker);
    o->Delete();

}

// ----------------------------------------------------------------------------

} // namespace ut

} // namespace fwRenderVTK
