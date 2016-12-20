/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#include "fwVtkIO/vtk.hpp"

#include <fwData/Image.hpp>
#include <fwData/ObjectLock.hpp>

#include <fwDataTools/helper/Image.hpp>
#include <fwDataTools/helper/ImageGetter.hpp>

#include <fwMath/MeshFunctions.hpp>

#include <vtkCell.h>
#include <vtkCellType.h>
#include <vtkDataArray.h>
#include <vtkDataSetAttributes.h>
#include <vtkImageData.h>
#include <vtkImageExport.h>
#include <vtkImageImport.h>
#include <vtkLookupTable.h>
#include <vtkMatrix4x4.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataWriter.h>
#include <vtkSetGet.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>
#include <vtkUnstructuredGrid.h>

#include <boost/assign/list_of.hpp>
#include <boost/cast.hpp>

#include <cstring>
#include <functional>
#include <numeric>
#include <stdexcept>


namespace fwVtkIO
{


TypeTranslator::fwToolsToVtkMap::mapped_type TypeTranslator::translate(
    const TypeTranslator::fwToolsToVtkMap::key_type& key )
{
    fwToolsToVtkMap::const_iterator it = s_toVtk.find( key );
    FW_RAISE_IF("Unknown Type: " << key, it == s_toVtk.end() );
    return it->second;
}

TypeTranslator::VtkTofwToolsMap::mapped_type TypeTranslator::translate(
    const TypeTranslator::VtkTofwToolsMap::key_type& key )
{
    VtkTofwToolsMap::const_iterator it = s_fromVtk.find( key );
    FW_RAISE_IF("Unknown Type: " << key, it == s_fromVtk.end() );
    return it->second;
}




const TypeTranslator::fwToolsToVtkMap TypeTranslator::s_toVtk
    = boost::assign::map_list_of
      // char and signed char are treated as the same type.
      // and plain char is used when writing an int8 image
          ( fwTools::Type::create("int8" ), VTK_CHAR )
          ( fwTools::Type::create("uint8" ), VTK_UNSIGNED_CHAR )

          ( fwTools::Type::create("int16"), VTK_SHORT )
          ( fwTools::Type::create("uint16"), VTK_UNSIGNED_SHORT )

          ( fwTools::Type::create("int32"), VTK_INT )
          ( fwTools::Type::create("uint32"), VTK_UNSIGNED_INT )

          ( fwTools::Type::create("float" ), VTK_FLOAT )
          ( fwTools::Type::create("double"), VTK_DOUBLE )

#if ( INT_MAX < LONG_MAX )
      ( fwTools::Type::create("int64"), VTK_LONG )
          ( fwTools::Type::create("uint64"), VTK_UNSIGNED_LONG )
#endif
    ;




const TypeTranslator::VtkTofwToolsMap TypeTranslator::s_fromVtk
    = boost::assign::map_list_of

      // char and signed char are treated as the same type.
      // and plain char is used when writing an int8 image
          ( VTK_SIGNED_CHAR, fwTools::Type::create("int8" )  )
          ( VTK_CHAR, fwTools::Type::create("int8" )  )
          ( VTK_UNSIGNED_CHAR, fwTools::Type::create("uint8" ) )

          ( VTK_SHORT, fwTools::Type::create("int16")  )
          ( VTK_UNSIGNED_SHORT, fwTools::Type::create("uint16") )

          ( VTK_INT, fwTools::Type::create("int32")  )
          ( VTK_UNSIGNED_INT, fwTools::Type::create("uint32") )

          ( VTK_FLOAT, fwTools::Type::create("float" ) )
          ( VTK_DOUBLE, fwTools::Type::create("double") )

#if ( INT_MAX < LONG_MAX )
      ( VTK_LONG, fwTools::Type::create("int64")  )
          ( VTK_UNSIGNED_LONG, fwTools::Type::create("uint64") )

          ( VTK___INT64, fwTools::Type::create("int64")  )
          ( VTK_LONG_LONG, fwTools::Type::create("int64")  )

