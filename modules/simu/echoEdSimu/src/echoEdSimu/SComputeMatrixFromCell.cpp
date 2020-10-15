/************************************************************************
 *
 * Copyright (C) 2014-2020 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "echoEdSimu/SComputeMatrixFromCell.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Array.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwServices/macros.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace echoEdSimu
{

fwServicesRegisterMacro( ::arServices::ISimulator, ::echoEdSimu::SComputeMatrixFromCell,
                         ::fwData::TransformationMatrix3D)

const ::fwCom::Slots::SlotKeyType SComputeMatrixFromCell::s_UPDATE_SINGLE_SLOT = "updateSingle";
const ::fwCom::Slots::SlotKeyType SComputeMatrixFromCell::s_UPDATE_BOTH_SLOT = "updateBoth";

//------------------------------------------------------------------------------
SComputeMatrixFromCell::SComputeMatrixFromCell() noexcept :
    m_nbCellsPerLine(0),
    m_cranioCaudalIndex(0),
    m_radialIndex(0)
{
    newSlot( s_UPDATE_SINGLE_SLOT, &SComputeMatrixFromCell::updateSingle, this );
    newSlot( s_UPDATE_BOTH_SLOT, &SComputeMatrixFromCell::updateBoth, this );
}

//------------------------------------------------------------------------------

SComputeMatrixFromCell::~SComputeMatrixFromCell() noexcept
{
}

//------------------------------------------------------------------------------

void SComputeMatrixFromCell::starting()
{
    m_mesh = this->getInput< ::fwData::Mesh>("radialMesh");
    SLM_ASSERT("Key 'radialMesh' is not a ::fwData::Mesh", m_mesh);

    const auto dumpLock = m_mesh->lock();

    const auto points = m_mesh->begin< ::fwData::iterator::ConstPointIterator >();
    auto cellItr      = m_mesh->begin< ::fwData::iterator::ConstCellIterator >();

    /// Get the first cell coordinates
    const auto index = cellItr->pointIdx[0];

    const auto point = points + index;
    const int z      = static_cast<int>(point->point->z);

    /// Find the first cell of the second line to know the number of cell per line.
    int currentZ;
    do
    {
        ++m_nbCellsPerLine;
        ++cellItr;

        const auto index = cellItr->pointIdx[0];

        const auto point = points + index;
        currentZ = static_cast<int>(point->point->z);
    }
    while ( currentZ == z);
}

//------------------------------------------------------------------------------

void SComputeMatrixFromCell::stopping()
{
}

//------------------------------------------------------------------------------

void SComputeMatrixFromCell::configuring()
{
}

//------------------------------------------------------------------------------

void SComputeMatrixFromCell::updating()
{
}

//------------------------------------------------------------------------------

void SComputeMatrixFromCell::updateSingle(int i, std::string key)
{
    if(key == "craniocaudal")
    {
        updateBoth(i, m_radialIndex);
    }
    else if(key == "radial")
    {
        updateBoth(m_cranioCaudalIndex, i);
    }
}

//------------------------------------------------------------------------------

void SComputeMatrixFromCell::updateBoth(int i, int j)
{
    SLM_FATAL_IF("Missing mesh, maybe the service has not been updated after start ?", !m_mesh);

    m_cranioCaudalIndex = i;
    m_radialIndex       = j;

    const unsigned int nbrCells = static_cast<unsigned int>(m_mesh->getNumberOfCells());
    unsigned int indexCell      = static_cast<unsigned int>(j) + static_cast<unsigned int>(i)* m_nbCellsPerLine;

    if (indexCell >= nbrCells)
    {
        indexCell = nbrCells -1;
    }
    const auto cell   = m_mesh->begin< ::fwData::iterator::ConstCellIterator >() + indexCell;
    const auto points = m_mesh->begin< ::fwData::iterator::ConstPointIterator >();

    ::fwData::Mesh::NormalValueType nx = cell->normal->nx;
    ::fwData::Mesh::NormalValueType ny = cell->normal->ny;
    ::fwData::Mesh::NormalValueType nz = cell->normal->nz;

    ::fwData::Mesh::PointValueType cellPoints[4][3];
    for(int i = 0; i < 4; ++i)
    {
        const auto index = cell->pointIdx[i];

        const auto point = points + index;
        cellPoints[i][0] = static_cast<int>(point->point->x);
        cellPoints[i][1] = static_cast<int>(point->point->y);
        cellPoints[i][2] = static_cast<int>(point->point->z);
    }

    ::fwData::TransformationMatrix3D::sptr matrix        = this->getInOut< ::fwData::TransformationMatrix3D >("matrix");
    ::fwData::TransformationMatrix3D::TMCoefArray& coefs = matrix->getCoefficients();

    ::fwData::Mesh::PointValueType barycenter[3] = {0., 0., 1.};
    for(int i = 0; i < 4; ++i)
    {
        barycenter[0] = (barycenter[0] * (i) + cellPoints[i][0]) / (i + 1);
        barycenter[1] = (barycenter[1] * (i) + cellPoints[i][1]) / (i + 1);
        barycenter[2] = (barycenter[2] * (i) + cellPoints[i][2]) / (i + 1);
    }

    ::fwData::Mesh::PointValueType zVector[3] = {0., 0., 1.};

    ::glm::vec3 yAxis(nx, ny, nz);
    ::glm::vec3 zAxis(zVector[0], zVector[1], zVector[2]);
    ::glm::vec3 xAxis;

    xAxis = ::glm::cross(yAxis, zAxis);
    zAxis = ::glm::cross(xAxis, yAxis);

    yAxis = ::glm::normalize(yAxis);
    zAxis = ::glm::normalize(zAxis);
    xAxis = ::glm::normalize(xAxis);

    coefs[0] = xAxis[0];
    coefs[4] = xAxis[1];
    coefs[8] = xAxis[2];

    coefs[1] = yAxis[0];
    coefs[5] = yAxis[1];
    coefs[9] = yAxis[2];

    coefs[0] = xAxis[0];
    coefs[4] = xAxis[1];
    coefs[8] = xAxis[2];

    coefs[1] = yAxis[0];
    coefs[5] = yAxis[1];
    coefs[9] = yAxis[2];

    coefs[2]  = zAxis[0];
    coefs[6]  = zAxis[1];
    coefs[10] = zAxis[2];

    coefs[3]  = barycenter[0];
    coefs[7]  = barycenter[1];
    coefs[11] = barycenter[2];

    auto sig = matrix->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

} // namespace echoEdSimu
