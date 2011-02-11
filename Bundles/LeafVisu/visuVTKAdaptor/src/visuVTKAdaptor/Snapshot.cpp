/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem.hpp>

#include <vtkWindowToImageFilter.h>
#include <vtkImageWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkBMPWriter.h>
#include <vtkTIFFWriter.h>
#include <vtkPNGWriter.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/IService.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/CompositeMsg.hpp>

#include <vtkActor.h>
#include <vtkRenderer.h>


#include "visuVTKAdaptor/Snapshot.hpp"


REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Snapshot, ::fwData::Composite ) ;

namespace visuVTKAdaptor
{

Snapshot::Snapshot() throw()
{
    addNewHandledEvent("SNAP");
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

void Snapshot::doUpdate( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    ::fwComEd::CompositeMsg::csptr compositeMsg = ::fwComEd::CompositeMsg::dynamicConstCast( msg );

    if ( compositeMsg && compositeMsg->hasEvent( "SNAP"))
    {
        ::fwData::Object::csptr dataInfo = compositeMsg->getDataInfo("SNAP");

        SLM_ASSERT("sceneID missing", dataInfo->getFieldSize("sceneID"));
        ::fwData::String::sptr sceneID = dataInfo->getFieldSingleElement< ::fwData::String >("sceneID");
        SLM_ASSERT("sceneID empty!", sceneID);

        SLM_ASSERT("filename missing", dataInfo->getFieldSize("filename"));
        ::fwData::String::sptr filename = dataInfo->getFieldSingleElement< ::fwData::String >("filename");
        SLM_ASSERT("filename empty!", !filename->value().empty());

        if( this->getRenderService()->getID() == sceneID->value())
        {
            this->snap(filename->value());
        }
    }
}

//------------------------------------------------------------------------------

void Snapshot::snap(std::string filePath)
{
    SLM_ASSERT("filePath is empty", !filePath.empty());
    namespace fs = ::boost::filesystem;
    fs::path pathImageSnap(filePath);

    std::string ext = ".jpg";
    ext = pathImageSnap.extension();

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


    writer->SetInput( snapper->GetOutput() );
    writer->SetFileName( pathImageSnap.string().c_str() );
    writer->Write();

    snapper->Delete();
    writer->Delete();
}


} //namespace visuVTKAdaptor
