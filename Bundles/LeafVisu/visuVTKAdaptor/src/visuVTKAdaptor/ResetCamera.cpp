/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IService.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/CompositeMsg.hpp>

#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkActorCollection.h>

#include "visuVTKAdaptor/ResetCamera.hpp"


REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::ResetCamera, ::fwData::Composite ) ;

namespace visuVTKAdaptor
{

ResetCamera::ResetCamera() throw()
{
    bCameraReseted = false;
    m_comChannelPriority = 0.1;
    addNewHandledEvent(::fwComEd::CompositeMsg::ADDED_FIELDS);
}

//------------------------------------------------------------------------------

ResetCamera::~ResetCamera() throw()
{
}

//------------------------------------------------------------------------------

void ResetCamera::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
}

//------------------------------------------------------------------------------

void ResetCamera::doStart() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void ResetCamera::doUpdate() throw(fwTools::Failed)
{

}

//------------------------------------------------------------------------------

void ResetCamera::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void ResetCamera::doStop() throw(fwTools::Failed)
{

}

//------------------------------------------------------------------------------

void ResetCamera::doUpdate( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    ::fwComEd::CompositeMsg::csptr compositeMsg = ::fwComEd::CompositeMsg::dynamicConstCast( msg );

    if ( compositeMsg && compositeMsg->hasEvent( ::fwComEd::CompositeMsg::ADDED_FIELDS))
    {
        ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
        if(!bCameraReseted)
        {
            vtkRenderer* renderer = this->getRenderer();
            renderer->ResetCamera();
            bCameraReseted = true;
            this->setVtkPipelineModified();
        }
    }
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
