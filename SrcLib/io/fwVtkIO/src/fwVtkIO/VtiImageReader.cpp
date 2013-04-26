/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkSmartPointer.h>
#include <vtkXMLImageDataReader.h>
#include <vtkImageData.h>

#include <fwCore/base.hpp>

#include <fwDataIO/reader/registry/macros.hpp>

#include "fwVtkIO/vtk.hpp"
#include "fwVtkIO/VtiImageReader.hpp"
#include "fwVtkIO/helper/ProgressVtkToFw.hpp"

fwDataIOReaderRegisterMacro( ::fwVtkIO::VtiImageReader );


namespace fwVtkIO
{
//------------------------------------------------------------------------------

VtiImageReader::VtiImageReader(::fwDataIO::reader::IObjectReader::Key key)
: ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

VtiImageReader::~VtiImageReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void VtiImageReader::read()
{
    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::fwData::Image::sptr pImage = getConcreteObject();

    vtkSmartPointer< vtkXMLImageDataReader > reader = vtkSmartPointer< vtkXMLImageDataReader >::New();
    reader->SetFileName(this->getFile().string().c_str());

    Progressor progress(reader, this->getSptr(), this->getFile().string());

    reader->Update();

    vtkDataObject *obj = reader->GetOutput();
    vtkImageData* img = vtkImageData::SafeDownCast(obj);
    FW_RAISE_IF("VtiImageReader cannot read Vti image file :"<<this->getFile().string(), !img);
    try
    {
        ::fwVtkIO::fromVTKImage( img, pImage);
    }
    catch( std::exception &e)
    {
        FW_RAISE("VTIImage to fwData::Image failed "<<e.what());
    }
}

//------------------------------------------------------------------------------

std::string  VtiImageReader::extension()
{
   return ".vti";
}

} // namespace fwVtkIO
