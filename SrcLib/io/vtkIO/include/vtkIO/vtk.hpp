/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKIO_VTK_HPP_
#define _VTKIO_VTK_HPP_

#include <boost/shared_ptr.hpp>

#include <fwData/Image.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include "vtkIO/config.hpp"


// forward declaration
class vtkPolyData;
class vtkImageData;
class vtkImageImport;
class vtkMatrix4x4;

/**
 * @brief	Namespace vtkIO.
 * @namespace	vtkIO.
 * @author	IRCAD (Research and Development Team).
 * @date	2009.
 *
 *  The namespace vtkIO contains reader, writer and helper using the VTK lib for output and input actions between data and the frameWork
 */
namespace vtkIO
{

/**
 * @brief Allocate a new vtkImageData* from a ::boost::shared_ptr< ::fwData::Image >.
 *
 * @param[in] _data ::boost::shared_ptr< ::fwData::Image >.
 * @param[out] dst, the vtk image to fill if provided
 * @return vtkImageData*.
 *
 * if dst is provided the new image will be imported into the dst image else
 * allocate a new vtkImage from a ::fwData::Image, vtkImage doesn't manage its buffer (the buffer is not copied)
 *
 */
VTKIO_API vtkImageData*  toVTKImage( ::boost::shared_ptr< ::fwData::Image > _data ,  vtkImageData *dst=NULL);

/*!
 * @brief Convert a vtkImageData* to a ::boost::shared_ptr< ::fwData::Image >.
 *
 * @param[in] _source vtkImageData*.
 * @param[out] _destination ::boost::shared_ptr< ::fwData::Image >.
 * @return bool.
 *
 * Returns \b true if the conversion is a success and \b false if it fails
 */
VTKIO_API bool fromVTKImage( vtkImageData* _source, ::boost::shared_ptr< ::fwData::Image > _destination );

/*!
 * @brief Convert a ::boost::shared_ptr< ::fwData::Image > to a vtkImageImport*.
 *
 * @param[in] _data ::boost::shared_ptr< ::fwData::Image >.
 * @return vtkImageImport*.
 */
VTKIO_API vtkImageImport* convertToVTKImageImport( ::boost::shared_ptr< ::fwData::Image > _data );

/*!
 * @brief Configure a vtkImageImport* from a ::boost::shared_ptr< ::fwData::Image >.
 *
 * @param[in] _pDataImage ::boost::shared_ptr< ::fwData::Image >.
 * @param[out] _pImageImport ::vtkImageImport*.
 */
VTKIO_API void configureVTKImageImport( ::vtkImageImport * _pImageImport, ::boost::shared_ptr< ::fwData::Image > _pDataImage );

/*!
 * @brief Convert a ::boost::shared_ptr< ::fwData::TriangularMesh > to a vtkPolyData*.
 *
 * @param[in] _mesh ::boost::shared_ptr< ::fwData::TriangularMesh >.
 * @return vtkPolyData*.
 */
VTKIO_API vtkPolyData*  toVTKMesh( ::boost::shared_ptr< ::fwData::TriangularMesh > _mesh );

/*!
 * @brief Convert a vtkPolyData* to a ::boost::shared_ptr< ::fwData::TriangularMesh >.
 *
 * @param[in] _polyData vtkPolyData*.
 * @param[out] _triangularMesh ::boost::shared_ptr< ::fwData::TriangularMesh >.
 * @return bool.
 *
 * Returns \b true if the conversion is a success and \b false if it fails
 */
VTKIO_API  bool fromVTKMesh( vtkPolyData *_polyData, ::boost::shared_ptr< ::fwData::TriangularMesh > _triangularMesh );


/*
 * @brief compute the volume of the mesh using MassProperties vtk class
 */
VTKIO_API  double computeVolume(  ::boost::shared_ptr< ::fwData::TriangularMesh > _triangularMesh );

/*
* @brief compute the volume of the mesh using ImageStencil
*/
VTKIO_API  double computeVolumeWithStencil(  ::boost::shared_ptr< ::fwData::TriangularMesh > _triangularMesh );

/*!
 * @brief Convert a ::boost::shared_ptr< ::fwData::TransformationMatrix3D > to a vtkMatrix4x4*.
 *
 * @param[in] _transfoMatrix ::boost::shared_ptr< ::fwData::TransformationMatrix3D >.
 * @return vtkPolyData*.
 */
VTKIO_API vtkMatrix4x4 *  toVTKMatrix(  ::boost::shared_ptr< ::fwData::TransformationMatrix3D > _transfoMatrix );

/*!
 * @brief Convert a vtkMatrix4x4* to a ::boost::shared_ptr< ::fwData::TransformationMatrix3D >.
 *
 * @param[in] _matrix vtkMatrix4x4*.
 * @param[out] _transfoMatrix ::boost::shared_ptr< ::fwData::TransformationMatrix3D >.
 * @return bool.
 *
 * Returns \b true if the conversion is a success and \b false if it fails
 */
VTKIO_API  bool fromVTKMatrix( vtkMatrix4x4* _matrix,  ::boost::shared_ptr< ::fwData::TransformationMatrix3D > _transfoMatrix);

// not implemented since unused : perhaps when image edition using VTK is possible
//void DATACONVERT_API ::boost::shared_ptr< ::fwData::Image > toDataImage( vtkImageData *vtkIm  ) ;

}

#endif // _VTKIO_VTK_HPP_
