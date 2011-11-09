/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkMetaImageWriter.h>

#include <fwTools/ClassRegistrar.hpp>

#include <fwCore/base.hpp>

#include "vtkIO/vtk.hpp"
#include "vtkIO/MetaImageWriter.hpp"
#include "vtkIO/helper/ProgressVtkToFw.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::writer::IObjectWriter , ::vtkIO::MetaImageWriter, ::vtkIO::MetaImageWriter );


namespace vtkIO
{
//------------------------------------------------------------------------------

MetaImageWriter::MetaImageWriter() : ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

MetaImageWriter::~MetaImageWriter()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void MetaImageWriter::write()
{
    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::fwData::Image::sptr pImage = getConcreteObject();

    vtkSmartPointer< vtkMetaImageWriter > writer = vtkSmartPointer< vtkMetaImageWriter >::New();
    vtkSmartPointer< vtkImageData > vtkImage = vtkSmartPointer< vtkImageData >::New();
    ::vtkIO::toVTKImage( pImage, vtkImage );
    writer->SetInput( vtkImage );
    writer->SetFileName( this->getFile().string().c_str() );
    writer->SetCompression(true);

    Progressor progress(writer, this->getSptr(), this->getFile().string());

    writer->Write();
}

//------------------------------------------------------------------------------

std::string  MetaImageWriter::extension()
{
   return ".mhd";
}

} // namespace vtkIO
