/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKIO_VTK_HPP_
#define _VTKIO_VTK_HPP_

#include <boost/shared_ptr.hpp>

#include <fwData/Image.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/TransfertFunction.hpp>

#include "vtkIO/config.hpp"


// forward declaration
class vtkPolyData;
class vtkImageData;
class vtkImageImport;
class vtkMatrix4x4;
class vtkLookupTable;

/**
 * @brief   Namespace vtkIO.
 * @namespace   vtkIO.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 *
 *  The namespace vtkIO contains reader, writer and helper using the VTK lib for output and input actions between data and the frameWork
 */
namespace vtkIO
{

/**
 * @brief Allocate a new vtkImageData* from a ::fwData::Image::sptr.
 *
 * @param[in] _data ::fwData::Image::sptr.
 * @param[out] dst, the vtk image to fill if provided
 * @return vtkImageData*.
 *
 * if dst is provided the new image will be imported into the dst image else
 * allocate a new vtkImage from a ::fwData::Image, vtkImage doesn't manage its buffer (the buffer is not copied)
 *
 */
VTKIO_API vtkImageData*  toVTKImage( ::fwData::Image::sptr _data ,  vtkImageData *dst=NULL);

/*!
 * @brief Convert a vtkImageData* to a ::fwData::Image::sptr.
 *
 * @param[in] _source vtkImageData*.
 * @param[out] _destination ::fwData::Image::sptr.
 * @return bool.
 *
 * Returns \b true if the conversion is a success and \b false if it fails
 */
VTKIO_API bool fromVTKImage( vtkImageData* _source, ::fwData::Image::sptr _destination );

/*!
 * @brief Convert a ::fwData::Image::sptr to a vtkImageImport*.
 *
 * @param[in] _data ::fwData::Image::sptr.
 * @return vtkImageImport*.
 */
VTKIO_API vtkImageImport* convertToVTKImageImport( ::fwData::Image::sptr _data );

/*!
 * @brief Configure a vtkImageImport* from a ::fwData::Image::sptr.
 *
 * @param[in] _pDataImage ::fwData::Image::sptr.
 * @param[out] _pImageImport ::vtkImageImport*.
 */
VTKIO_API void configureVTKImageImport( ::vtkImageImport * _pImageImport, ::fwData::Image::sptr _pDataImage );

/*!
 * @brief Convert a ::fwData::TriangularMesh::sptr to a vtkPolyData*.
 *
 * @param[in] _mesh ::fwData::TriangularMesh::sptr.
 * @return vtkPolyData*.
 */
VTKIO_API vtkPolyData*  toVTKMesh( ::fwData::TriangularMesh::sptr _mesh );

/*!
 * @brief Convert a vtkPolyData* to a ::fwData::TriangularMesh::sptr.
 *
 * @param[in] _polyData vtkPolyData*.
 * @param[out] _triangularMesh ::fwData::TriangularMesh::sptr.
 * @return bool.
 *
 * Returns \b true if the conversion is a success and \b false if it fails
 */
VTKIO_API  bool fromVTKMesh( vtkPolyData *_polyData, ::fwData::TriangularMesh::sptr _triangularMesh );


/*
 * @brief compute the volume of the mesh using MassProperties vtk class
 */
VTKIO_API  double computeVolume(  ::fwData::TriangularMesh::sptr _triangularMesh );

/*
* @brief compute the volume of the mesh using ImageStencil
*/
VTKIO_API  double computeVolumeWithStencil(  ::fwData::TriangularMesh::sptr _triangularMesh );

/*!
 * @brief Convert a ::fwData::TransformationMatrix3D::sptr to a vtkMatrix4x4*.
 *
 * @param[in] _transfoMatrix ::fwData::TransformationMatrix3D::sptr.
 * @return vtkPolyData*.
 */
VTKIO_API vtkMatrix4x4 *  toVTKMatrix(  ::fwData::TransformationMatrix3D::sptr _transfoMatrix );

/*!
 * @brief Convert a vtkMatrix4x4* to a ::fwData::TransformationMatrix3D::sptr.
 *
 * @param[in] _matrix vtkMatrix4x4*.
 * @param[out] _transfoMatrix ::fwData::TransformationMatrix3D::sptr.
 * @return bool.
 *
 * Returns \b true if the conversion is a success and \b false if it fails
 */
VTKIO_API  bool fromVTKMatrix( vtkMatrix4x4* _matrix,  ::fwData::TransformationMatrix3D::sptr _transfoMatrix);

// not implemented since unused : perhaps when image edition using VTK is possible
//void DATACONVERT_API ::fwData::Image::sptr toDataImage( vtkImageData *vtkIm  ) ;

/*!
 * @brief Convert a ::fwData::TransfertFunction to a vtkLookupTable.
 *
 * @param[in] _pTransfertFunctionSrc ::fwData::TransfertFunction
 * @param[out] lookupTableDst vtkLookupTable
 * @param[in] allow_transparency Enable convertion of ::fwData::TransfertFunction alpha channel
 *
 */
VTKIO_API void convertTF2vtkTF(
        ::fwData::TransfertFunction::sptr _pTransfertFunctionSrc ,
        vtkLookupTable * lookupTableDst,
        bool allow_transparency = false
        );

/*!
 * @brief Convert the range of ::fwData::TransfertFunction to a black and white vtkLookupTable.
 *
 * @param[in] _pTransfertFunctionSrc ::fwData::TransfertFunction
 * @param[out] lookupTableDst vtkLookupTable
 *
 */
VTKIO_API void convertTF2vtkTFBW(
        ::fwData::TransfertFunction::sptr _pTransfertFunctionSrc ,
        vtkLookupTable * lookupTableDst
        );

}

#endif // _VTKIO_VTK_HPP_
