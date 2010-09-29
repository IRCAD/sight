/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: fwVtkPicker.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME fwVtkPicker - superclass for 3D geometric pickers (uses ray cast)
// .SECTION Description
// fwVtkPicker is used to select instances of vtkProp3D by shooting a ray
// into a graphics window and intersecting with the actor's bounding box. 
// The ray is defined from a point defined in window (or pixel) coordinates, 
// and a point located from the camera's position.
//
// fwVtkPicker may return more than one vtkProp3D, since more than one bounding
// box may be intersected. fwVtkPicker returns the list of props that were hit,
// the pick coordinates in world and untransformed mapper space, and the 
// prop (vtkProp3D) and mapper that are "closest" to the camera. The closest 
// prop is the one whose center point (i.e., center of bounding box) 
// projected on the ray is closest to the camera.

// .SECTION See Also
// fwVtkPicker is used for quick geometric picking. If you desire to pick
// points or cells, use the subclass vtkPointPicker or vtkCellPicker,
// respectively.  Or you may use hardware picking to pick any type of vtkProp
// - see vtkPropPicker or vtkWorldPointPicker.

#ifndef __FWRENDERVTK_VTKPICKER__
#define __FWRENDERVTK_VTKCPICKER__

#include "fwRenderVTK/config.hpp"

#include <vtkPicker.h>

class vtkAbstractMapper3D;
class vtkDataSet;
class vtkTransform;
class vtkActorCollection;
class vtkProp3DCollection;
class vtkPropCollection;
class vtkPolyData;
class vtkPoints;

class FWRENDERVTK_CLASS_API fwVtkPicker : public vtkPicker
{
public:
    FWRENDERVTK_API static fwVtkPicker *New();

  vtkTypeRevisionMacro(fwVtkPicker,vtkPicker);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Perform pick operation with selection point provided. Normally the 
  // first two values for the selection point are x-y pixel coordinate, and
  // the third value is =0. Return non-zero if something was successfully 
  // picked.
  FWRENDERVTK_API virtual int Pick(double selectionX, double selectionY, double selectionZ,
                   vtkRenderer *renderer);  



  FWRENDERVTK_API int PickPolyData( double p1[3], double p2[3], vtkPolyData *polydata);
  FWRENDERVTK_API int Pick( double p1[3], double p2[3], vtkPropCollection *props);
  // Description: 
  // Perform pick operation with selection point provided. Normally the first
  // two values for the selection point are x-y pixel coordinate, and the
  // third value is =0. Return non-zero if something was successfully picked.
  int Pick(double selectionPt[3], vtkRenderer *ren)
    {return this->Pick(selectionPt[0], selectionPt[1], selectionPt[2], ren);};
      
protected:
  fwVtkPicker();
  ~fwVtkPicker();

private:
  fwVtkPicker(const fwVtkPicker&);  // Not implemented.
  void operator=(const fwVtkPicker&);  // Not implemented.
};


#endif


