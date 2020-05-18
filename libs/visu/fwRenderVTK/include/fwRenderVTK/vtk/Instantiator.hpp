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

#include <fwCore/spyLog.hpp>

#include <vtkObjectFactory.h>
#include <vtkVersion.h>

#include <string>
#include <vector>

namespace fwRenderVTK
{

namespace vtk
{

/**
 * @brief The Instantiator is a vtk factory object used to instantiate our sight-vtk object.
 * Registering a sight-vtk object or a vtk object is only needed when it's instantiated from string representing its
 * name (e.g. when using it in xml).
 */
class FWRENDERVTK_CLASS_API Instantiator : public vtkObjectFactory
{

public:
    /// Constructor: performs the registration in the factory.
    FWRENDERVTK_API Instantiator();

    /// Creates the object.
    FWRENDERVTK_API static Instantiator* New()
    {
        Instantiator* f = new Instantiator;
        f->InitializeObjectBase();
        return f;
    }
    /// Overrides.
    FWRENDERVTK_API const char* GetVTKSourceVersion() override
    {
        return VTK_SOURCE_VERSION;
    }
    /// Overrrides.
    FWRENDERVTK_API const char* GetDescription() override
    {
        return "The sight-vtk factory (fwRenderVTK).";
    }

    /**
     * @brief Returns a vector of class name registred into the factory.
     * Also prints it into log output when log level is DEBUG.
     * @return std::vector<std::string>
     */
    FWRENDERVTK_API std::vector< std::string > getClassOverrides()
    {
        const int nb = this->GetNumberOfOverrides();

        std::vector< std::string > classOverrides;
        classOverrides.resize(static_cast<size_t>(nb));

        std::stringstream log;
        log << "sight-vtk factory can build: "<< nb <<" classes;"<< std::endl;

        for(int i = 0; i < nb; ++i)
        {
            classOverrides[static_cast<size_t>(i)] = std::string(this->GetClassOverrideName(i));

            log << " - " + classOverrides[static_cast<size_t>(i)] << std::endl;
        }

        SLM_DEBUG(log.str());
        return classOverrides;
    }
};

} //namespace vtk

} //namespace fwRenderVTK
