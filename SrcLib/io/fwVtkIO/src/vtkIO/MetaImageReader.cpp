/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkSmartPointer.h>
#include <vtkMetaImageReader.h>
#include <vtkImageData.h>

#include <fwCore/base.hpp>

#include <fwDataIO/reader/registry/macros.hpp>

#include "vtkIO/helper/ProgressVtkToFw.hpp"
#include "vtkIO/vtk.hpp"
#include "vtkIO/MetaImageReader.hpp"

fwDataIOReaderRegisterMacro( ::vtkIO::MetaImageReader );


namespace vtkIO
{
//------------------------------------------------------------------------------

MetaImageReader::MetaImageReader(::fwDataIO::reader::IObjectReader::Key key)
: ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

MetaImageReader::~MetaImageReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void MetaImageReader::read()
{
    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::fwData::Image::sptr pImage = this->getConcreteObject();

    vtkSmartPointer< vtkMetaImageReader > reader = vtkSmartPointer< vtkMetaImageReader >::New();
    reader->SetFileName(this->getFile().string().c_str());

    Progressor progress(reader, this->getSptr(), this->getFile().string());

    reader->Update();

    vtkDataObject *obj = reader->GetOutput();
    vtkImageData* img = vtkImageData::SafeDownCast(obj);
    FW_RAISE_IF("MetaImageReader cannot read mhd image file :"<<this->getFile().string(), !img);
    try
    {
        ::vtkIO::fromVTKImage( img, pImage);
    }
    catch( std::exception &e)
    {
        FW_RAISE("MetaImage to fwData::Image failed : "<<e.what());
    }
}

//------------------------------------------------------------------------------

std::string  MetaImageReader::extension()
{
   return ".mhd";
}

} // namespace vtkIO
