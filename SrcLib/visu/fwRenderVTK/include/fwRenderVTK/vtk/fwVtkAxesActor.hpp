/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDERVTK_VTK_FWVTKAXESACTOR_HPP__
#define __FWRENDERVTK_VTK_FWVTKAXESACTOR_HPP__

#ifndef ANDROID

#include "fwRenderVTK/config.hpp"

#include <vtkAxesActor.h>

class vtkPropCollection;

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

#endif // ANDROID
#endif //__FWRENDERVTK_VTK_FWVTKAXESACTOR_HPP__
