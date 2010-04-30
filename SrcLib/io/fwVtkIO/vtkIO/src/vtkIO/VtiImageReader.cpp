/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkXMLImageDataReader.h>
#include <vtkImageData.h>

#include <fwTools/ClassRegistrar.hpp>

#include <fwCore/base.hpp>

#include "vtkIO/vtk.hpp"
#include "vtkIO/VtiImageReader.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::reader::IObjectReader , ::vtkIO::VtiImageReader, ::vtkIO::VtiImageReader );


namespace vtkIO
{
//------------------------------------------------------------------------------

VtiImageReader::VtiImageReader()
: ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >(this)
{
    SLM_TRACE("vtkIO::VtiImageReader::VtiImageReader");
}

//------------------------------------------------------------------------------

VtiImageReader::~VtiImageReader()
{
    SLM_TRACE("vtkIO::VtiImageReader::~VtiImageReader");
}

//------------------------------------------------------------------------------

void VtiImageReader::read()
{
    assert( m_object.use_count() );
    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::fwData::Image::sptr pImage = getConcreteObject();

    vtkXMLImageDataReader * reader = vtkXMLImageDataReader::New();
    reader->SetFileName(this->getFile().string().c_str());
    reader->Update();

    vtkDataObject *obj = reader->GetOutput();
    vtkImageData* img = vtkImageData::SafeDownCast(obj);
    if(img)
    {
        ::vtkIO::fromVTKImage(img, pImage);
    }
    else
    {
        std::string errMsg;
        errMsg  = "VtiImageReader cannot read Vti image file : ";
        errMsg.append( this->getFile().string() );
        throw( errMsg );
    }
    reader->Delete();
}

//------------------------------------------------------------------------------

std::string  VtiImageReader::extension()
{
   return ".vti";
}

} // namespace vtkIO
