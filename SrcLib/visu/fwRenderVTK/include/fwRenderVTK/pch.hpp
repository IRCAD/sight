/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDERVTK_PCH_HPP__
#define __FWRENDERVTK_PCH_HPP__

// Cause an internal compiler error on windows...
#ifndef _WIN32
#include <fwCom/HasSlots.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>
#endif // _WIN32

#include <fwData/Image.hpp>
#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Point.hpp>
#include <fwData/String.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/Image.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

#include <vtkActor.h>
#include <vtkCellPicker.h>
#include <vtkCommand.h>
#include <vtkMatrix4x4.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>

#endif // __FWRENDERVTK_PCH_HPP__
