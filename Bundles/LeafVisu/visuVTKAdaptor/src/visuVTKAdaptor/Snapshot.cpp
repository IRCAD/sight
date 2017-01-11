/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "visuVTKAdaptor/Snapshot.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/vtk.hpp>

#include <vtkActor.h>
#include <vtkBMPWriter.h>
#include <vtkImageData.h>
#include <vtkImageWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkPNGWriter.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkTIFFWriter.h>
#include <vtkWindowToImageFilter.h>

#include <boost/filesystem.hpp>

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Snapshot, ::fwData::Composite );

namespace visuVTKAdaptor
{

const ::fwCom::Slots::SlotKeyType Snapshot::s_SNAP_SLOT        = "snap";
const ::fwCom::Slots::SlotKeyType Snapshot::s_SNAPTOIMAGE_SLOT = "snapToImage";

Snapshot::Snapshot() throw()
{
    newSlot(s_SNAP_SLOT, &Snapshot::snap, this);
    newSlot(s_SNAPTOIMAGE_SLOT, &Snapshot::snapToImage, this);
}

//------------------------------------------------------------------------------

Snapshot::~Snapshot() throw()
{
}

//------------------------------------------------------------------------------

void Snapshot::doConfigure() throw(fwTools::Failed)
{
    if(m_configuration->hasAttribute("image"))
    {
        m_imageUid = m_configuration->getAttributeValue("image");
    }
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

void Snapshot::snapToImage()
{
    if ( !m_imageUid.empty() )
    {
        ::fwData::Image::sptr imageToSnap;

        if (!this->isVersion2())
        {
            ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(m_imageUid);
            SLM_ASSERT("Object '" + m_imageUid + "' is not found", obj);
            imageToSnap = ::fwData::Image::dynamicCast(obj);
            SLM_ASSERT("Object '" + m_imageUid + "' is not an ::fwData::Image (" + obj->getClassname() + ")",
                       imageToSnap);
        }
        else
        {
            imageToSnap = this->getSafeInOut< ::fwData::Image>(m_imageUid);
            SLM_ASSERT("The image \"" << m_imageUid << "\" is not valid.", imageToSnap);
        }

        vtkWindowToImageFilter* snapper = vtkWindowToImageFilter::New();
        snapper->SetMagnification( 1 );
        snapper->SetInput( this->getRenderer()->GetRenderWindow() );
        snapper->Update();

        vtkImageData* vtkImage = snapper->GetOutput();
        ::fwVtkIO::fromVTKImage(vtkImage, imageToSnap);

        snapper->Delete();
    }
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
