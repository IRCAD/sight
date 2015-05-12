/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkMetaImageWriter.h>

#include <fwCore/base.hpp>

#include <fwDataIO/writer/registry/macros.hpp>

#include "fwVtkIO/vtk.hpp"
#include "fwVtkIO/MetaImageWriter.hpp"
#include "fwVtkIO/helper/ProgressVtkToFw.hpp"

fwDataIOWriterRegisterMacro( ::fwVtkIO::MetaImageWriter );


namespace fwVtkIO
{
//------------------------------------------------------------------------------

MetaImageWriter::MetaImageWriter(::fwDataIO::writer::IObjectWriter::Key key) :
    ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this)
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
    vtkSmartPointer< vtkImageData > vtkImage     = vtkSmartPointer< vtkImageData >::New();
    ::fwVtkIO::toVTKImage( pImage, vtkImage );
    writer->SetInputData( vtkImage );
    writer->SetFileName( this->getFile().string().c_str() );
    writer->SetCompression(true);

    Progressor progress(writer, this->getSptr(), this->getFile().string());

    writer->Write();
}

//------------------------------------------------------------------------------

std::string MetaImageWriter::extension()
{
    return ".mhd";
}

} // namespace fwVtkIO
