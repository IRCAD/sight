/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "viz/qt3d/data/mesh.hpp"

#include "viz/qt3d/ComputeMaterial.hpp"
#include "viz/qt3d/core/GenericScene.hpp"

#include <geometry/data/mesh.hpp>

#include <QComputeCommand>

#include <Qt3DCore/QEntity>

#include <Qt3DExtras/QPhongMaterial>

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>

namespace sight::viz::qt3d::data
{

mesh::mesh(Qt3DCore::QNode* _parent) :
    QEntity(_parent),
    m_scene(qobject_cast<sight::viz::qt3d::core::GenericScene*>(_parent)),
    m_geomRenderer(new Qt3DRender::QGeometryRenderer(m_scene)),
    m_geometry(new Qt3DRender::QGeometry(m_geomRenderer)),
    m_material(new viz::qt3d::data::material()),
    m_posAttrib(new Qt3DRender::QAttribute(m_geometry)),
    m_normalAttrib(new Qt3DRender::QAttribute(m_geometry)),
    m_indexAttrib(new Qt3DRender::QAttribute(m_geometry)),
    m_posBuffer(new Qt3DRender::QBuffer(m_geometry)),
    m_normalBuffer(new Qt3DRender::QBuffer(m_geometry)),
    m_indexBuffer(new Qt3DRender::QBuffer(m_geometry))
{
    this->addComponent(m_geomRenderer);
    this->addComponent(m_material);
}

//------------------------------------------------------------------------------

mesh::~mesh()
= default;

//------------------------------------------------------------------------------

viz::qt3d::data::material* mesh::getMaterial() const
{
    return m_material;
}

//------------------------------------------------------------------------------

sight::viz::qt3d::core::GenericScene* mesh::getScene() const
{
    return m_scene;
}

//------------------------------------------------------------------------------

void mesh::setMaterial(viz::qt3d::data::material* _material)
{
    SIGHT_ASSERT("Material can't be set to null value.", _material);
    this->removeComponent(m_material);
    m_material = _material;
    this->addComponent(m_material);
}

//------------------------------------------------------------------------------

void mesh::setScene(sight::viz::qt3d::core::GenericScene* _scene)
{
    SIGHT_ASSERT("Scene can't be set to null value.", _scene);
    m_scene = _scene;
}

//------------------------------------------------------------------------------

void mesh::setMesh(sight::data::mesh::sptr _mesh)
{
    SIGHT_ASSERT("sight::data::mesh pointer does not exist.", _mesh);

    // Sets the number of points (for a mesh of triangles).
    m_numberOfPoints = static_cast<unsigned int>(_mesh->numPoints());

    if(!m_geometry->attributes().empty())
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
    m_posAttrib->setByteStride(3 * sizeof(float));
    m_posAttrib->setCount(m_numberOfPoints);
    m_posAttrib->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    m_geometry->addAttribute(m_posAttrib);

    m_normalAttrib->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    m_normalAttrib->setBuffer(m_normalBuffer);
    m_normalAttrib->setVertexBaseType(Qt3DRender::QAttribute::Float);
    m_normalAttrib->setVertexSize(m_vertexSize);
    m_normalAttrib->setByteOffset(0);
    m_normalAttrib->setByteStride(3 * sizeof(float));
    m_normalAttrib->setCount(m_numberOfPoints);
    m_normalAttrib->setName(Qt3DRender::QAttribute::defaultNormalAttributeName());

    m_geometry->addAttribute(m_normalAttrib);

    m_indexAttrib->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    m_indexAttrib->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    m_indexAttrib->setBuffer(m_indexBuffer);
    m_indexAttrib->setCount(3 * static_cast<unsigned int>(_mesh->numCells()));
    m_indexAttrib->setName("indexAttribute");

    m_geometry->addAttribute(m_indexAttrib);

    // Fills buffers with data extracted from _mesh.
    this->buildBuffers(_mesh);

    m_geometry->setBoundingVolumePositionAttribute(m_posAttrib);

    m_geomRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
    m_geomRenderer->setGeometry(m_geometry);
}

//------------------------------------------------------------------------------

void mesh::centerCameraOnMesh()
{
    auto* camera = m_scene->getCamera();
    camera->viewSphere(
        m_meshCenter,
        std::max(
            std::max(m_maxExtent.x() - m_minExtent.x(), m_maxExtent.y() - m_minExtent.y()),
            m_maxExtent.z() - m_minExtent.z()
        )
    );
}

//------------------------------------------------------------------------------

void mesh::buildBuffers(sight::data::mesh::sptr _mesh)
{
    SIGHT_ASSERT("sight::data::mesh pointer does not exist.", _mesh);

    // Returns if _mesh is empty.
    if(static_cast<int>(_mesh->numCells()) == 0 || static_cast<int>(_mesh->numPoints()) == 0)
    {
        return;
    }

    // Declares data arrays which are associated with QBuffers.
    QByteArray posBufferData;
    posBufferData.resize(static_cast<int>(static_cast<std::size_t>(m_numberOfPoints) * m_vertexSize * sizeof(float)));
    auto* rawPosBufferData = reinterpret_cast<float*>(posBufferData.data());

    QByteArray normalBufferData;
    normalBufferData.resize(static_cast<int>(std::size_t(m_numberOfPoints) * m_vertexSize * sizeof(float)));
    auto* rawNormalBufferData = reinterpret_cast<float*>(normalBufferData.data());

    QByteArray indexBufferData;
    indexBufferData.resize(3 * static_cast<int>(_mesh->numCells()) * static_cast<int>(sizeof(unsigned int)));
    auto* rawIndexBufferData = reinterpret_cast<unsigned int*>(indexBufferData.data());

    // Checks if the mesh has normals. If not, generates them.
    if(!_mesh->has<sight::data::mesh::Attributes::POINT_NORMALS>())
    {
        geometry::data::mesh::generatePointNormals(_mesh);
    }

    float minX     = 0;
    float maxX     = 0;
    float minY     = 0;
    float maxY     = 0;
    float minZ     = 0;
    float maxZ     = 0;
    bool minMaxSet = false;

    // Iterates over points and registers each point in position and normal buffers.
    namespace point = sight::data::iterator::point;
    namespace cell  = sight::data::iterator::cell;

    unsigned int countPts = 0;
    for(const auto& [p, n] : _mesh->czip_range<point::xyz, point::nxyz>())
    {
        rawPosBufferData[countPts]     = p.x;
        rawPosBufferData[countPts + 1] = p.y;
        rawPosBufferData[countPts + 2] = p.z;

        rawNormalBufferData[countPts]     = n.nx;
        rawNormalBufferData[countPts + 1] = n.ny;
        rawNormalBufferData[countPts + 2] = n.nz;

        countPts += 3;

        // Computes mesh extents and center. Needed to center camera.
        if(!minMaxSet)
        {
            minX      = p.x;
            maxX      = p.x;
            minY      = p.y;
            maxY      = p.y;
            minZ      = p.z;
            maxZ      = p.z;
            minMaxSet = true;
        }
        else
        {
            if(p.x < minX)
            {
                minX = p.x;
            }

            if(p.x > maxX)
            {
                maxX = p.x;
            }

            if(p.y < minY)
            {
                minY = p.y;
            }

            if(p.y > maxY)
            {
                maxY = p.y;
            }

            if(p.z < minZ)
            {
                maxZ = p.z;
            }

            if(p.z > maxZ)
            {
                maxZ = p.z;
            }
        }
    }

    m_minExtent = QVector3D(minX, minY, minZ);
    m_maxExtent = QVector3D(maxX, maxY, maxZ);

    m_meshCenter =
        QVector3D(
            (m_minExtent.x() + m_maxExtent.x()) / 2,
            (m_minExtent.y() + m_maxExtent.y()) / 2,
            (m_minExtent.z() + m_maxExtent.z()) / 2
        );

    // Iterates over cells and registers points index in index buffer.
    const auto cellType = _mesh->getCellType();

    // TODO: The loop before is probably not optimal and would need some extra optimization work
    // See comments in https://git.ircad.fr/sight/sight/-/merge_requests/476
    unsigned int countIndex = 0;

    if(cellType == sight::data::mesh::CellType::TRIANGLE)
    {
        for(const auto& cell : _mesh->crange<cell::triangle>())
        {
            rawIndexBufferData[countIndex++] = cell.pt[0];
            rawIndexBufferData[countIndex++] = cell.pt[1];
            rawIndexBufferData[countIndex++] = cell.pt[2];
        }
    }
    else if(cellType == sight::data::mesh::CellType::QUAD)
    {
        // Resizes index buffer if quad mesh to have correct number of indices once quad mesh is converted to
        // triangle mesh.
        indexBufferData.resize(
            6 * static_cast<int>(_mesh->numCells())
            * static_cast<int>(sizeof(unsigned int))
        );
        rawIndexBufferData = reinterpret_cast<unsigned int*>(indexBufferData.data());

        m_indexAttrib->setCount(6 * static_cast<unsigned int>(_mesh->numCells()));

        this->addComputeEntityToScene(static_cast<int>(_mesh->numCells()));
        for(const auto& cell : _mesh->crange<cell::quad>())
        {
            rawIndexBufferData[countIndex++] = cell.pt[0];
            rawIndexBufferData[countIndex++] = cell.pt[1];
            rawIndexBufferData[countIndex++] = cell.pt[2];
            rawIndexBufferData[countIndex++] = cell.pt[3];

            rawIndexBufferData[countIndex++]     = 0;
            rawIndexBufferData[countIndex++ + 1] = cell.pt[0];
            rawIndexBufferData[countIndex++ + 2] = 0;
        }
    }

    // Associates QBuffers with respective data arrays.
    m_posBuffer->setData(posBufferData);
    m_normalBuffer->setData(normalBufferData);
    m_indexBuffer->setData(indexBufferData);
}

//------------------------------------------------------------------------------

void mesh::addComputeEntityToScene(int _numberOfCells)
{
    auto* const computeEntity  = new Qt3DCore::QEntity(m_scene);
    auto* const computeCommand = new Qt3DRender::QComputeCommand(computeEntity);

    auto* const computeMat = new viz::qt3d::ComputeMaterial();
    computeMat->setIndexBuffer(m_indexBuffer);

    computeEntity->addComponent(computeMat);
    computeEntity->addComponent(computeCommand);

    computeMat->updateFrameGraph(m_scene->getFrameGraph(), _numberOfCells);
}

} // namespace sight::viz::qt3d::data
