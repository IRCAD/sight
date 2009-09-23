/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_TRIANGULARMESH_HPP_
#define _FWDATA_TRIANGULARMESH_HPP_

#include <vector>

#include "fwData/config.hpp"
#include "fwData/Object.hpp"

namespace fwData
{
/**
 * @class 	TriangularMesh
 * @brief 	This class defines a mesh.
 *
 * A mesh is represented by a container of 3D points and a container of cells.
 * A cell contains indices of the three points related to a triangular cell.
 *
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 */
class FWDATA_CLASS_API TriangularMesh : public Object
{
public :
	fwCoreClassDefinitionsWithFactoryMacro( (TriangularMesh)(::fwData::Object::Baseclass), (()), ::fwTools::Factory::New< TriangularMesh >) ;

	/// 3D point container
	typedef std::vector< std::vector< float > > 	PointContainer ;
	/// A cell contains indices of the three points related to a triangular cell
	typedef std::vector< std::vector< int > > 		CellContainer ;

	/**
	 * @brief constructor
	 */
	FWDATA_API TriangularMesh();

	/**
	 * @brief destructor
	 */
	FWDATA_API virtual ~TriangularMesh() ;

	/**
	 * @brief returns editable point container
	 */
	FWDATA_API PointContainer &points() ;
	/**
	 * @brief returns editable cell container
	 */
	FWDATA_API CellContainer &cells() ;


protected :

	PointContainer		m_points ;
	CellContainer		m_cells ;

} ;

}

#endif //_FWDATA_TRIANGULARMESH_HPP_
