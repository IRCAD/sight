/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKARAdaptor/FixedInteractorStyle.hpp"

#include <fwCore/base.hpp>

#include <vtkCommand.h>
#include <vtkObjectFactory.h>

//------------------------------------------------------------------------------

vtkObject* vtkInstantiatorFixedInteractorStyleNew()
{
    return FixedInteractorStyle::New();
}

//------------------------------------------------------------------------------

FixedInteractorStyle* FixedInteractorStyle::New()
{
    return new FixedInteractorStyle;
}

//------------------------------------------------------------------------------

FixedInteractorStyle::FixedInteractorStyle() :
    vtkInteractorStyleUser()
{
}

//------------------------------------------------------------------------------

FixedInteractorStyle::~FixedInteractorStyle()
{
}

//------------------------------------------------------------------------------

void FixedInteractorStyle::OnChar()
{
}

//------------------------------------------------------------------------------

void FixedInteractorStyle::OnLeftButtonDown()
{
}

//------------------------------------------------------------------------------

void FixedInteractorStyle::OnRightButtonDown()
{
}

//------------------------------------------------------------------------------

void FixedInteractorStyle::OnMiddleButtonDown()
{
}

//------------------------------------------------------------------------------

void FixedInteractorStyle::OnMouseMove()
{
}

//----------------------------------------------------------------------------

void FixedInteractorStyle::OnMouseWheelBackward()
{
}

//----------------------------------------------------------------------------

void FixedInteractorStyle::OnMouseWheelForward()
{
}
