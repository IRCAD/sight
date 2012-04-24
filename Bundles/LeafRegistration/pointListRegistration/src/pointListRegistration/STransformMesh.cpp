/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/spyLog.hpp>

// Service associated data
#include <fwData/Composite.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/IEditionService.hpp>

#include <fwComEd/TransformationMatrix3DMsg.hpp>
#include <fwComEd/MeshMsg.hpp>
#include <fwComEd/Dictionary.hpp>

#include <fwDataTools/MeshGenerator.hpp>

#include <arlcore/PointsList.h>

// Services tools
#include <fwServices/Base.hpp>

#include "pointListRegistration/STransformMesh.hpp"

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

using fwTools::fwID;

REGISTER_SERVICE( ::fwServices::IController, ::pointListRegistration::STransformMesh, ::fwData::Composite );


namespace pointListRegistration
{

STransformMesh::STransformMesh()
{}

STransformMesh::~STransformMesh()
{}

void STransformMesh::configuring() throw ( ::fwTools::Failed )
{
}

void STransformMesh::starting() throw ( ::fwTools::Failed )
{}

void STransformMesh::stopping() throw ( ::fwTools::Failed )
{}

void STransformMesh::updating() throw ( ::fwTools::Failed )
{
    // Get fw4spl data
    ::fwData::TransformationMatrix3D::NewSptr transform;

    double angle = 10;
    double angleRad = angle*M_PI/180.0;

    double cosAngle = cos(angleRad);
    double sinAngle = sin(angleRad);

    transform->setCoefficient(0,0, cosAngle); transform->setCoefficient(0,1, -sinAngle); transform->setCoefficient(0,2, 0); transform->setCoefficient(0,3, 0);
    transform->setCoefficient(1,0, sinAngle); transform->setCoefficient(1,1, cosAngle);  transform->setCoefficient(1,2, 0); transform->setCoefficient(1,2, 0);
    transform->setCoefficient(2,0, 0);        transform->setCoefficient(2,1, 0);         transform->setCoefficient(2,2, 1); transform->setCoefficient(2,3, 10);
    transform->setCoefficient(3,0, 0);        transform->setCoefficient(3,1, 0);         transform->setCoefficient(3,2, 0); transform->setCoefficient(3,3, 1);

    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::dynamicCast( ::fwTools::fwID::getObject("myToolMesh") );
    ::fwDataTools::MeshGenerator::transform( mesh, transform );

    // Notify reading
    ::fwComEd::MeshMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::MeshMsg::NEW_MESH );
    ::fwServices::IEditionService::notify( this->getSptr(), mesh, msg );
}

void STransformMesh::updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{}

void STransformMesh::swapping() throw ( ::fwTools::Failed )
{
    // Classic default approach to update service when oject change
    this->stopping();
    this->starting();
}

} // namespace pointListRegistration


