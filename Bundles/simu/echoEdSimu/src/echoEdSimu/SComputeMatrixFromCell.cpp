/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "echoEdSimu/SComputeMatrixFromCell.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Array.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwDataTools/helper/Array.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwServices/macros.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace echoEdSimu
{

fwServicesRegisterMacro( ::arServices::ISimulator, ::echoEdSimu::SComputeMatrixFromCell,
                         ::fwData::TransformationMatrix3D);

const ::fwCom::Slots::SlotKeyType SComputeMatrixFromCell::s_UPDATE_SINGLE_SLOT = "updateSingle";
const ::fwCom::Slots::SlotKeyType SComputeMatrixFromCell::s_UPDATE_BOTH_SLOT   = "updateBoth";

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

    ::fwData::Array::sptr cellArray = m_mesh->getCellDataArray();
    ::fwDataTools::helper::Array cellArrayHelper(cellArray);

    ::fwData::Array::sptr pointsArray = m_mesh->getPointsArray();
    ::fwDataTools::helper::Array pointsArrayHelper(pointsArray);

    /// Get the first cell coordinates
    const unsigned int indexCell = 0;
    int x, y, z;
    for(int i = 0; i < 4; ++i)
    {
        ::fwData::Array::IndexType indexCellVector;
        indexCellVector.push_back(indexCell * 4 + static_cast<unsigned int>(i));
        ::fwData::Mesh::CellValueType* cell = cellArrayHelper.getItem< ::fwData::Mesh::CellValueType>(indexCellVector);

        ::fwData::Array::IndexType indexPointsVector;
        indexPointsVector.push_back(*cell);
        x = static_cast<int>(*pointsArrayHelper.getItem< ::fwData::Mesh::PointValueType>(indexPointsVector, 0));
        y = static_cast<int>(*pointsArrayHelper.getItem< ::fwData::Mesh::PointValueType>(indexPointsVector, 1));
        z = static_cast<int>(*pointsArrayHelper.getItem< ::fwData::Mesh::PointValueType>(indexPointsVector, 2));

    }

    /// Find the first cell of the second line to know the number of cell per line.
    int currentX, currentY, currentZ;
    do
    {
        ++m_nbCellsPerLine;
        for(int i = 0; i < 4; ++i)
        {
            ::fwData::Array::IndexType indexCellVector;
            indexCellVector.push_back(m_nbCellsPerLine * 4 + static_cast<unsigned int>(i));
            ::fwData::Mesh::CellValueType* cell = cellArrayHelper.getItem< ::fwData::Mesh::CellValueType>(
                indexCellVector);

            ::fwData::Array::IndexType indexPointsVector;
            indexPointsVector.push_back(*cell);
            currentX =
                static_cast<int>(*pointsArrayHelper.getItem< ::fwData::Mesh::PointValueType>(indexPointsVector, 0));
            currentY =
                static_cast<int>(*pointsArrayHelper.getItem< ::fwData::Mesh::PointValueType>(indexPointsVector, 1));
            currentZ =
                static_cast<int>(*pointsArrayHelper.getItem< ::fwData::Mesh::PointValueType>(indexPointsVector, 2));
        }
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
    OSLM_FATAL_IF("Missing mesh, maybe the service has not been updated after start ?", !m_mesh);

    m_cranioCaudalIndex = i;
    m_radialIndex       = j;

    const unsigned int nbrCells = static_cast<unsigned int>(m_mesh->getNumberOfCells());
    unsigned int indexCell      = static_cast<unsigned int>(j) + static_cast<unsigned int>(i)* m_nbCellsPerLine;

    if (indexCell >= nbrCells)
    {
        indexCell = nbrCells -1;
    }

    ::fwData::Array::sptr normalArray = m_mesh->getCellNormalsArray();
    ::fwDataTools::helper::Array normalArrayHelper(normalArray);

    ::fwData::Array::IndexType indexVector;
    indexVector.push_back(indexCell);

    ::fwData::Mesh::NormalValueType nx = *normalArrayHelper.getItem< ::fwData::Mesh::NormalValueType>(indexVector, 0);
    ::fwData::Mesh::NormalValueType ny = *normalArrayHelper.getItem< ::fwData::Mesh::NormalValueType>(indexVector, 1);
    ::fwData::Mesh::NormalValueType nz = *normalArrayHelper.getItem< ::fwData::Mesh::NormalValueType>(indexVector, 2);

    ::fwData::Array::sptr cellArray = m_mesh->getCellDataArray();
    ::fwDataTools::helper::Array cellArrayHelper(cellArray);

    ::fwData::Array::sptr pointsArray = m_mesh->getPointsArray();
    ::fwDataTools::helper::Array pointsArrayHelper(pointsArray);

    ::fwData::Mesh::PointValueType points[4][3];
    for(int i = 0; i < 4; ++i)
    {
        ::fwData::Array::IndexType indexCellVector;
        indexCellVector.push_back(indexCell * 4 + static_cast<unsigned int>(i));
        ::fwData::Mesh::CellValueType* cell = cellArrayHelper.getItem< ::fwData::Mesh::CellValueType>(indexCellVector);

        ::fwData::Array::IndexType indexPointsVector;
        indexPointsVector.push_back(*cell);
        points[i][0] = *pointsArrayHelper.getItem< ::fwData::Mesh::PointValueType>(indexPointsVector, 0);
        points[i][1] = *pointsArrayHelper.getItem< ::fwData::Mesh::PointValueType>(indexPointsVector, 1);
        points[i][2] = *pointsArrayHelper.getItem< ::fwData::Mesh::PointValueType>(indexPointsVector, 2);
    }

    ::fwData::TransformationMatrix3D::sptr matrix        = this->getInOut< ::fwData::TransformationMatrix3D >("matrix");
    ::fwData::TransformationMatrix3D::TMCoefArray& coefs = matrix->getCoefficients();

    ::fwData::Mesh::PointValueType barycenter[3] = {0., 0., 1.};
    for(int i = 0; i < 4; ++i)
    {
        barycenter[0] = (barycenter[0] * (i) + points[i][0]) / (i + 1);
        barycenter[1] = (barycenter[1] * (i) + points[i][1]) / (i + 1);
        barycenter[2] = (barycenter[2] * (i) + points[i][2]) / (i + 1);
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
