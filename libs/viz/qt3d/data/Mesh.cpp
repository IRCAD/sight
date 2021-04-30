/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "viz/qt3d/data/Mesh.hpp"

#include "viz/qt3d/ComputeMaterial.hpp"
#include "viz/qt3d/core/GenericScene.hpp"

#include <geometry/data/Mesh.hpp>

#include <QComputeCommand>

#include <Qt3DCore/QEntity>

#include <Qt3DExtras/QPhongMaterial>

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>

namespace sight::viz::qt3d
{

namespace data
{

Mesh::Mesh(Qt3DCore::QNode* _parent) :
    QEntity(_parent)
{
    auto defaultMaterial = new viz::qt3d::data::Material();

    m_material = defaultMaterial;
    m_scene    = qobject_cast< sight::viz::qt3d::core::GenericScene* >(_parent);

    m_geomRenderer = new Qt3DRender::QGeometryRenderer(m_scene);
    m_geometry     = new Qt3DRender::QGeometry(m_geomRenderer);

    m_posBuffer    = new Qt3DRender::QBuffer(m_geometry);
    m_normalBuffer = new Qt3DRender::QBuffer(m_geometry);
    m_indexBuffer  = new Qt3DRender::QBuffer(m_geometry);

    m_posAttrib    = new Qt3DRender::QAttribute(m_geometry);
    m_normalAttrib = new Qt3DRender::QAttribute(m_geometry);
    m_indexAttrib  = new Qt3DRender::QAttribute(m_geometry);

    this->addComponent(m_geomRenderer);
    this->addComponent(m_material);
}

//------------------------------------------------------------------------------

Mesh::~Mesh()
{
}

//------------------------------------------------------------------------------

viz::qt3d::data::Material* Mesh::getMaterial() const
{
    return m_material;
}

//------------------------------------------------------------------------------

sight::viz::qt3d::core::GenericScene* Mesh::getScene() const
{
    return m_scene;
}

//------------------------------------------------------------------------------

void Mesh::setMaterial(viz::qt3d::data::Material* _material)
{
    SIGHT_ASSERT("Material can't be set to null value.", _material);
    this->removeComponent(m_material);
    m_material = _material;
    this->addComponent(m_material);
}

//------------------------------------------------------------------------------

void Mesh::setScene(sight::viz::qt3d::core::GenericScene* _scene)
{
    SIGHT_ASSERT("Scene can't be set to null value.", _scene);
    m_scene = _scene;
}

//------------------------------------------------------------------------------

void Mesh::setMesh(sight::data::Mesh::sptr _mesh)
{
    SIGHT_ASSERT("::sight::data::Mesh pointer does not exist.", _mesh);

    // Sets the number of points (for a mesh of triangles).
    m_numberOfPoints = static_cast<unsigned int>(_mesh->getNumberOfPoints());

    if(m_geometry->attributes().size() != 0)
    {
        m_geometry->removeAttribute(m_posAttrib);
        m_geometry->removeAttribute(m_normalAttrib);
        m_geometry->removeAttribute(m_indexAttrib);
    }

    // Configures rendering attributes.
    m_posAttrib->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    m_posAttrib->setBuffer(m_posBuffer);
    m_posAttrib->setVertexBaseType(Qt3DRender::QAttribute::Float);
    m_posAttrib->setVertexSize(m_vertexSize);
    m_posAttrib->setByteOffset(0);
    m_posAttrib->setByteStride(3*sizeof(float));
    m_posAttrib->setCount(m_numberOfPoints);
    m_posAttrib->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    m_geometry->addAttribute(m_posAttrib);

    m_normalAttrib->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    m_normalAttrib->setBuffer(m_normalBuffer);
    m_normalAttrib->setVertexBaseType(Qt3DRender::QAttribute::Float);
    m_normalAttrib->setVertexSize(m_vertexSize);
    m_normalAttrib->setByteOffset(0);
    m_normalAttrib->setByteStride(3*sizeof(float));
    m_normalAttrib->setCount(m_numberOfPoints);
    m_normalAttrib->setName(Qt3DRender::QAttribute::defaultNormalAttributeName());

    m_geometry->addAttribute(m_normalAttrib);

    m_indexAttrib->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    m_indexAttrib->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    m_indexAttrib->setBuffer(m_indexBuffer);
    m_indexAttrib->setCount(3 * static_cast<unsigned int>(_mesh->getNumberOfCells()));
    m_indexAttrib->setName("indexAttribute");

    m_geometry->addAttribute(m_indexAttrib);

    // Fills buffers with data extracted from _mesh.
    this->buildBuffers(_mesh);

    m_geometry->setBoundingVolumePositionAttribute(m_posAttrib);

    m_geomRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
    m_geomRenderer->setGeometry(m_geometry);
}

//------------------------------------------------------------------------------

void Mesh::centerCameraOnMesh()
{
    auto camera = m_scene->getCamera();
    camera->viewSphere(m_meshCenter,
                       std::max(std::max(m_maxExtent.x()-m_minExtent.x(), m_maxExtent.y()-m_minExtent.y()),
                                m_maxExtent.z()-m_minExtent.z()));
}

//------------------------------------------------------------------------------

void Mesh::buildBuffers(sight::data::Mesh::sptr _mesh)
{
    SIGHT_ASSERT("::sight::data::Mesh pointer does not exist.", _mesh);

    // Returns if _mesh is empty.
    if(static_cast<int>(_mesh->getNumberOfCells()) == 0 || static_cast<int>(_mesh->getNumberOfPoints()) == 0)
    {
        return;
    }

    // Declares data arrays which are associated with QBuffers.
    QByteArray posBufferData;
    posBufferData.resize(static_cast<int>(m_numberOfPoints * m_vertexSize * sizeof(float)));
    float* rawPosBufferData = reinterpret_cast<float*>( posBufferData.data() );

    QByteArray normalBufferData;
    normalBufferData.resize(static_cast<int>(m_numberOfPoints * m_vertexSize * sizeof(float)));
    float* rawNormalBufferData = reinterpret_cast<float*>( normalBufferData.data() );

    QByteArray indexBufferData;
    indexBufferData.resize(3 * static_cast<int>(_mesh->getNumberOfCells()) * static_cast<int>(sizeof(unsigned int)));
    unsigned int* rawIndexBufferData = reinterpret_cast<unsigned int*>( indexBufferData.data() );

    // Checks if the mesh has normals. If not, generates them.
    if(!_mesh->hasPointNormals())
    {
        geometry::data::Mesh::generatePointNormals(_mesh);
    }

    float minX = 0, maxX = 0, minY = 0, maxY = 0, minZ = 0, maxZ = 0;
    bool minMaxSet = false;

    // Iterates over points and registers each point in position and normal buffers.
    auto itrPt          = _mesh->begin< sight::data::iterator::ConstPointIterator >();
    const auto endItrPt = _mesh->end< sight::data::iterator::ConstPointIterator >();

    unsigned int countPts = 0;
    for(; itrPt != endItrPt; ++itrPt)
    {
        rawPosBufferData[countPts]   = itrPt->point->x;
        rawPosBufferData[countPts+1] = itrPt->point->y;
        rawPosBufferData[countPts+2] = itrPt->point->z;

        rawNormalBufferData[countPts]   = itrPt->normal->nx;
        rawNormalBufferData[countPts+1] = itrPt->normal->ny;
        rawNormalBufferData[countPts+2] = itrPt->normal->nz;

        countPts += 3;

        // Computes mesh extents and center. Needed to center camera.
        if(!minMaxSet)
        {
            minX      = itrPt->point->x;
            maxX      = itrPt->point->x;
            minY      = itrPt->point->y;
            maxY      = itrPt->point->y;
            minZ      = itrPt->point->z;
            maxZ      = itrPt->point->z;
            minMaxSet = true;
        }
        else
        {
            if(itrPt->point->x < minX)
            {
                minX = itrPt->point->x;
            }
            if(itrPt->point->x > maxX)
            {
                maxX = itrPt->point->x;
            }
            if(itrPt->point->y < minY)
            {
                minY = itrPt->point->y;
            }
            if(itrPt->point->y > maxY)
            {
                maxY = itrPt->point->y;
            }
            if(itrPt->point->z < minZ)
            {
                maxZ = itrPt->point->z;
            }
            if(itrPt->point->z > maxZ)
            {
                maxZ = itrPt->point->z;
            }
        }
    }

    m_minExtent = QVector3D(minX, minY, minZ);
    m_maxExtent = QVector3D(maxX, maxY, maxZ);

    m_meshCenter =
        QVector3D((m_minExtent.x()+m_maxExtent.x())/2, (m_minExtent.y()+m_maxExtent.y())/2,
                  (m_minExtent.z()+m_maxExtent.z())/2);

    // Iterates over cells and registers points index in index buffer.
    auto itrCell      = _mesh->begin< sight::data::iterator::ConstCellIterator >();
    const auto endItr = _mesh->end< sight::data::iterator::ConstCellIterator >();

    bool isTriangleMesh = true;

    // TODO: The loop before is probably not optimal and would need some extra optimization work
    // Also, mesh composed of triangles and quads would probably not be handled properly
    // See comments in https://git.ircad.fr/sight/sight/-/merge_requests/476
    unsigned int countIndex = 0;
    for(; itrCell != endItr; ++itrCell)
    {
        if(countIndex == 0 && itrCell->nbPoints == 4)
        {
            isTriangleMesh = false;

            // Resizes index buffer if quad mesh to have correct number of indices once quad mesh is converted to
            // triangle mesh.
            indexBufferData.resize(6 * static_cast<int>(_mesh->getNumberOfCells()) *
                                   static_cast<int>(sizeof(unsigned int)));
            rawIndexBufferData = reinterpret_cast<unsigned int*>( indexBufferData.data() );

            m_indexAttrib->setCount(6 * static_cast<unsigned int>(_mesh->getNumberOfCells()));

            this->addComputeEntityToScene(static_cast<int>(_mesh->getNumberOfCells()));
        }

        SIGHT_WARN_IF("A mesh can contain only one type of primitive (quad or triangle).",
                      (!isTriangleMesh && itrCell->nbPoints != 4) || (isTriangleMesh && itrCell->nbPoints != 3))

        for(unsigned int i = 0; i < itrCell->nbPoints; ++i)
        {

            auto pIdx = static_cast<unsigned int>(itrCell->pointIdx[i]);

            // If triangle mesh, only need to copy each point
            if(isTriangleMesh || (i != itrCell->nbPoints-1))
            {
                rawIndexBufferData[countIndex] = pIdx;

                countIndex++;
            }
            // If quad mesh, a quad face is rearranged in 2 triangle face
            // 6 points are defined, 4 with quad coordinates, and the last 2 points will be completed by compute shader
            else
            {
                rawIndexBufferData[countIndex]   = 0;
                rawIndexBufferData[countIndex+1] = pIdx;
                rawIndexBufferData[countIndex+2] = 0;

                countIndex += 3;
            }
        }
    }

    // Associates QBuffers with respective data arrays.
    m_posBuffer->setData(posBufferData);
    m_normalBuffer->setData(normalBufferData);
    m_indexBuffer->setData(indexBufferData);

    return;
}

//------------------------------------------------------------------------------

void Mesh::addComputeEntityToScene(int _numberOfCells)
{
    auto const computeEntity  = new Qt3DCore::QEntity(m_scene);
    auto const computeCommand = new Qt3DRender::QComputeCommand(computeEntity);

    auto const computeMat = new viz::qt3d::ComputeMaterial();
    computeMat->setIndexBuffer(m_indexBuffer);

    computeEntity->addComponent(computeMat);
    computeEntity->addComponent(computeCommand);

    computeMat->updateFrameGraph(m_scene->getFrameGraph(), _numberOfCells);
}

} // namespace data.

} // namespace sight::viz::qt3d.
