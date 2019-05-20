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

#include <vtkAxesActor.h>

class vtkPropCollection;

namespace fwRenderVTK
{

namespace vtk
{

/**
 * @brief The fwVtkAxesActor class is a specific and movable vtkAxesActor
 * @see GetBounds method
 */
class FWRENDERVTK_CLASS_API fwVtkAxesActor : public vtkAxesActor
{
public:

    FWRENDERVTK_API static fwVtkAxesActor* New();

    vtkTypeMacro(fwVtkAxesActor, vtkAxesActor);
    void PrintSelf(ostream& os, vtkIndent indent) override;

    /**
     * @brief Overload of GetBounds method :
     * In vtkAxesActor Bounds are computed as 'Max' of each actors, and 'Min' values  are equals to '-Max'.
     * This work well when vtkAxesActor is centered on 0, but if the actor is moved
     * bounds values may be unconsistent.
     *
     * In this method we compute bounds as the min and max of each actors in each direction (X Y Z).
     *
     */
    FWRENDERVTK_API double* GetBounds() override;

protected:
    fwVtkAxesActor();
    virtual ~fwVtkAxesActor();

private:
    fwVtkAxesActor(const fwVtkAxesActor&); // Not implemented.
    void operator=(const fwVtkAxesActor&); // Not implemented.
};

} // namespace vtk

} // namespace fwRenderVTK
