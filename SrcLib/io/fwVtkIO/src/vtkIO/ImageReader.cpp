/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkGenericDataObjectReader.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>

#include <fwTools/ClassRegistrar.hpp>

#include <fwCore/base.hpp>

#include "vtkIO/vtk.hpp"
#include "vtkIO/ImageReader.hpp"
#include "vtkIO/helper/ProgressVtkToFw.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::reader::IObjectReader , ::vtkIO::ImageReader, ::vtkIO::ImageReader );


namespace vtkIO
{
//------------------------------------------------------------------------------

ImageReader::ImageReader()
: ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

ImageReader::~ImageReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void ImageReader::read()
{
    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::fwData::Image::sptr pImage = getConcreteObject();

    vtkSmartPointer< vtkGenericDataObjectReader > reader = vtkSmartPointer< vtkGenericDataObjectReader >::New();
    reader->SetFileName(this->getFile().string().c_str());

    //add progress observation
    Progressor progress(reader, this->getSptr(), this->getFile().string());

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
        errMsg  = "ImageReader cannot read VTK image file : ";
        errMsg.append( this->getFile().string() );
        throw( errMsg );
    }
}

//------------------------------------------------------------------------------

std::string  ImageReader::extension()
{
   return ".vtk";
}

} // namespace vtkIO
