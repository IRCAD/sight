/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _OPVTKMESH_ACTION_VTKMESHCREATION_HPP_
#define _OPVTKMESH_ACTION_VTKMESHCREATION_HPP_

#include <gui/action/IAction.hpp>


#include "opVTKMesh/config.hpp"

namespace opVTKMesh
{

namespace action
{

class OPVTKMESH_CLASS_API VTKMeshCreation : public ::gui::action::IAction
{

public :

        OPVTKMESH_API VTKMeshCreation() throw() ;

        OPVTKMESH_API virtual ~VTKMeshCreation() throw() ;

        OPVTKMESH_API void starting() throw ( ::fwTools::Failed );

        OPVTKMESH_API void stopping() throw ( ::fwTools::Failed );

        OPVTKMESH_API void updating( fwServices::ObjectMsg::csptr _pMsg ) throw ( ::fwTools::Failed );

        OPVTKMESH_API void configuring() throw ( ::fwTools::Failed );

        OPVTKMESH_API void updating() throw ( ::fwTools::Failed );

        OPVTKMESH_API void info ( std::ostream &_sstream ) ;

private :

        std::string m_imageUID;
        std::string m_meshUID;
        unsigned int m_reduction;

};


} // namespace action
} // namespace opVTKMesh


#endif // _OPVTKMESH_ACTION_VTKMESHCREATION_HPP_
