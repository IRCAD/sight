/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "vtkSimpleMesh/SSimpleMeshDeformation.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Mesh.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectReadToWriteLock.hpp>

#include <fwDataTools/Mesh.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <functional>

fwServicesRegisterMacro( ::fwServices::IController, ::vtkSimpleMesh::SSimpleMeshDeformation, ::fwData::Mesh );

namespace vtkSimpleMesh
{

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SSimpleMeshDeformation::s_START_DEFORMATION_SLOT = "startDeformation";
const ::fwCom::Slots::SlotKeyType SSimpleMeshDeformation::s_STOP_DEFORMATION_SLOT  = "stopDeformation";

static const std::string s_MESH_KEY = "mesh";

//-----------------------------------------------------------------------------

SSimpleMeshDeformation::SSimpleMeshDeformation() throw()
{
    newSlot(s_START_DEFORMATION_SLOT, &SSimpleMeshDeformation::startDeformation, this);
    newSlot(s_STOP_DEFORMATION_SLOT, &SSimpleMeshDeformation::stopDeformation, this);
}

//-----------------------------------------------------------------------------

SSimpleMeshDeformation::~SSimpleMeshDeformation() throw()
{
}

//-----------------------------------------------------------------------------

void SSimpleMeshDeformation::starting() throw(fwTools::Failed)
{
    SLM_ASSERT("No valid worker for mesh deformation", m_associatedWorker);
    m_timer = m_associatedWorker->createTimer();

    ::fwThread::Timer::TimeDurationType duration = ::boost::chrono::milliseconds(200);

    m_timer->setFunction(  std::bind( &SSimpleMeshDeformation::updating, this)  );
    m_timer->setDuration(duration);
}

//-----------------------------------------------------------------------------

void SSimpleMeshDeformation::configuring() throw(::fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void SSimpleMeshDeformation::stopping() throw(fwTools::Failed)
{
    this->stopDeformation();
}

//-----------------------------------------------------------------------------

void SSimpleMeshDeformation::updating() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    auto mesh = this->getInOut< ::fwData::Mesh >(s_MESH_KEY);

    ::fwData::mt::ObjectReadToWriteLock lock(mesh);
    if ( mesh->getNumberOfPoints() > 0 )
    {
        m_hiRestimer.reset();
        m_hiRestimer.start();
        this->computeDeformation(m_mesh, m_transformMesh);
        m_hiRestimer.stop();
        OSLM_INFO("Deformation time (milli sec) = " << m_hiRestimer.getElapsedTimeInMilliSec());

        lock.upgrade();
        m_hiRestimer.reset();
        m_hiRestimer.start();
        copyMesh(m_transformMesh, mesh);
        m_hiRestimer.stop();
        OSLM_INFO("Copy time (milli sec) = " << m_hiRestimer.getElapsedTimeInMilliSec());
        lock.downgrade();

        ::fwData::Mesh::VertexModifiedSignalType::sptr sig;
        sig = mesh->signal< ::fwData::Mesh::VertexModifiedSignalType >( ::fwData::Mesh::s_VERTEX_MODIFIED_SIG );
        sig->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

void SSimpleMeshDeformation::startDeformation()
{
    bool meshIsLoaded;
    {
        auto mesh = this->getInOut< ::fwData::Mesh >(s_MESH_KEY);
        ::fwData::mt::ObjectReadLock lock(mesh);
        meshIsLoaded = mesh->getNumberOfPoints() > 0;
    }

    if ( meshIsLoaded )
    {
        this->initMeshBackup();

        if (!m_timer->isRunning())
        {
            m_timer->start();
        }
    }
}

//-----------------------------------------------------------------------------

void SSimpleMeshDeformation::stopDeformation()
{
    if (m_timer->isRunning())
    {
        m_timer->stop();
        m_transformMesh.reset();
        m_mesh.reset();
    }
}

//-----------------------------------------------------------------------------

void SSimpleMeshDeformation::copyMesh( const ::fwData::Mesh::sptr& src, const ::fwData::Mesh::sptr& dest ) const
{
    dest->setPointsArray(::fwData::Object::copy( src->getPointsArray() ));
    dest->setPointNormalsArray(::fwData::Object::copy( src->getPointNormalsArray() ));
    dest->setPointColorsArray(::fwData::Object::copy( src->getPointColorsArray() ));
}

//-----------------------------------------------------------------------------

void SSimpleMeshDeformation::computeDeformation (
    const ::fwData::Mesh::sptr& refMesh,
    const ::fwData::Mesh::sptr& transformMesh,
    float deformationPercent )
{
    SLM_ASSERT("Deformation range must be equal to [0,1]", 0 <= deformationPercent && deformationPercent <= 1 );

    const float maxDeformation = 15/100.0f;
    const float center         = 2/3.0f;

    ::fwDataTools::helper::Mesh meshHelper(refMesh);
    ::fwDataTools::helper::Mesh transformMeshHelper(transformMesh);

    ::fwData::Mesh::PointsMultiArrayType points              = meshHelper.getPoints();
    ::fwData::Mesh::PointsMultiArrayType pointsTransform     = transformMeshHelper.getPoints();
    ::fwData::Mesh::PointColorsMultiArrayType colorTransform = transformMeshHelper.getPointColors();

    size_t nbPts = refMesh->getNumberOfPoints();

    // Compute limits
    float ymin = points[0][1];
    float ymax = points[0][1];
    for(size_t i = 0; i != nbPts; ++i)
    {
        const float val = points[i][1];
        if ( val < ymin )
        {
            ymin = val;
        }
        else if ( val > ymax )
        {
            ymax = val;
        }
    }

    // Compute deformation
    float sizeRef       = ymax-ymin;
    float yref          = sizeRef * center + ymin;
    float strafe        = maxDeformation * sizeRef;
    float currentStrafe = deformationPercent * strafe;

    for(size_t i = 0; i < nbPts; ++i )
    {
        float y = points[i][1];
        if( y < yref )
        {
            float val = ( yref - y ) / ( yref - ymin ) * currentStrafe;
            pointsTransform[i][1] = y - val;
            colorTransform[i][0]  = 255;
            colorTransform[i][1]  = static_cast<fwData::Mesh::ColorValueType>(255 - 255 * ( val / strafe ));
            colorTransform[i][2]  = static_cast<fwData::Mesh::ColorValueType>(255 - 255 * ( val / strafe ));
        }
        else
        {
            colorTransform[i][0] = 255;
            colorTransform[i][1] = 255;
            colorTransform[i][2] = 255;
        }
    }

    ::fwDataTools::Mesh::generatePointNormals(transformMesh);
}

//-----------------------------------------------------------------------------

void SSimpleMeshDeformation::computeDeformation(
    const ::fwData::Mesh::sptr& refMesh,
    const ::fwData::Mesh::sptr& transformMesh )
{
    const int step = 5;

    m_currentDeformation += m_currentIncrement;
    if ( m_currentDeformation == 100 )
    {
        m_currentIncrement = -step;
    }
    else if ( m_currentDeformation == 0 )
    {
        m_currentIncrement = step;
    }
    this->computeDeformation( refMesh, transformMesh, m_currentDeformation/100.0f );
}

//-----------------------------------------------------------------------------

void SSimpleMeshDeformation::initMeshBackup()
{
    SLM_WARN_IF("Data already init", m_transformMesh || m_mesh);
    if (!m_mesh && !m_transformMesh)
    {
        m_currentIncrement   = 0;
        m_currentDeformation = 0;

        auto mesh = this->getInOut< ::fwData::Mesh >(s_MESH_KEY);
        ::fwData::mt::ObjectReadToWriteLock lock(mesh);

        lock.upgrade();
        ::fwDataTools::Mesh::generatePointNormals(mesh);
        mesh->allocatePointColors( ::fwData::Mesh::RGB );
        lock.downgrade();

        m_mesh = ::fwData::Object::copy( mesh );

        m_transformMesh = ::fwData::Object::copy( mesh );
    }
}

//-----------------------------------------------------------------------------

}
