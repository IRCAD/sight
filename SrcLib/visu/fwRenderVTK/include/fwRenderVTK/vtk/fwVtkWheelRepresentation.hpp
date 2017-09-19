/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDERVTK_VTK_FWVTKWHEELREPRESENTATION_HPP__
#define __FWRENDERVTK_VTK_FWVTKWHEELREPRESENTATION_HPP__

#include "fwRenderVTK/config.hpp"

#include <glm/vec2.hpp>

#include <vtkActor2D.h>
#include <vtkPoints.h>
#include <vtkPropCollection.h>
#include <vtkWidgetRepresentation.h>

/**
 * @brief Representation of a wheel widget.
 *
 * This widget can be moved by dragging its center. The outer part can be rotated.
 */
class FWRENDERVTK_CLASS_API fwVtkWheelRepresentation : public vtkWidgetRepresentation
{
public:

    /// Calls the constructor. Initializes wheel geometry and actor.
    FWRENDERVTK_API static fwVtkWheelRepresentation* New();

    vtkTypeMacro(fwVtkWheelRepresentation, vtkWidgetRepresentation);

    /// Prints class information. Not implemented.
    void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

    /// Generates the representation using the center, radius and orientation.
    FWRENDERVTK_API void BuildRepresentation() VTK_OVERRIDE;

    /// Updates wheel geometry using the center, radius and orientation.
    FWRENDERVTK_API void UpdateRepresentation();

    /// Adds the wheel actor to the prop collection.
    FWRENDERVTK_API void GetActors2D(vtkPropCollection* pc) VTK_OVERRIDE;

    /// Releases graphic resources used by this class' actor.
    FWRENDERVTK_API void ReleaseGraphicsResources(vtkWindow*) VTK_OVERRIDE;

    /// Standard VTK render methods.
    FWRENDERVTK_API int RenderOverlay(vtkViewport*) VTK_OVERRIDE;
    FWRENDERVTK_API int RenderOpaqueGeometry(vtkViewport*) VTK_OVERRIDE;
    FWRENDERVTK_API int RenderTranslucentPolygonalGeometry(vtkViewport*) VTK_OVERRIDE;

    /// Check if this class holds any translucent geometry. (Not implemented)
    FWRENDERVTK_API int HasTranslucentPolygonalGeometry() VTK_OVERRIDE;

    /// Set the wheel orientation, expressed in radians.
    FWRENDERVTK_API void SetOrientation(double orientation);

    /// Returns the center in viewport coordinates.
    FWRENDERVTK_API ::glm::dvec2 GetCenterInScreenSpace() const;

    /// Check if the (X, Y) screen position is inside the wheel center.
    FWRENDERVTK_API bool isInCenter(int X, int Y) const;

    /// Check if the (X, Y) screen position is on the wheel.
    FWRENDERVTK_API bool isOnWheel(int X, int Y) const;

    //------------------------------------------------------------------------------

    /// Returns the orientation in radians.
    FWRENDERVTK_API double GetOrientation() const
    {
        return Orientation;
    }

    //------------------------------------------------------------------------------

    /// Get the actor holding the wheel.
    FWRENDERVTK_API vtkActor2D* GetWheelActor()
    {
        return WheelActor;
    }

protected:

    /// Initializes wheel geometry and actor.
    fwVtkWheelRepresentation();

    /// Deletes wheel geometry and actor.
    ~fwVtkWheelRepresentation() VTK_OVERRIDE;

    /// List of points defining the wheel.
    vtkPoints* WheelPoints;

    /// Actor displaying the widget.
    vtkActor2D* WheelActor;

private:
    fwVtkWheelRepresentation(const fwVtkWheelRepresentation&) VTK_DELETE_FUNCTION;
    void operator=(const fwVtkWheelRepresentation&) VTK_DELETE_FUNCTION;

    /// Wheel central part dimensions. The center is used to move the wheel.
    double CenterInnerRadius;
    double CenterOuterRadius;

    /// Actual wheel dimensions. Defines the rotating part.
    double WheelInnerRadius;
    double WheelOuterRadius;

    /// Wheel angle in radians.
    double Orientation;

    unsigned int nSectors;

    unsigned int nMarkedSectors;

    /// The wheel center, expressed relatively to the widget position.
    ::glm::dvec2 Center;

    /// 2D Translation to convert from widget to wheel space.
    ::glm::dvec2 WidgetToCenterTranslation;

};

#endif // __FWRENDERVTK_VTK_FWVTKWHEELREPRESENTATION_HPP__
