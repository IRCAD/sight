/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/vr/SliceVolumeRenderer.hpp"

#include "fwRenderOgre/helper/Shading.hpp"

#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTexture.h>
#include <OGRE/OgreTextureUnitState.h>

namespace fwRenderOgre
{

namespace vr
{

//-----------------------------------------------------------------------------

SliceVolumeRenderer::SliceVolumeRenderer(std::string parentId,
                                         ::Ogre::SceneManager* sceneManager,
                                         ::Ogre::SceneNode* parentNode,
                                         ::Ogre::TexturePtr imageTexture,
                                         TransferFunction& gpuTF,
                                         PreIntegrationTable& preintegrationTable) :
    IVolumeRenderer        (parentId, sceneManager, parentNode, imageTexture, gpuTF, preintegrationTable),
    m_intersectingPolygons (nullptr)
{
    m_sceneRenderQueue = m_sceneManager->getRenderQueue();

    // Set materials.
    const std::vector<std::string> vrMaterials {{ "PreIntegratedSliceVolume", "SliceVolume" }};

    for(const std::string& mtlName : vrMaterials)
    {
        ::Ogre::MaterialPtr volumeMtl              = ::Ogre::MaterialManager::getSingletonPtr()->getByName(mtlName);
        ::Ogre::Material::TechniqueIterator techIt = volumeMtl->getTechniqueIterator();

        while( techIt.hasMoreElements())
        {
            ::Ogre::Technique* tech = techIt.getNext();
            SLM_ASSERT("Technique is not set", tech);

            if(::fwRenderOgre::helper::Shading::isColorTechnique(*tech))
            {
                ::Ogre::Pass* pass = tech->getPass(0);

                ::Ogre::TextureUnitState* tex3DState = pass->getTextureUnitState("image");
                ::Ogre::TextureUnitState* texTFState = pass->getTextureUnitState("transferFunction");

                SLM_ASSERT("'image' texture unit is not found", tex3DState);
                SLM_ASSERT("'transferFunction' texture unit is not found", texTFState);

                tex3DState->setTexture(m_3DOgreTexture);

                if(mtlName == "PreIntegratedSliceVolume")
                {
                    texTFState->setTexture(m_preIntegrationTable.getTexture());
                    m_preIntegrationShaderParameters = pass->getFragmentProgramParameters();
                }
                else
                {
                    texTFState->setTexture(m_gpuTF.getTexture());
                    m_defaultShaderParameters = pass->getFragmentProgramParameters();
                    m_currentShaderParameters = m_defaultShaderParameters;
                }
            }
        }
    }

    m_renderListener = new RenderListener(this);

    initSlices();
    updateAllSlices();
}

//-----------------------------------------------------------------------------

SliceVolumeRenderer::~SliceVolumeRenderer()
{
    m_sceneManager->destroyManualObject(m_intersectingPolygons);
    delete m_renderListener;
}

//-----------------------------------------------------------------------------

void SliceVolumeRenderer::imageUpdate(fwData::Image::sptr image, fwData::TransferFunction::sptr tf)
{
    scaleCube(image->getSpacing());

    if(m_preIntegratedRendering)
    {
        m_preIntegrationTable.imageUpdate(image, tf, m_sampleDistance);

        auto minMax = m_preIntegrationTable.getMinMax();

        m_preIntegrationShaderParameters->setNamedConstant("u_min", minMax.first);
        m_preIntegrationShaderParameters->setNamedConstant("u_max", minMax.second);
    }
}

//-----------------------------------------------------------------------------

void SliceVolumeRenderer::setSampling(uint16_t nbSamples)
{
    m_nbSlices = nbSamples;

    initSlices();
    updateAllSlices();
}

//-----------------------------------------------------------------------------

void SliceVolumeRenderer::setPreIntegratedRendering(bool preIntegratedRendering)
{
    m_preIntegratedRendering = preIntegratedRendering;

    m_currentShaderParameters = m_preIntegratedRendering ? m_preIntegrationShaderParameters : m_defaultShaderParameters;

    initSlices();
    updateAllSlices();
}

//-----------------------------------------------------------------------------

void SliceVolumeRenderer::initSlices()
{
    if(m_intersectingPolygons)
    {
        m_sceneManager->destroyManualObject(m_intersectingPolygons);
    }

    m_intersectingPolygons = m_sceneManager->createManualObject(m_parentId + "_VolumeRenderSlices");
    m_intersectingPolygons->setListener(m_renderListener);

    // create m_nbSlices slices
    m_intersectingPolygons->estimateVertexCount(6 /* * m_nbSlices*/ * 2);
    m_intersectingPolygons->setDynamic(true);

    std::string material = m_preIntegratedRendering ? "PreIntegratedSliceVolume" : "SliceVolume";

    for(uint16_t sliceNumber = 0; sliceNumber < m_nbSlices; ++sliceNumber)
    {
        m_intersectingPolygons->begin(material, ::Ogre::RenderOperation::OT_TRIANGLE_STRIP);
        {
            // initialize slice with a 4 triangle strip
            for(unsigned i = 0; i < 6; ++i)
            {
                m_intersectingPolygons->position(0, 0, 0);
                m_intersectingPolygons->textureCoord(0, 0, 0);
            }
        }
        m_intersectingPolygons->end();
    }

    m_volumeSceneNode->attachObject(m_intersectingPolygons);
}

//-----------------------------------------------------------------------------

void SliceVolumeRenderer::updateAllSlices()
{
    ::Ogre::Plane cameraPlane = getCameraPlane();

    const unsigned closestVtxIndex   = computeSampleDistance(cameraPlane);
    const ::Ogre::Vector3 closestVtx = m_clippedImagePositions[closestVtxIndex];

    const ::Ogre::Vector3 planeNormal = cameraPlane.normal;

    m_currentShaderParameters->setNamedConstant("u_sampleDistance", m_sampleDistance);

    // set first plane
    ::Ogre::Vector3 planeVertex = closestVtx + planeNormal * m_sampleDistance;

    // compute all slices
    for(int sliceNumber = m_nbSlices - 1; sliceNumber > 0; --sliceNumber)
    {
        Polygon intersections = cubePlaneIntersection(planeNormal, planeVertex, (unsigned)closestVtxIndex);

        if(intersections.size() >= 3)
        {
            updateSlice(intersections, sliceNumber);
        }

        // set next plane
        planeVertex += planeNormal * m_sampleDistance;
    }
}

//-----------------------------------------------------------------------------

void SliceVolumeRenderer::updateSlice(const Polygon& _polygon,const unsigned _sliceIndex)
{
    const size_t nbVertices = _polygon.size();

    // triangulate polygon into a triangle strip
    m_intersectingPolygons->beginUpdate(_sliceIndex);
    {
        m_intersectingPolygons->position( _polygon[1] );
        m_intersectingPolygons->textureCoord( _polygon[1] );
        m_intersectingPolygons->position( _polygon[0] );
        m_intersectingPolygons->textureCoord( _polygon[0] );
        m_intersectingPolygons->position( _polygon[2] );
        m_intersectingPolygons->textureCoord( _polygon[2] );

        for(unsigned i = 0; i < nbVertices - 3; ++i)
        {
            m_intersectingPolygons->position( _polygon[nbVertices - 1 - i] );
            m_intersectingPolygons->textureCoord( _polygon[nbVertices - 1 - i] );

            if(i + 3 < nbVertices - 1)
            {
                m_intersectingPolygons->position( _polygon[i + 3] );
                m_intersectingPolygons->textureCoord( _polygon[i + 3] );
            }
        }
    }
    m_intersectingPolygons->end();
}

//-----------------------------------------------------------------------------

bool SliceVolumeRenderer::planeEdgeIntersection(const ::Ogre::Vector3& _planeNormal,
                                                const ::Ogre::Vector3& _planeVertex,
                                                const unsigned _edgeVertexIndex0,
                                                const unsigned _edgeVertexIndex1,
                                                Polygon& _result
                                                ) const
{
    bool planeIntersectsEdge = false;

    const ::Ogre::Vector3 edgePoint0 = m_clippedImagePositions[_edgeVertexIndex0];
    const ::Ogre::Vector3 edgePoint1 = m_clippedImagePositions[_edgeVertexIndex1];

    if(_planeNormal.dotProduct(edgePoint1 - edgePoint0) != 0) // plane and edge are not parallel
    {
        // intersectPoint represents the intersection point r on the parametric line described by this equation:
        // S(r) = _egdePoint0 + r * (_edgePoint1 - _edgePoint0)
        ::Ogre::Real intersectPoint = _planeNormal.dotProduct(_planeVertex - edgePoint0)
                                      / _planeNormal.dotProduct(edgePoint1 - edgePoint0);

        planeIntersectsEdge = (intersectPoint >= 0) && (intersectPoint <= 1);

        if(planeIntersectsEdge)
        {
            const ::Ogre::Vector3 position = edgePoint0 + intersectPoint * (edgePoint1 - edgePoint0);

            _result.push_back(position);
        }
    }
    //TODO: else check if _planeVertex == _edgePoint0 or _edgePoint1

    return planeIntersectsEdge;
}

//-----------------------------------------------------------------------------

SliceVolumeRenderer::Polygon SliceVolumeRenderer::cubePlaneIntersection(const ::Ogre::Vector3& _planeNormal,
                                                                        const ::Ogre::Vector3& _planeVertex,
                                                                        const unsigned _closestVertexIndex) const
{
    Polygon intersections;
    intersections.reserve(6); // there is a maximum of 6 intersections

    // sequence[i] represents the order in which we must traverse
    // the cube's edges starting with the ith vertex
    const unsigned sequences[8][8] = {
        { 0, 1, 2, 3, 4, 5, 6, 7 },
        { 1, 4, 5, 0, 3, 7, 2, 6 },
        { 2, 6, 0, 5, 7, 3, 1, 4 },
        { 3, 0, 6, 4, 1, 2, 7, 5 },
        { 4, 3, 7, 1,0, 6, 5, 2 },
        { 5, 2, 1, 7, 6, 0, 4, 3 },
        { 6, 7, 3, 2, 5, 4, 0, 1 },
        { 7, 5, 4, 6, 2, 1, 3, 0 }
    };

//    const unsigned paths[3][5] = { { 0, 1 ,4 ,7, 5 } ,
//                                   { 0, 2, 5, 7, 6 } ,
//                                   { 0, 3, 6, 7, 4 } };
    const unsigned lastEdge[3] = { 5, 6, 4 };

    for(int pathNum = 0; pathNum < 3; ++pathNum)
    {
        bool hasIntersection = false;
        unsigned v1, v2;

        const unsigned path[] = {
            sequences[_closestVertexIndex][0],
            sequences[_closestVertexIndex][pathNum + 1],
            sequences[_closestVertexIndex][pathNum + 4],
            sequences[_closestVertexIndex][7],
            sequences[_closestVertexIndex][ lastEdge[ pathNum ] ]
        };

        for(int i = 0; i < 3 && !hasIntersection; ++i)
        {
            v1 = path[i    ];
            v2 = path[i + 1];

            hasIntersection = planeEdgeIntersection(_planeNormal, _planeVertex, v1, v2, intersections);
        }

        v1 = path[1];
        v2 = path[4];

        if( hasIntersection )
        {
            planeEdgeIntersection(_planeNormal, _planeVertex, v1, v2, intersections);
        }
    }

    return intersections;
}

//-----------------------------------------------------------------------------

} // namespace vr

} // namespace fwRenderOgre
