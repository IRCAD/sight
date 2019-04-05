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

#include <fwRenderVTK/vtk/Instantiator.hpp>

#include <vtkObjectFactory.h>

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
    // Try to instantiate our vtk object by names.

    vtkObject* o;
    o = vtkObjectFactory::CreateInstance("fwVtkCellPicker");
    CPPUNIT_ASSERT_MESSAGE("Unable to instantiate a fwVtkCellPicker", o);
    o->Delete();

    o = vtkObjectFactory::CreateInstance("InteractorStyle2DForNegato");
    CPPUNIT_ASSERT_MESSAGE("Unable to instantiate a InteractorStyle2DForNegato", o);
    o->Delete();

    o = vtkObjectFactory::CreateInstance("InteractorStyle3DForNegato");
    CPPUNIT_ASSERT_MESSAGE("Unable to instantiate a InteractorStyle3DForNegato", o);
    o->Delete();

    o = vtkObjectFactory::CreateInstance("FixedInteractorStyle");
    CPPUNIT_ASSERT_MESSAGE("Unable to instantiate a FixedInteractorStyle", o);
    o->Delete();
}

// ----------------------------------------------------------------------------

} // namespace ut

} // namespace fwRenderVTK
