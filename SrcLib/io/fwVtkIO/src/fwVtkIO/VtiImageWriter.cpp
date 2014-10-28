/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkImageData.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkZLibDataCompressor.h>
#include <vtkSmartPointer.h>

#include <fwCore/base.hpp>

#include <fwDataIO/writer/registry/macros.hpp>

#include "fwVtkIO/vtk.hpp"
#include "fwVtkIO/VtiImageWriter.hpp"
#include "fwVtkIO/helper/ProgressVtkToFw.hpp"

fwDataIOWriterRegisterMacro( ::fwVtkIO::VtiImageWriter );


namespace fwVtkIO
{
//------------------------------------------------------------------------------

VtiImageWriter::VtiImageWriter(::fwDataIO::writer::IObjectWriter::Key key) :
        ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this)
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
    ::fwVtkIO::toVTKImage( pImage, vtkImage );
    writer->SetInput( vtkImage );
    writer->SetFileName( this->getFile().string().c_str() );
    writer->SetDataModeToAppended();

    vtkSmartPointer< vtkZLibDataCompressor > compressor  = vtkSmartPointer< vtkZLibDataCompressor >::New();
    compressor->SetCompressionLevel(1);
    writer->SetCompressor( compressor );
    writer->EncodeAppendedDataOff();

    Progressor progress(writer, this->getSptr(), this->getFile().string());

    writer->Write();
}

//------------------------------------------------------------------------------

std::string  VtiImageWriter::extension()
{
   return ".vti";
}

} // namespace fwVtkIO
