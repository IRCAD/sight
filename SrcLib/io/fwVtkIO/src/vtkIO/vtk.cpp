/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#include <cstring>
#include <functional>
#include <numeric>
#include <stdexcept>

#include <boost/assign/list_of.hpp>
#include <boost/bimap/bimap.hpp>
#include <boost/cast.hpp>

#include <vtkImageImport.h>
#include <vtkSetGet.h>

// for mesh
#include <vtkCell.h>
#include <vtkCellType.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolyDataWriter.h>
#include <vtkImageData.h>
#include <vtkImageImport.h>
#include <vtkImageExport.h>
#include <vtkMatrix4x4.h>
#include <vtkPolyDataNormals.h>
#include <vtkLookupTable.h>

#include <vtkMassProperties.h>

#include <vtkPolyDataToImageStencil.h>
#include <vtkImageStencil.h>
#include <vtkImageAccumulate.h>

#include <vtkDataSetAttributes.h>
#include <vtkDataArray.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>

#include <fwMath/MeshFunctions.hpp>

#include "vtkIO/vtk.hpp"


namespace vtkIO
{

struct DynamicTypeOrderer
{
  bool operator()(const fwTools::DynamicType & d1, const fwTools::DynamicType &d2) const
  {
    return d1.string() < d2.string();
  }
};




// BOOST 1.38 -> ::boost::bimaps::bimap<....>    note bimapS  ## BOOST 1.39 -> ::boost::bimap::bimap<....>
typedef  ::boost::bimaps::bimap<
                    ::boost::bimaps::set_of< fwTools::DynamicType , ::vtkIO::DynamicTypeOrderer  >
                  , ::boost::bimaps::set_of< int >
                   > DynamicTypeTranslator;

DynamicTypeTranslator PixelTypeTranslation = boost::assign::list_of< DynamicTypeTranslator::relation >
                                                                    ( fwTools::makeDynamicType<signed char>(),    VTK_CHAR )
                                                                    ( fwTools::makeDynamicType<unsigned char>(),  VTK_UNSIGNED_CHAR )
                                                                    ( fwTools::makeDynamicType<signed short>(),   VTK_SHORT )
                                                                    ( fwTools::makeDynamicType<unsigned short>(), VTK_UNSIGNED_SHORT )
                                                                    ( fwTools::makeDynamicType<signed int>(),     VTK_INT )
                                                                    ( fwTools::makeDynamicType<unsigned int>(),   VTK_UNSIGNED_INT )
                                                                    ( fwTools::makeDynamicType<signed long>(),    VTK_LONG )
                                                                    ( fwTools::makeDynamicType<unsigned long>(),  VTK_UNSIGNED_LONG )
                                                                    ( fwTools::makeDynamicType<float>(),          VTK_FLOAT )
                                                                    ( fwTools::makeDynamicType<double>(),         VTK_DOUBLE );

typedef  ::boost::bimaps::bimap<
                    ::boost::bimaps::set_of< fwTools::Type >
                  , ::boost::bimaps::set_of< int >
                   > TypeTranslator;

TypeTranslator TypeTranslation = boost::assign::list_of< TypeTranslator::relation >
                                                                    ( fwTools::Type::create("int8" ), VTK_CHAR )
                                                                    ( fwTools::Type::create("int8" ), VTK_SIGNED_CHAR )
                                                                    ( fwTools::Type::create("uint8" ), VTK_UNSIGNED_CHAR )

                                                                    ( fwTools::Type::create("int16"), VTK_SHORT )
                                                                    ( fwTools::Type::create("uint16"), VTK_UNSIGNED_SHORT )

                                                                    ( fwTools::Type::create("int32"), VTK_INT )
                                                                    ( fwTools::Type::create("uint32"), VTK_UNSIGNED_INT )

                                                                    ( fwTools::Type::create<long>(), VTK_LONG )
                                                                    ( fwTools::Type::create<unsigned long>(), VTK_UNSIGNED_LONG )

