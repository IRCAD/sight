/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwRenderVTK/config.hpp"

#include <vtkObjectFactory.h>
#include <vtkVersion.h>

namespace fwRenderVTK
{

namespace vtk
{

/**
 * @brief The Instantiator is a vtk factory object used to instantiate our sight-vtk object.
 * You only need to register your sight-vtk object in the factory when the object will be
 * instantiate from it's name in string (for example in xml).
 */
class FWRENDERVTK_CLASS_API Instantiator : public vtkObjectFactory
{

public:
    /// Constructor: performs the registration in the factory.
    Instantiator();

    /// Creates the object.
    static Instantiator* New()
    {
        Instantiator* f = new Instantiator;
        f->InitializeObjectBase();
        return f;
    }
    /// Overrides.
    const char* GetVTKSourceVersion() override
    {
        return VTK_SOURCE_VERSION;
    }
    /// Overrrides.
    const char* GetDescription() override
    {
        return "The sight-vtk factory (fwRenderVTK).";
    }
};

} //namespace vtk

} //namespace fwRenderVTK
