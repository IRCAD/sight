/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "basicRegistration/STransformMesh.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwCore/spyLog.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/Mesh.hpp>
#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwServices/macros.hpp>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

using fwTools::fwID;

fwServicesRegisterMacro( ::fwServices::IController, ::basicRegistration::STransformMesh, ::fwData::Composite );


namespace basicRegistration
{

STransformMesh::STransformMesh()
{
}

STransformMesh::~STransformMesh()
{
}

void STransformMesh::configuring() throw ( ::fwTools::Failed )
{
}

void STransformMesh::starting() throw ( ::fwTools::Failed )
{
}

void STransformMesh::stopping() throw ( ::fwTools::Failed )
{
}

void STransformMesh::updating() throw ( ::fwTools::Failed )
{
    // Get fw4spl data
    ::fwData::TransformationMatrix3D::sptr transform = ::fwData::TransformationMatrix3D::New();

    double angle    = 10;
    double angleRad = angle*M_PI/180.0;

    double cosAngle = cos(angleRad);
    double sinAngle = sin(angleRad);

    transform->setCoefficient(0,0, cosAngle); transform->setCoefficient(0,1, -sinAngle); transform->setCoefficient(0,2,
                                                                                                                   0);
    transform->setCoefficient(0,3, 0);
    transform->setCoefficient(1,0, sinAngle); transform->setCoefficient(1,1, cosAngle);  transform->setCoefficient(1,2,
                                                                                                                   0);
    transform->setCoefficient(1,2, 0);
    transform->setCoefficient(2,0, 0);        transform->setCoefficient(2,1, 0);         transform->setCoefficient(2,2,
                                                                                                                   1);
    transform->setCoefficient(2,3, 10);
    transform->setCoefficient(3,0, 0);        transform->setCoefficient(3,1, 0);         transform->setCoefficient(3,2,
                                                                                                                   0);
    transform->setCoefficient(3,3, 1);

    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::dynamicCast( ::fwTools::fwID::getObject("myToolMesh") );
    ::fwDataTools::Mesh::transform( mesh, transform );

    // Notify reading
    ::fwData::Mesh::VertexModifiedSignalType::sptr sig;
    sig = mesh->signal< ::fwData::Mesh::VertexModifiedSignalType >(::fwData::Mesh::s_VERTEX_MODIFIED_SIG);
    sig->asyncEmit();
}

void STransformMesh::swapping() throw ( ::fwTools::Failed )
{
    // Classic default approach to update service when oject change
    this->stopping();
    this->starting();
}

} // namespace basicRegistration


