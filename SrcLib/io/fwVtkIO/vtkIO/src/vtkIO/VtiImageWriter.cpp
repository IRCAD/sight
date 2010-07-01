/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkImageData.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkZLibDataCompressor.h>

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
    SLM_TRACE("vtkIO::VtiImageWriter::VtiImageWriter");
}

//------------------------------------------------------------------------------

VtiImageWriter::~VtiImageWriter()
{
    SLM_TRACE("vtkIO::VtiImageWriter::~VtiImageWriter");
}

//------------------------------------------------------------------------------

void VtiImageWriter::write()
{
    assert( m_object.use_count() );
    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::fwData::Image::sptr pImage = getConcreteObject();

    vtkXMLImageDataWriter * writer = vtkXMLImageDataWriter::New();
    writer->SetInput( ::vtkIO::toVTKImage( pImage ) );
    writer->SetFileName( this->getFile().string().c_str() );
    writer->SetDataModeToAppended();

    vtkZLibDataCompressor* compressor  = vtkZLibDataCompressor::New();
    compressor->SetCompressionLevel(1);
    writer->SetCompressor( compressor );
    writer->EncodeAppendedDataOff();

    writer->Write();
    compressor->Delete();
    writer->Delete();
}

//------------------------------------------------------------------------------

std::string  VtiImageWriter::extension()
{
   return ".vti";
}

} // namespace vtkIO
