/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _OPVTKMESH_VTKMESHERSERVICE_HPP_
#define _OPVTKMESH_VTKMESHERSERVICE_HPP_

#include "opVTKMesh/IMesher.hpp"
#include "opVTKMesh/config.hpp"

namespace opVTKMesh
{

class OPVTKMESH_CLASS_API VTKMesherService : public ::opVTKMesh::IMesher
{

public :

    fwCoreServiceClassDefinitionsMacro ( (VTKMesherService)(::opVTKMesh::IMesher::Baseclass) ) ;

    OPVTKMESH_API VTKMesherService() throw() ;

    OPVTKMESH_API virtual ~VTKMesherService() throw() ;

protected:

    OPVTKMESH_API virtual void starting() throw ( ::fwTools::Failed );

    OPVTKMESH_API virtual void stopping() throw ( ::fwTools::Failed );

    OPVTKMESH_API virtual void updating( fwServices::ObjectMsg::csptr _pMsg ) throw ( ::fwTools::Failed );

    OPVTKMESH_API virtual void configuring() throw ( ::fwTools::Failed );

    OPVTKMESH_API virtual void updating() throw ( ::fwTools::Failed );

    OPVTKMESH_API virtual  void info ( std::ostream &_sstream ) ;

private :

    unsigned int m_reduction;

};

} // namespace opVTKMesh


#endif // _OPVTKMESH_VTKMESHERSERVICE_HPP_