          ( VTK_UNSIGNED___INT64, fwTools::Type::create("uint64") )
          ( VTK_UNSIGNED_LONG_LONG, fwTools::Type::create("uint64") )
#else
      ( VTK_LONG, fwTools::Type::create("int32")  )
          ( VTK_UNSIGNED_LONG, fwTools::Type::create("uint32") )
#endif
    ;


//-----------------------------------------------------------------------------

void toVTKImage( ::fwData::Image::csptr data,  vtkImageData* dst)
{
    vtkSmartPointer< vtkImageImport > importer = vtkSmartPointer< vtkImageImport >::New();

    configureVTKImageImport( importer, data );

    importer->Update();

    dst->ShallowCopy(importer->GetOutput());
}

//-----------------------------------------------------------------------------


template< typename IMAGETYPE >
void* newBuffer(size_t size)
{
    IMAGETYPE* destBuffer;
    try
    {
        destBuffer = new IMAGETYPE[ size ];
    }
    catch (std::exception& e)
    {
        OSLM_ERROR ("No enough memory to allocate an image of type "
                    << fwTools::makeDynamicType<IMAGETYPE>().string()
                    << " and of size "<< size << "." << std::endl
                    << e.what() );
        throw;
    }
    return destBuffer;
}

//-----------------------------------------------------------------------------

template< typename IMAGETYPE >
void fromRGBBuffer( void* input, size_t size, void*& destBuffer)
{
    if(destBuffer == NULL)
    {
        destBuffer = newBuffer<IMAGETYPE>(size);
    }

    IMAGETYPE* destBufferTyped = (IMAGETYPE*)destBuffer;
    IMAGETYPE* inputTyped      = (IMAGETYPE*)input;
    IMAGETYPE* finalPtr        = ((IMAGETYPE*)destBuffer) + size;
    IMAGETYPE valR, valG,valB;

    while (destBufferTyped < finalPtr)
    {
        valR                 = (IMAGETYPE)(float((*(inputTyped++)) * 0.30));
        valG                 = (IMAGETYPE)(float((*(inputTyped++)) * 0.59));
        valB                 = (IMAGETYPE)(float((*(inputTyped++)) * 0.11));
        (*destBufferTyped++) = valR + valG + valB;
    }
}


//-----------------------------------------------------------------------------

template< typename IMAGETYPE >
void fromRGBBufferColor( void* input, size_t size, void*& destBuffer)
{
    if(destBuffer == NULL)
    {
        destBuffer = newBuffer<IMAGETYPE>(size);
    }

    IMAGETYPE* destBufferTyped = (IMAGETYPE*)destBuffer;
    IMAGETYPE* inputTyped      = (IMAGETYPE*)input;
    IMAGETYPE* finalPtr        = ((IMAGETYPE*)destBuffer) + size;

    while (destBufferTyped < finalPtr)
    {
        (*destBufferTyped++) = (*(inputTyped++));
    }
}

//-----------------------------------------------------------------------------


void fromVTKImage( vtkImageData* source, ::fwData::Image::sptr destination )
{
    SLM_ASSERT("vtkImageData source and/or ::fwData::Image destination are not correct", destination && source );

    ::fwDataTools::helper::Image imageHelper(destination);

    // ensure image size correct
//    source->UpdateInformation();
//    source->PropagateUpdateExtent();

    int dim = source->GetDataDimension();
    OSLM_TRACE("source->GetDataDimension() : " << dim);

    SLM_WARN_IF("2D Vtk image are not yet correctly managed", dim == 2);

    if(dim == 2)
    {
        dim = 3;
        int size[3];
        size[0] = source->GetDimensions()[0];
        size[1] = source->GetDimensions()[1];
        size[2] = 1;
        destination->setSize( ::fwData::Image::SizeType(size, size+dim) );

        double spacing[3];
        spacing[0] = source->GetSpacing()[0];
        spacing[1] = source->GetSpacing()[1];
        spacing[2] = 0;
        destination->setSpacing( ::fwData::Image::SpacingType(spacing, spacing+dim) );

        double origin[3];
        origin[0] = source->GetOrigin()[0];
        origin[1] = source->GetOrigin()[1];
        origin[2] = 0;
        destination->setOrigin( ::fwData::Image::OriginType(origin, origin+dim) );
    }
    else
    {
        destination->setSize( ::fwData::Image::SizeType(source->GetDimensions(), source->GetDimensions()+dim) );
        destination->setSpacing( ::fwData::Image::SpacingType(source->GetSpacing(), source->GetSpacing()+dim) );
        destination->setOrigin( ::fwData::Image::OriginType(source->GetOrigin(), source->GetOrigin()+dim) );
    }


    int nbComponents = source->GetNumberOfScalarComponents();
    size_t size      = std::accumulate(source->GetDimensions(), source->GetDimensions()+dim, std::max(3,
                                                                                                      nbComponents),
                                       std::multiplies<size_t>() );
    void* input = source->GetScalarPointer();

    if (size != 0)
    {
        void* destBuffer;
        int nbBytePerPixel = source->GetScalarSize();
        OSLM_TRACE("image size : " << size << " - nbBytePerPixel : " << nbBytePerPixel );


        destination->setNumberOfComponents(3);
        if (nbComponents == 3 && nbBytePerPixel == 2)
        {
            SLM_TRACE ("RGB 16bits");

            destination->setType( "uint16" );
            destination->allocate();
            ::fwData::ObjectLock lock(destination);
            destBuffer = imageHelper.getBuffer();
            SLM_ASSERT("Image allocation error", destBuffer != NULL);
            fromRGBBufferColor< unsigned short >(input, size, destBuffer);
        }
        else if (nbComponents == 3 && nbBytePerPixel == 1)
        {
            SLM_TRACE ("RGB 8bits");

            destination->setType( "uint8" );
            destination->allocate();
            ::fwData::ObjectLock lock(destination);
            destBuffer = imageHelper.getBuffer();
            SLM_ASSERT("Image allocation error", destBuffer != NULL);
            fromRGBBufferColor< unsigned char >(input, size, destBuffer);
        }
        else if (nbComponents == 4 && nbBytePerPixel == 1)
        {
            SLM_TRACE ("RGBA 8bits");

            destination->setNumberOfComponents(nbComponents);
            destination->setType( "uint8" );
            destination->allocate();
            ::fwData::ObjectLock lock(destination);
            destBuffer = imageHelper.getBuffer();
            SLM_ASSERT("Image allocation error", destBuffer != NULL);
            fromRGBBufferColor< unsigned char >(input, size, destBuffer);
        }
        else
        {
            SLM_TRACE ("Luminance image");
            destination->setType( TypeTranslator::translate( source->GetScalarType() ) );
            destination->setNumberOfComponents(nbComponents);
            destination->allocate();
            ::fwData::ObjectLock lock(destination);
            destBuffer = imageHelper.getBuffer();
            size_t sizeInBytes = destination->getSizeInBytes();
            std::memcpy(destBuffer, input, sizeInBytes);
        }
    }

}

//------------------------------------------------------------------------------

void configureVTKImageImport( ::vtkImageImport* _pImageImport, ::fwData::Image::csptr _pDataImage )
{
    ::fwDataTools::helper::ImageGetter imageHelper(_pDataImage);

    _pImageImport->SetDataSpacing(  _pDataImage->getSpacing().at(0),
                                    _pDataImage->getSpacing().at(1),
                                    _pDataImage->getSpacing().at(2)
                                    );

    _pImageImport->SetDataOrigin(   _pDataImage->getOrigin().at(0),
                                    _pDataImage->getOrigin().at(1),
                                    _pDataImage->getOrigin().at(2)
                                    );

    _pImageImport->SetWholeExtent(  0, _pDataImage->getSize().at(0) - 1,
                                    0, _pDataImage->getSize().at(1) - 1,
                                    0, _pDataImage->getSize().at(2) - 1
                                    );

    _pImageImport->SetNumberOfScalarComponents(static_cast<int>( _pDataImage->getNumberOfComponents() ));

    // copy WholeExtent to DataExtent
    _pImageImport->SetDataExtentToWholeExtent();
    // no copy, no buffer destruction/management
    _pImageImport->SetImportVoidPointer( imageHelper.getBuffer() );
    // used to set correct pixeltype to VtkImage
    _pImageImport->SetDataScalarType( TypeTranslator::translate(_pDataImage->getType()) );
}

//-----------------------------------------------------------------------------

vtkMatrix4x4*  toVTKMatrix( ::fwData::TransformationMatrix3D::sptr _transfoMatrix )
{
    vtkMatrix4x4* matrix = vtkMatrix4x4 ::New();
    for(int l = 0; l<4; l++)
    {
        for(int c = 0; c <4; c++)
        {
            matrix->SetElement(l, c, _transfoMatrix->getCoefficient(l, c));
        }
    }
    return matrix;
}

//-----------------------------------------------------------------------------

bool fromVTKMatrix( vtkMatrix4x4* _matrix, ::fwData::TransformationMatrix3D::sptr _transfoMatrix)
{
    SLM_TRACE_FUNC();
    bool res = true;
    for(int l = 0; l<4; l++)
    {
        for(int c = 0; c <4; c++)
        {
            _transfoMatrix->setCoefficient(l,c, _matrix->GetElement(l,c));
        }
    }
    return res;
}

//-----------------------------------------------------------------------------

} // namespace fwVtkIO
