/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkImageData.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkZLibDataCompressor.h>
#include <vtkSmartPointer.h>

#include <fwTools/ClassRegistrar.hpp>

#include <fwCore/base.hpp>

#include "vtkIO/vtk.hpp"
#include "vtkIO/VtiImageWriter.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::writer::IObjectWriter , ::vtkIO::VtiImageWriter, ::vtkIO::VtiImageWriter );


namespace vtkIO
{
//------------------------------------------------------------------------------

VtiImageWriter::VtiImageWriter() : ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

VtiImageWriter::~VtiImageWriter()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void VtiImageWriter::write()
{
    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::fwData::Image::sptr pImage = getConcreteObject();

    vtkSmartPointer< vtkXMLImageDataWriter > writer = vtkSmartPointer< vtkXMLImageDataWriter >::New();
    vtkSmartPointer< vtkImageData > vtkImage = vtkSmartPointer< vtkImageData >::New();
    ::vtkIO::toVTKImage( pImage, vtkImage );
    writer->SetInput( vtkImage );
    writer->SetFileName( this->getFile().string().c_str() );
    writer->SetDataModeToAppended();

    vtkSmartPointer< vtkZLibDataCompressor > compressor  = vtkSmartPointer< vtkZLibDataCompressor >::New();
    compressor->SetCompressionLevel(1);
    writer->SetCompressor( compressor );
    writer->EncodeAppendedDataOff();

    writer->Write();
}

//------------------------------------------------------------------------------

std::string  VtiImageWriter::extension()
{
   return ".vti";
}

} // namespace vtkIO
