/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkImageData.h>
#include <vtkGenericDataObjectWriter.h>


#include <fwTools/ClassRegistrar.hpp>

#include <fwCore/base.hpp>

#include "vtkIO/vtk.hpp"
#include "vtkIO/ImageWriter.hpp"
#include "vtkIO/ProgressVtktoFw.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::writer::IObjectWriter , ::vtkIO::ImageWriter, ::vtkIO::ImageWriter );


namespace vtkIO
{
//------------------------------------------------------------------------------

ImageWriter::ImageWriter() : ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this)
{
    SLM_TRACE("vtkIO::ImageWriter::ImageWriter");
}

//------------------------------------------------------------------------------

ImageWriter::~ImageWriter()
{
    SLM_TRACE("vtkIO::ImageWriter::~ImageWriter");
}

//------------------------------------------------------------------------------

void ImageWriter::write()
{
    assert( m_object.use_count() );
    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::fwData::Image::sptr pImage = getConcreteObject();

    vtkGenericDataObjectWriter *writer = vtkGenericDataObjectWriter::New();
    writer->SetInput( ::vtkIO::toVTKImage( pImage ) );
    writer->SetFileName(this->getFile().string().c_str());
    writer->SetFileTypeToBinary ();

    //add progress observation
    ::vtkIO::ProgressVtktoFw( writer, this, getFile().string() );

    writer->Write();
    writer->Delete();
}

//------------------------------------------------------------------------------

std::string  ImageWriter::extension()
{
   return ".vtk";
}

} // namespace vtkIO
