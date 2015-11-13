/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "visuVTKAdaptor/Snapshot.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwComEd/Dictionary.hpp>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IService.hpp>

#include <vtkActor.h>
#include <vtkBMPWriter.h>
#include <vtkImageWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkPNGWriter.h>
#include <vtkRenderer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkTIFFWriter.h>
#include <vtkWindowToImageFilter.h>

#include <boost/filesystem.hpp>


fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Snapshot, ::fwData::Composite );

namespace visuVTKAdaptor
{

const ::fwCom::Slots::SlotKeyType Snapshot::s_SNAP_SIG = "snap";

Snapshot::Snapshot() throw()
{
    newSlot(s_SNAP_SIG, &Snapshot::snap, this);
}

//------------------------------------------------------------------------------

Snapshot::~Snapshot() throw()
{
}

//------------------------------------------------------------------------------

void Snapshot::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
}

//------------------------------------------------------------------------------

void Snapshot::doStart() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Snapshot::doUpdate() throw(fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void Snapshot::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Snapshot::doStop() throw(fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void Snapshot::snap(std::string filePath)
{
    SLM_ASSERT("filePath is empty", !filePath.empty());
    namespace fs = ::boost::filesystem;
    fs::path pathImageSnap(filePath);

    std::string ext = ".jpg";
    ext = pathImageSnap.extension().string();
    vtkImageWriter* writer = 0;

    if( ext==".jpg" || ext==".jpeg" )
    {
        writer = vtkJPEGWriter::New();
    }
    else if ( ext==".bmp" )
    {
        writer = vtkBMPWriter::New();
    }
    else if ( ext==".tiff" )
    {
        writer = vtkTIFFWriter::New();
    }
    else if ( ext==".png" )
    {
        writer = vtkPNGWriter::New();
    }
    else
    {
        SLM_FATAL("Error: Format is not supported.");
    }

    vtkWindowToImageFilter* snapper = vtkWindowToImageFilter::New();
    snapper->SetMagnification( 1 );
    snapper->SetInput( this->getRenderer()->GetRenderWindow() );

    writer->SetInputConnection( snapper->GetOutputPort() );
    writer->SetFileName( pathImageSnap.string().c_str() );
    writer->Write();

    snapper->Delete();
    writer->Delete();
}


} //namespace visuVTKAdaptor
