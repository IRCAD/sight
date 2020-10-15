/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "vtkSimpleMesh/SSimpleMeshDeformation.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Mesh.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectReadToWriteLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/Mesh.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <functional>

fwServicesRegisterMacro( ::fwServices::IController, ::vtkSimpleMesh::SSimpleMeshDeformation, ::fwData::Mesh )

namespace vtkSimpleMesh
{

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SSimpleMeshDeformation::s_START_DEFORMATION_SLOT = "startDeformation";
const ::fwCom::Slots::SlotKeyType SSimpleMeshDeformation::s_STOP_DEFORMATION_SLOT  = "stopDeformation";

static const std::string s_MESH_KEY = "mesh";

//-----------------------------------------------------------------------------

SSimpleMeshDeformation::SSimpleMeshDeformation() noexcept
{
    newSlot(s_START_DEFORMATION_SLOT, &SSimpleMeshDeformation::startDeformation, this);
    newSlot(s_STOP_DEFORMATION_SLOT, &SSimpleMeshDeformation::stopDeformation, this);
}

//-----------------------------------------------------------------------------

SSimpleMeshDeformation::~SSimpleMeshDeformation() noexcept
{
}

//-----------------------------------------------------------------------------

void SSimpleMeshDeformation::starting()
{
    SLM_ASSERT("No valid worker for mesh deformation", m_associatedWorker);
    m_timer = m_associatedWorker->createTimer();

    ::fwThread::Timer::TimeDurationType duration = std::chrono::milliseconds(200);

    m_timer->setFunction(  std::bind( &SSimpleMeshDeformation::updating, this)  );
    m_timer->setDuration(duration);
}

//-----------------------------------------------------------------------------

void SSimpleMeshDeformation::configuring()
{
}

//-----------------------------------------------------------------------------

void SSimpleMeshDeformation::stopping()
{
    this->stopDeformation();
}

//-----------------------------------------------------------------------------

void SSimpleMeshDeformation::updating()
{

    auto mesh = this->getInOut< ::fwData::Mesh >(s_MESH_KEY);

    ::fwData::mt::ObjectReadToWriteLock lock(mesh);
    if ( mesh->getNumberOfPoints() > 0 )
    {
        m_hiRestimer.reset();
        m_hiRestimer.start();
        this->computeDeformation(m_mesh, m_transformMesh);
        m_hiRestimer.stop();
        SLM_INFO("Deformation time (milli sec) = " << m_hiRestimer.getElapsedTimeInMilliSec());

        lock.upgrade();
        m_hiRestimer.reset();
        m_hiRestimer.start();
        this->copyMesh(m_transformMesh, mesh);
        m_hiRestimer.stop();
        SLM_INFO("Copy time (milli sec) = " << m_hiRestimer.getElapsedTimeInMilliSec());
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

void SSimpleMeshDeformation::copyMesh( const ::fwData::Mesh::csptr& src, const ::fwData::Mesh::sptr& dest ) const
{
    const auto srcDumpLock  = src->lock();
    const auto destDumpLock = dest->lock();

    auto srcItr       = src->begin< ::fwData::iterator::ConstPointIterator >();
    const auto srcEnd = src->end< ::fwData::iterator::ConstPointIterator >();
    auto destItr      = dest->begin< ::fwData::iterator::PointIterator >();

    for (; srcItr != srcEnd; ++srcItr, ++destItr)
    {
        destItr->point->x = srcItr->point->x;
        destItr->point->y = srcItr->point->y;
        destItr->point->z = srcItr->point->z;

        destItr->normal->nx = srcItr->normal->nx;
        destItr->normal->ny = srcItr->normal->ny;
        destItr->normal->nz = srcItr->normal->nz;

        destItr->rgba->r = srcItr->rgba->r;
        destItr->rgba->g = srcItr->rgba->g;
        destItr->rgba->b = srcItr->rgba->b;
        destItr->rgba->a = srcItr->rgba->a;
    }
}

//-----------------------------------------------------------------------------

void SSimpleMeshDeformation::computeDeformation (
    const ::fwData::Mesh::csptr& refMesh,
    const ::fwData::Mesh::sptr& transformMesh,
    float deformationPercent )
{
    SLM_ASSERT("Deformation range must be equal to [0,1]", 0 <= deformationPercent && deformationPercent <= 1 );

    const float maxDeformation = 15/100.0f;
    const float center         = 2/3.0f;

    const auto refDumpLock       = refMesh->lock();
    const auto transformDumpLock = transformMesh->lock();

    auto refItr       = refMesh->begin< ::fwData::iterator::ConstPointIterator >();
    const auto refEnd = refMesh->end< ::fwData::iterator::ConstPointIterator >();
    auto transformItr = transformMesh->begin< ::fwData::iterator::PointIterator >();

    // Compute limits
    float ymin = refItr->point->y;
    float ymax = ymin;
    for (; refItr != refEnd; ++refItr)
    {
        const float val = refItr->point->y;
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
    const float sizeRef       = ymax-ymin;
    const float yref          = sizeRef * center + ymin;
    const float strafe        = maxDeformation * sizeRef;
    const float currentStrafe = deformationPercent * strafe;

    refItr = refMesh->begin< ::fwData::iterator::ConstPointIterator >();

    for (; refItr != refEnd; ++refItr, ++transformItr)
    {
        const float y = refItr->point->y;
        if( y < yref )
        {
            float val = ( yref - y ) / ( yref - ymin ) * currentStrafe;
            transformItr->point->y = y - val;
            transformItr->rgba->r  = 255;
            transformItr->rgba->g  = static_cast<fwData::Mesh::ColorValueType>(255 - 255 * ( val / strafe ));
            transformItr->rgba->b  = static_cast<fwData::Mesh::ColorValueType>(255 - 255 * ( val / strafe ));
            transformItr->rgba->a  = 255;
        }
        else
        {
            transformItr->rgba->r = 255;
            transformItr->rgba->g = 255;
            transformItr->rgba->b = 255;
            transformItr->rgba->a = 255;
        }
    }

    ::fwDataTools::Mesh::generatePointNormals(transformMesh);
}

//-----------------------------------------------------------------------------

void SSimpleMeshDeformation::computeDeformation(
    const ::fwData::Mesh::csptr& refMesh,
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
        ::fwData::mt::ObjectWriteLock lock(mesh);

        ::fwDataTools::Mesh::generatePointNormals(mesh);
        if (!mesh->hasPointColors())
        {
            mesh->resize(mesh->getNumberOfPoints(), mesh->getNumberOfCells(), mesh->getCellDataSize(),
                         ::fwData::Mesh::Attributes::POINT_COLORS);
            ::fwDataTools::Mesh::colorizeMeshPoints(mesh, 255, 255, 255);
        }

        m_mesh = ::fwData::Object::copy( mesh );

        m_transformMesh = ::fwData::Object::copy( mesh );
    }
}

//-----------------------------------------------------------------------------

}
