/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "echoEdSimu/SProbeMesh.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/Mesh.hpp>

#include <fwDataTools/Mesh.hpp>
#include <fwDataTools/helper/Array.hpp>

#include <fwMath/VectorFunctions.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>

#include <boost/math/constants/constants.hpp>

namespace echoEdSimu
{

fwServicesRegisterMacro( ::arServices::ISimulator, ::echoEdSimu::SProbeMesh, ::fwData::Mesh );

//-----------------------------------------------------------------------------

SProbeMesh::SProbeMesh() throw()
{
}

//------------------------------------------------------------------------------

SProbeMesh::~SProbeMesh() throw()
{
}

//------------------------------------------------------------------------------

void SProbeMesh::updating() throw(::fwTools::Failed)
{
    ::fwData::Mesh::sptr mesh = this->getObject< ::fwData::Mesh >();

    const size_t depth   = 100;
    const size_t width   = 350;
    const size_t height  = 350;
    const int angle      = 60;
    const int deltaDepth = 40;
    // compute delta angle
    const double thetaInit = (90.- angle/2.) * ::boost::math::constants::pi<double>() / 180.;
    const double thetaEnd  = (90.+ angle/2.) * ::boost::math::constants::pi<double>() / 180.;
    const double dteta     = (thetaEnd - thetaInit )/ (width-1.);

    // compute delta lengths
    const double dDepth = depth / (height-1.);

    const fwVec3d centerPosition = {{0., 0., 0.}};
    const fwVec3d direction      = {{0., 1., 0.}};
    const fwVec3d normal         = {{1., 0., 0.}};

    const size_t numPointsTotal = width * height;
    const size_t numQuads       = (width -1 ) * (height - 1 );

    mesh->allocate(numPointsTotal, numQuads, 0);
    mesh->setNumberOfPoints(numPointsTotal);
    mesh->setNumberOfCells(numQuads);
    mesh->setCellDataSize(numQuads*4);
    mesh->adjustAllocatedMemory();

    // allocate memory for texture coordinates
    mesh->allocateCellTexCoords();
    mesh->allocatePointTexCoords();

    // points position
    ::fwDataTools::helper::Array points(mesh->getPointsArray());
    ::fwData::Mesh::PointValueType* ptsBuffer = static_cast< ::fwData::Mesh::PointValueType* >( points.getBuffer() );

    // cells index (4 in a row)
    ::fwDataTools::helper::Array cells( mesh->getCellDataArray());
    ::fwData::Mesh::CellValueType* cellsBuffer = static_cast< ::fwData::Mesh::CellValueType* >( cells.getBuffer() );

    // type of the cell
    ::fwDataTools::helper::Array cellTypes(mesh->getCellTypesArray());
    ::fwData::Mesh::CellTypes* cellsTypeBuffer = static_cast< ::fwData::Mesh::CellTypes* >( cellTypes.getBuffer() );

    // offset +4 for quad
    ::fwDataTools::helper::Array cellDataOffsets(mesh->getCellDataOffsetsArray());
    ::fwData::Mesh::CellDataOffsetType* cellsOffBuffer =
        static_cast< ::fwData::Mesh::CellDataOffsetType* >( cellDataOffsets.getBuffer() );

    // index for each cell
    ::fwData::Mesh::CellDataOffsetType idCell = 0;

    ::fwData::Mesh::Id idx1, idx2, idx3, idx4;
    for ( size_t i = 0; i < width - 1; ++i )
    {
        for ( size_t j = 0; j < height - 1; ++j )
        {
            idx1 = j + i * height;
            idx2 = idx1 + 1;
            idx4 = idx1 + height;
            idx3 = idx4 + 1;

            *cellsOffBuffer++ = idCell; //offset 0, 4, 8, etc... for triangles
            idCell           += 4;

            *cellsBuffer++ = (::fwData::Mesh::CellValueType) idx1;
            *cellsBuffer++ = (::fwData::Mesh::CellValueType) idx2;
            *cellsBuffer++ = (::fwData::Mesh::CellValueType) idx3;
            *cellsBuffer++ = (::fwData::Mesh::CellValueType) idx4;

            *cellsTypeBuffer++ = ::fwData::Mesh::QUAD;
        }
    }

    for (unsigned int widthGrid = 0; widthGrid < width; ++widthGrid)
    {
        fwVec3d directionLive;
        fwVec3d centerLive;

        const double angleLive = thetaInit + dteta*(width - widthGrid - 1);
        directionLive = std::cos(angleLive)*normal + std::sin(angleLive)*direction;
        centerLive    = centerPosition;

        for (unsigned int depthGrid = 0; depthGrid < height; ++depthGrid)
        {
            const fwVec3d posReal = centerLive + ( depthGrid*dDepth + deltaDepth) * directionLive;

            *ptsBuffer++ = static_cast<float>(posReal[0]);
            *ptsBuffer++ = static_cast<float>(posReal[1]);
            *ptsBuffer++ = static_cast<float>(posReal[2]);
        }
    }

    // cell tex coord
    ::fwDataTools::helper::Array cellTexCoords(mesh->getCellTexCoordsArray());
    ::fwData::Mesh::TexCoordValueType* bufferCellTexCoord =
        static_cast< ::fwData::Mesh::TexCoordValueType* >( cellTexCoords.getBuffer() );

    const size_t nbrCellCoords = mesh->getNumberOfCells();

    cellsBuffer = static_cast< ::fwData::Mesh::CellValueType* >( cells.getBuffer() );

    for ( size_t i = 0; i < nbrCellCoords; ++i )
    {
        *bufferCellTexCoord++ = static_cast< ::fwData::Mesh::TexCoordValueType >(*cellsBuffer++);
    }

    // point tex coord
    ::fwDataTools::helper::Array pointTexCoords(mesh->getPointTexCoordsArray());
    ::fwData::Mesh::TexCoordValueType* bufferPointTexCoord =
        static_cast< ::fwData::Mesh::TexCoordValueType* >( pointTexCoords.getBuffer() );

    for ( size_t i = 0; i < width; ++i )
    {
        for ( size_t j = 0; j < height; ++j )
        {
            *bufferPointTexCoord++ = i / static_cast< ::fwData::Mesh::TexCoordValueType> (width -1);
            *bufferPointTexCoord++ = j / static_cast< ::fwData::Mesh::TexCoordValueType >(height-1);
        }
    }

    ::fwDataTools::Mesh::generatePointNormals(mesh);
    ::fwDataTools::Mesh::generateCellNormals(mesh);

    auto sig = mesh->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    sig->asyncEmit();


}

//------------------------------------------------------------------------------

void SProbeMesh::stopping() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SProbeMesh::configuring() throw(fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SProbeMesh::starting() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SProbeMesh::swapping() throw(::fwTools::Failed)
{

}

//------------------------------------------------------------------------------

} // namespace echoEdSimu