                                                                    ( fwTools::Type::create("int64"), VTK___INT64 )
                                                                    ( fwTools::Type::create("uint64"), VTK_UNSIGNED___INT64 )
                                                                    ( fwTools::Type::create("int64"), VTK_LONG_LONG )
                                                                    ( fwTools::Type::create("uint64"), VTK_UNSIGNED_LONG_LONG )

                                                                    ( fwTools::Type::create("float" ), VTK_FLOAT )
                                                                    ( fwTools::Type::create("double"), VTK_DOUBLE );


//-----------------------------------------------------------------------------

int getVtkScalarType(::fwData::Image::sptr image)
{
    OSLM_ASSERT("Unknown Type "<<image->getType().string(),
            TypeTranslation.left.find( image->getType() ) != TypeTranslation.left.end() );
    return TypeTranslation.left.at( image->getType() );
}

//-----------------------------------------------------------------------------

const char *myScalarTypeCallback(void *imageData)
{
    ::fwData::Image *trueImageData = static_cast< ::fwData::Image *>(imageData);
    OSLM_ASSERT("Unknown Type "<<trueImageData->getType().string(),
                TypeTranslation.left.find( trueImageData->getType() ) != TypeTranslation.left.end() );
    return vtkImageScalarTypeNameMacro( TypeTranslation.left.at( trueImageData->getType() ) );
}

//-----------------------------------------------------------------------------

void toVTKImage( ::fwData::Image::sptr data,  vtkImageData *dst)
{
    vtkSmartPointer< vtkImageImport > importer = vtkSmartPointer< vtkImageImport >::New();
    importer->SetDataSpacing( data->getSpacing().at(0),
                              data->getSpacing().at(1),
                              data->getSpacing().at(2)
                            );

    // !!!!!!!!!!!!!!!!
    // FIX - ticket #1775 : All vtk adaptor don't support yet origin for image
    // !!!!!!!!!!!!!!!!
    importer->SetDataOrigin( 0.0, //data->getCRefOrigin().at(0),
                             0.0, //data->getCRefOrigin().at(1),
                             0.0  //data->getCRefOrigin().at(2)
                            );

    const ::fwData::Image::SizeType imgSize = data->getSize();
    importer->SetWholeExtent(   0, imgSize.at(0) - 1,
                                0, imgSize.at(1) - 1,
                                0, imgSize.at(2) - 1
                            );

    // copy WholeExtent to DataExtent
    importer->SetDataExtentToWholeExtent();

    // no copy, no buffer destruction/management
    importer->SetImportVoidPointer( data->getBuffer() );
    importer->SetCallbackUserData( data.get() );
    // used to set correct pixeltype to VtkImage
    importer->SetDataScalarType( getVtkScalarType(data) );
    importer->Update();

    dst->ShallowCopy(importer->GetOutput());
}

//-----------------------------------------------------------------------------


template< typename IMAGETYPE >
void *newBuffer(size_t size)
{
    IMAGETYPE *destBuffer;
    try
    {
        destBuffer = new IMAGETYPE[ size ];
    }
    catch (std::exception &e)
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
void fromRGBBuffer( void *input, size_t size, void *&destBuffer)
{
    if(destBuffer == NULL)
    {
        destBuffer = newBuffer<IMAGETYPE>(size);
    }

    IMAGETYPE *destBufferTyped = (IMAGETYPE*)destBuffer;
    IMAGETYPE *inputTyped      = (IMAGETYPE*)input;
    IMAGETYPE *finalPtr        = ((IMAGETYPE*)destBuffer) + size;
    IMAGETYPE valR, valG,valB;

    while (destBufferTyped < finalPtr)
    {
        valR = (IMAGETYPE)(float((*(inputTyped++)) * 0.30));
        valG = (IMAGETYPE)(float((*(inputTyped++)) * 0.59));
        valB = (IMAGETYPE)(float((*(inputTyped++)) * 0.11));
        (*destBufferTyped++) = valR + valG + valB;
    }
}
//-----------------------------------------------------------------------------


void fromVTKImage( vtkImageData* source, ::fwData::Image::sptr destination )
{
    assert(destination && source );

    // ensure image size correct
    source->UpdateInformation();
    source->PropagateUpdateExtent();

    int dim = source->GetDataDimension() ;
    OSLM_TRACE("source->GetDataDimension() : " << dim);

    SLM_WARN_IF("2D Vtk image are not yet correctly managed", dim == 2);

    destination->setSize( ::fwData::Image::SizeType(source->GetDimensions(), source->GetDimensions()+dim) );
    destination->setSpacing( ::fwData::Image::SpacingType(source->GetSpacing(), source->GetSpacing()+dim) );
    destination->setOrigin( ::fwData::Image::OriginType(source->GetOrigin(), source->GetOrigin()+dim) );


    size_t size = std::accumulate(source->GetDimensions(), source->GetDimensions()+dim, 1, std::multiplies<size_t>() );
    void *input = source->GetScalarPointer();
    void *destBuffer;

    if (size != 0)
    {
        int nbBytePerPixel = source->GetScalarSize();
        int nbComponents = source->GetNumberOfScalarComponents();
        OSLM_TRACE("image size : " << size << " - nbBytePerPixel : " << nbBytePerPixel );

        if (nbComponents == 3 && nbBytePerPixel == 2)
        {
            SLM_TRACE ("RGB 16bits");

            destination->setType( "uint16" );
            destination->allocate();
            destBuffer = destination->getBuffer();
            SLM_ASSERT("Image allocation error", destBuffer != NULL);
            fromRGBBuffer< unsigned short >(input, size, destBuffer);
        }
        else if (nbComponents == 3 && nbBytePerPixel == 1)
        {
            SLM_TRACE ("RGB 8bits");

            destination->setType( "uint8" );
            destination->allocate();
            destBuffer = destination->getBuffer();
            SLM_ASSERT("Image allocation error", destBuffer != NULL);
            fromRGBBuffer< unsigned char >(input, size, destBuffer);
        }
        else if (nbComponents == 1)
        {
            SLM_TRACE ("Luminance image");
            destination->setType( TypeTranslation.right.at( source->GetScalarType() ) );
            destination->allocate();
            size_t sizeInBytes = destination->getSizeInBytes();
            std::memcpy(destBuffer, input, sizeInBytes);
        }
        else
        {
            OSLM_ERROR ("Image type not supported (image nbComponents:"<< nbComponents <<")");
        }
    }


    ::fwData::Image::SizeType imgSize = destination->getSize();
    ::fwData::Image::SpacingType spacing = destination->getSpacing();
    ::fwData::Image::OriginType origin = destination->getOrigin();
    for( ::boost::uint8_t d=0; d<dim; ++d)
    {
        OSLM_TRACE("Size["<< d << "]:" << imgSize[d]);
        OSLM_TRACE("Origin["<< d << "]:" << origin[d]);
        OSLM_TRACE("Spacing["<< d << "]:" << spacing[d]);
        origin[d]=0.0; //FIXME !!! Hack because we currently don't support image origin (visu services)
    }
}

//------------------------------------------------------------------------------

void configureVTKImageImport( ::vtkImageImport * _pImageImport, ::fwData::Image::sptr _pDataImage )
{

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

    // copy WholeExtent to DataExtent
    _pImageImport->SetDataExtentToWholeExtent();
    // no copy, no buffer destruction/management
    _pImageImport->SetImportVoidPointer( _pDataImage->getBuffer() );
    _pImageImport->SetCallbackUserData( _pDataImage.get() );
    // used to set correct pixeltype to VtkImage
    _pImageImport->SetDataScalarType( getVtkScalarType(_pDataImage) );
}

//-----------------------------------------------------------------------------

// This method is written to be as fast as possible, take care when modifying it.
vtkPolyData*  updatePolyDataPoints(vtkPolyData* polyDataDst, ::fwData::TriangularMesh::sptr meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    vtkPoints *polyDataPoints = polyDataDst->GetPoints();
    ::fwData::TriangularMesh::PointContainer &points = meshSrc->points();

    ::fwData::TriangularMesh::PointContainer::iterator pointsIter = points.begin();
    ::fwData::TriangularMesh::PointContainer::iterator pointsEnd  = points.end();

    float *xyz = 0;
    vtkIdType id = 0;
    if (points.size() != polyDataPoints->GetNumberOfPoints())
    {
        polyDataPoints->SetNumberOfPoints(points.size());
    }
    for( ; pointsIter != pointsEnd ; ++pointsIter )
    {
        xyz = &(pointsIter->front());
        polyDataPoints->SetPoint(id++, xyz);
    }
    polyDataPoints->Modified();
    return polyDataDst;
}

//-----------------------------------------------------------------------------

vtkPolyData*  toVTKMesh( ::fwData::TriangularMesh::sptr mesh )
{
    vtkPolyData *polygonGrid = vtkPolyData::New();

    if ( mesh && !mesh->points().empty() )
    {
        vtkSmartPointer< vtkPoints > trianPts = vtkSmartPointer< vtkPoints >::New();
        polygonGrid->SetPoints(trianPts);
        updatePolyDataPoints(polygonGrid, mesh);

        ::fwData::TriangularMesh::CellContainer &cells = mesh->cells();
        unsigned int nbCells = cells.size() ;
        assert( nbCells ) ;
        vtkIdType typeCell = VTK_TRIANGLE;
        polygonGrid->Allocate(typeCell,nbCells);
        for(unsigned int i=0 ; i<nbCells ; ++i )
        {
            vtkIdType cell[3];
            const std::vector<int> &meshCell = (cells[i]);
            cell[0] = meshCell[0] ;
            cell[1] = meshCell[1] ;
            cell[2] = meshCell[2] ;
            polygonGrid->InsertNextCell( typeCell, 3, cell );
        }
    }
    else
    {
        SLM_INFO("vtkPolyData build from empty mesh.")
    }

    return polygonGrid;
}

//-----------------------------------------------------------------------------

bool fromVTKMesh( vtkPolyData *polyData, ::fwData::TriangularMesh::sptr triangularMesh)
{
    SLM_TRACE_FUNC();
    bool res = false;
    vtkPoints *trianPts = polyData->GetPoints();

    if (trianPts)
    {
        // Clear the container cells and set its capacity to 0
        triangularMesh->clearCells();
        // Clear the container points and set its capacity to 0
        triangularMesh->clearPoints();

        vtkDataArray* points = trianPts->GetData();
        vtkIdType numberOfTuples = points->GetNumberOfTuples();
        std::vector<float> vPoint(3, 0.0);
        triangularMesh->points().resize(numberOfTuples, vPoint);
        double* tuple;
        for (vtkIdType i = 0; i < numberOfTuples; ++i)
        {
            tuple = points->GetTuple(i);
            ::fwData::TriangularMesh::PointContainer::value_type &vPoints = triangularMesh->points()[i];
            std::copy(tuple, tuple+3, vPoints.begin());
        }

        vtkIdType numberOfCells = polyData->GetNumberOfCells();
        std::vector<int> vCell(3, 0);
        triangularMesh->cells().resize(numberOfCells, vCell);
        vtkCell* cell;
        vtkIdList* idList;
        vtkIdType* idType;
        for (vtkIdType i = 0 ; i < numberOfCells ; ++i)
        {
            cell = polyData->GetCell(i);
            idList = cell->GetPointIds();
            idType = idList->GetPointer(0);
            ::fwData::TriangularMesh::CellContainer::value_type &vCells = triangularMesh->cells()[i];
            std::copy(idType, idType+3, vCells.begin());
        }
        res = true;
    }

    return res;
}

//-----------------------------------------------------------------------------

double computeVolume( ::fwData::TriangularMesh::sptr _triangularMesh )
{
    ::fwData::TriangularMesh::NewSptr closedMesh;
    closedMesh->deepCopy(_triangularMesh);

    ::fwMath::closeSurface(closedMesh->points(), closedMesh->cells());

    vtkPolyData*  vtkMeshRaw = toVTKMesh( closedMesh );

    vtkSmartPointer< vtkPolyDataNormals > filter = vtkSmartPointer< vtkPolyDataNormals >::New();
    filter->SetInput(vtkMeshRaw);
    filter->AutoOrientNormalsOn();
    filter->FlipNormalsOff();

    vtkSmartPointer< vtkMassProperties > calculator = vtkSmartPointer< vtkMassProperties >::New();
    calculator->SetInput( filter->GetOutput() );
    calculator->Update();
    double volume =  calculator->GetVolume();
    OSLM_DEBUG("GetVolume : " << volume << " vtkMassProperties::GetVolumeProjected = " << calculator->GetVolumeProjected() );
    OSLM_DEBUG("Error : " << (calculator->GetVolume()- fabs(calculator->GetVolumeProjected()))*10000);
    if ( (calculator->GetVolume()- fabs(calculator->GetVolumeProjected()))*10000 > calculator->GetVolume() )
    {
        std::stringstream ss;
        ss << "vtkMassProperties::GetVolume() - | vtkMassProperties::GetVolumeProjected() |";
        ss << ">  vtkMassProperties::GetVolume()/10000.0" << std::endl;
        ss << "vtkMassProperties::GetVolume() = " << volume << " vtkMassProperties::GetVolumeProjected = " << calculator->GetVolumeProjected();
        throw (std::out_of_range( ss.str() ));
    }

    vtkMeshRaw->Delete();

    return volume;
}

//-----------------------------------------------------------------------------

double computeVolumeWithStencil(  ::fwData::TriangularMesh::sptr _triangularMesh )
{
    vtkPolyData*  vtkMesh = toVTKMesh( _triangularMesh );

    vtkImageData* vi = vtkImageData::New();
    vi->SetOrigin( 0,0,0 ); // adjust these to your needs
    vi->SetSpacing( 0.5, 0.5, 0.5 ); // adjust these to your needs
    vi->SetDimensions( vtkMesh->GetBounds()[1]*2,  vtkMesh->GetBounds()[3]*2,  vtkMesh->GetBounds()[5]*2 ); // adjust these to your needs
    vi->SetScalarTypeToUnsignedChar ();
    vi->AllocateScalars();
    vi->GetPointData()->GetScalars()->FillComponent(0, 1.0);
    // outputMesh is of vtkPolyData* type and contains your mesh data
    vtkPolyDataToImageStencil* pti = vtkPolyDataToImageStencil::New();
    pti->SetInput( vtkMesh );
    vtkImageAccumulate* ac = vtkImageAccumulate::New();
    ac->SetInput( vi );
    ac->SetStencil( pti->GetOutput() );
    ac->ReverseStencilOff();
    ac->Update();

    unsigned long nbVoxel = ac->GetVoxelCount();

    pti->Delete();
    ac->Delete();
    vi->Delete();
    vtkMesh->Delete();

    return nbVoxel;
}

//-----------------------------------------------------------------------------

vtkMatrix4x4 *  toVTKMatrix( ::fwData::TransformationMatrix3D::sptr _transfoMatrix )
{
    vtkMatrix4x4  *matrix = vtkMatrix4x4 ::New();
    for(int l=0; l<4; l++)
    {
        for(int c=0; c <4; c++)
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
    for(int l=0; l<4; l++)
    {
        for(int c=0; c <4; c++)
        {
            _transfoMatrix->setCoefficient(l,c, _matrix->GetElement(l,c));
        }
    }
    return res;
}


//-----------------------------------------------------------------------------

void convertTF2vtkTF(
        ::fwData::TransfertFunction::sptr _pTransfertFunctionSrc ,
        vtkLookupTable * lookupTableDst,
        bool allow_transparency
        )
{
    SLM_TRACE_FUNC();
    //vtkWindowLevelLookupTable * lookupTable = vtkWindowLevelLookupTable::New();

    // Compute center and width
    std::pair< double, ::boost::int32_t > centerAndWidth = _pTransfertFunctionSrc->getCenterWidth();
    double width = centerAndWidth.second;

    // Compute min and max
    typedef ::fwData::TransfertFunction::TransfertFunctionPointIterator TFPCIterator;
    std::pair< TFPCIterator, TFPCIterator > range = _pTransfertFunctionSrc->getTransfertFunctionPoints();
    int min = (*range.first)->getValue();


    // Convert tf points
    //-------------------

    // Init iterator
    TFPCIterator iterTF = range.first;
    TFPCIterator iterTFNext = range.first + 1;
    TFPCIterator end = range.second;

    // Must have point in data tf
    assert( iterTF != end );

    // Init parameters
    double r, g, b, x;
    int    i          = 0;
    double alpha      = 1.0;
    double widthScale = 255.0 / width;
    int    value      = 0;


    // Set first point
    value = ((*iterTF)->getValue() - min) * widthScale;
    const ::fwData::Color::ColorArray & vRGBA0 = (*iterTF)->getColor()->getCRefRGBA();

    if(allow_transparency)
    {
        alpha = vRGBA0[3];
    }
    lookupTableDst->SetTableValue(i, vRGBA0[0], vRGBA0[1], vRGBA0[2], alpha);

    i++;

    ::fwData::Color::ColorType R, G, B, A;
    ::fwData::Color::ColorType deltaR, deltaV, deltaB, deltaA;
    int valueNext, deltaValue;

    while ( iterTFNext != end )
    {
        // First point
        const ::fwData::Color::ColorArray &vRGBA     = (*iterTF)->getColor()->getCRefRGBA();
        // Second point
        const ::fwData::Color::ColorArray &vRGBANext = (*iterTFNext)->getColor()->getCRefRGBA();


        value = ((*iterTF)->getValue() - min) * widthScale + 0.5; // + 0.5 just a hack to cap the integer
        valueNext = ((*iterTFNext)->getValue() - min) * widthScale + 0.5; // + 0.5 just a hack to cap the integer ex : replace 254.9999999999999999 by 255

        R = vRGBA[0];
        G = vRGBA[1];
        B = vRGBA[2];
        A = vRGBA[3];
        deltaR = vRGBANext[0] - vRGBA[0];
        deltaV = vRGBANext[1] - vRGBA[1];
        deltaB = vRGBANext[2] - vRGBA[2];
        deltaA = vRGBANext[3] - vRGBA[3];
        deltaValue = valueNext - value;

        // Interpolation
        if(allow_transparency)
        {
            while (i <= valueNext)
            {
                x = (double)(i - value) / (double)(deltaValue);
                r     = ( R + (deltaR * x) );
                g     = ( G + (deltaV * x) );
                b     = ( B + (deltaB * x) );
                alpha = ( A + (deltaA * x) );

                lookupTableDst->SetTableValue( i, r, g, b , alpha );
                i++;
            }
        }
        else
        {
            while (i <= valueNext)
            {
                x = (double)(i - value) / (double)(deltaValue);
                r     = ( R + (deltaR * x) );
                g     = ( G + (deltaV * x) );
                b     = ( B + (deltaB * x) );

                lookupTableDst->SetTableValue( i, r, g, b , alpha );
                i++;
            }
        }

        iterTF++;
        iterTFNext++;
    }

    lookupTableDst->SetTableRange( min, min + width );

    lookupTableDst->Build();
}

//-----------------------------------------------------------------------------

void convertTF2vtkTFBW(
        ::fwData::TransfertFunction::sptr _pTransfertFunctionSrc ,
        vtkLookupTable * lookupTableDst )
{
    SLM_TRACE_FUNC();

    // Compute center and width
    std::pair< double, ::boost::int32_t > centerAndWidth = _pTransfertFunctionSrc->getCenterWidth();
    double width = centerAndWidth.second;

    // Compute min and max
    typedef ::fwData::TransfertFunction::TransfertFunctionPointIterator TFPCIterator;
    std::pair< TFPCIterator, TFPCIterator > range = _pTransfertFunctionSrc->getTransfertFunctionPoints();
    int min = (*range.first)->getValue();


    double alpha = 1.0;
    float value;
    for( int k = 0; k < 256; k++ )
    {
        value = ((float) k)/255.0;
        lookupTableDst->SetTableValue( k, value, value, value, alpha );
    }
    lookupTableDst->SetTableRange( min, min + width );
    lookupTableDst->Build();
}

} // namespace vtkIO
