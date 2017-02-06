/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/interactor/Negato2DInteractor.hpp"

#include <fwRenderOgre/Layer.hpp>
#include <fwRenderOgre/registry/macros.hpp>

#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreCamera.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreMesh.h>
#include <OGRE/OgreRenderSystem.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreVector3.h>
#include <OGRE/OgreViewport.h>

#include <stack>

fwRenderOgreRegisterInteractorMacro( ::fwRenderOgre::interactor::Negato2DInteractor );

namespace fwRenderOgre
{
namespace interactor
{

const float Negato2DInteractor::SCALE_COEF = 0.9f;

// ----------------------------------------------------------------------------

Negato2DInteractor::Negato2DInteractor() :
    m_zoomCoefficient(0),
    m_zoomScale(1),
    m_totalWidth(1),
    m_totalHeight(1),
    m_currentWidth(10),
    m_currentHeight(10),
    m_camera(nullptr)
{
    m_minimumCorner = ::Ogre::Vector3(-1., -1., 0.);
    m_maximumCorner = ::Ogre::Vector3(1., 1., 0.);
}

// ----------------------------------------------------------------------------

void Negato2DInteractor::wheelEvent(int delta, int mouseX, int mouseY)
{
    this->updateRenderWindowDimensions();
    this->updateTotalSize();

    if(m_totalWidth > 1)
    {
        ::Ogre::Real xTexture = this->getTextureCoordinate_X( mouseX );
        ::Ogre::Real yTexture = this->getTextureCoordinate_Y( mouseY );

        // We've got a crash if we go above 21 and we have no image loaded due to float precision.
        if (delta > 0 && m_zoomCoefficient < 21)
        {
            ++m_zoomCoefficient;
            m_zoomScale *= Negato2DInteractor::SCALE_COEF;
        }
        else if (delta < 0 && m_zoomCoefficient > 0)
        {
            --m_zoomCoefficient;
            m_zoomScale /= Negato2DInteractor::SCALE_COEF;
        }

        // Update Camera Scale
        m_currentWidth  = m_zoomScale * m_totalWidth;
        m_currentHeight = m_zoomScale * m_totalHeight;

        ::Ogre::Real leftScreenBordure = xTexture - static_cast< ::Ogre::Real >(mouseX) / m_renderWindowWidth *
                                         m_currentWidth;
        ::Ogre::Real topScreenBordure = yTexture + static_cast< ::Ogre::Real >(mouseY) / m_renderWindowHeight *
                                        m_currentHeight;

        m_cameraPos.x = leftScreenBordure + m_currentWidth/2;
        m_cameraPos.y = topScreenBordure - m_currentHeight/2;

        this->updateCameraAngle();
        this->updateCameraPosition();
    }
}

// ----------------------------------------------------------------------------

void Negato2DInteractor::mouseMoveEvent(IInteractor::MouseButton button, int x, int y, int dx, int dy)
{
    if(button == MIDDLE && m_totalWidth > 1)
    {
        m_cameraPos.x += m_currentWidth / m_renderWindowWidth * static_cast< ::Ogre::Real >(dx);
        m_cameraPos.y -= m_currentHeight / m_renderWindowHeight * static_cast< ::Ogre::Real >(dy);
        this->updateCameraPosition();
    }
}

// ----------------------------------------------------------------------------

void Negato2DInteractor::updateTotalSize()
{
    ::Ogre::SceneNode* rootSceneNode = m_sceneManager->getRootSceneNode();
    rootSceneNode->getChildIterator();

    ::std::stack< const ::Ogre::SceneNode* > childrenStack;
    childrenStack.push(rootSceneNode);

    // Since we're working on a SNegato2D we assume there's only one entity in the scene node.
    while(!childrenStack.empty())
    {
        const ::Ogre::SceneNode* tempSceneNode = childrenStack.top();
        childrenStack.pop();

        // Retrieves an iterator pointing to the attached movable objects of the current scene node
        ::Ogre::SceneNode::ConstObjectIterator entitiesIt = tempSceneNode->getAttachedObjectIterator();
        while(entitiesIt.hasMoreElements())
        {
            // First, we must cast the MovableObject* into an Entity*
            const ::Ogre::Entity* e = dynamic_cast< ::Ogre::Entity* > (entitiesIt.getNext());

            if(e)
            {
                ::Ogre::MeshPtr negatoMesh             = e->getMesh();
                ::Ogre::AxisAlignedBox meshBoundingBox = negatoMesh->getBounds();
                m_minimumCorner                        = meshBoundingBox.getMinimum();
                m_maximumCorner                        = meshBoundingBox.getMaximum();
                m_totalWidth                           = meshBoundingBox.getSize().x;
                m_totalHeight                          = meshBoundingBox.getSize().y;
                break;
            }
        }

        // Retrieves an iterator pointing to the child nodes of the current scene node
        ::Ogre::SceneNode::ConstChildNodeIterator childNodesIt = tempSceneNode->getChildIterator();
        while(childNodesIt.hasMoreElements())
        {
            // First, we must cast the Node* into a SceneNode*
            const ::Ogre::SceneNode* childNode = dynamic_cast< ::Ogre::SceneNode* >(childNodesIt.getNext());
            if(childNode)
            {
                // Push the current node into the stack in order to continue iteration
                childrenStack.push(childNode);
            }
        }
    }
}

// ----------------------------------------------------------------------------

void Negato2DInteractor::updateRenderWindowDimensions()
{
    // Retrieve the render window bounds
    ::Ogre::RenderSystem* renderSystem = m_sceneManager->getDestinationRenderSystem();

    m_renderWindowWidth  = static_cast< ::Ogre::Real >(renderSystem->_getViewport()->getActualWidth());
    m_renderWindowHeight = static_cast< ::Ogre::Real >(renderSystem->_getViewport()->getActualHeight());
    m_renderWindowRatio  = m_renderWindowWidth / m_renderWindowHeight;
    this->getCamera()->setAspectRatio(m_renderWindowRatio);

}

// ----------------------------------------------------------------------------

void Negato2DInteractor::updateCameraPosition()
{
    // Checks if x position is within allowed bounds
    m_cameraPos.x = (m_cameraPos.x - m_currentWidth/2 < m_minimumCorner.x )
                    ? m_minimumCorner.x + m_currentWidth/2 : m_cameraPos.x;
    m_cameraPos.x = (m_cameraPos.x + m_currentWidth/2 > m_maximumCorner.x )
                    ? m_maximumCorner.x - m_currentWidth/2 : m_cameraPos.x;

    // Checks if y position is within allowed bounds
    m_cameraPos.y = (m_cameraPos.y - m_currentHeight/2 < m_minimumCorner.y )
                    ? m_minimumCorner.y + m_currentHeight/2 : m_cameraPos.y;
    m_cameraPos.y = (m_cameraPos.y + m_currentHeight/2 > m_maximumCorner.y )
                    ? m_maximumCorner.y - m_currentHeight/2 : m_cameraPos.y;

    this->getCamera()->setPosition( m_cameraPos );
}

// ----------------------------------------------------------------------------

void Negato2DInteractor::resetCameraPosition()
{
    ::Ogre::Real cameraWidth, cameraHeight;

    if( m_renderWindowWidth < m_renderWindowHeight)
    {
        cameraWidth  = m_totalWidth;
        cameraHeight = cameraWidth / m_renderWindowRatio;
    }
    else
    {
        cameraHeight = m_totalHeight;
        cameraWidth  = cameraHeight * m_renderWindowRatio;
    }

    this->getCamera()->setOrthoWindow(cameraWidth, cameraHeight);
}

// ----------------------------------------------------------------------------

::Ogre::Camera* Negato2DInteractor::getCamera()
{
    if(!m_camera)
    {
        m_camera    = m_sceneManager->getCamera(::fwRenderOgre::Layer::DEFAULT_CAMERA_NAME);
        m_cameraPos = m_camera->getPosition();
        m_camera->setOrthoWindow(1.f, 1.f);
    }
    return m_camera;
}

// ----------------------------------------------------------------------------

void Negato2DInteractor::updateCameraAngle()
{
    this->updateRenderWindowDimensions();
    ::Ogre::Real orthoWidth, orthoHeight, imageToWindowRatio;

    if( m_renderWindowWidth == m_renderWindowHeight )
    {
        this->getCamera()->setOrthoWindow( m_currentWidth, m_currentHeight );
    }
    else if( m_renderWindowWidth > m_renderWindowHeight )
    {
        orthoHeight        = m_currentHeight;
        imageToWindowRatio = m_currentHeight / m_renderWindowHeight;
        orthoWidth         = imageToWindowRatio * orthoHeight;
        this->getCamera()->setOrthoWindowHeight( orthoHeight);

    }
    else if( m_renderWindowWidth < m_renderWindowHeight )
    {
        orthoWidth         = m_currentWidth;
        imageToWindowRatio = m_currentWidth / m_renderWindowWidth;
        orthoHeight        = imageToWindowRatio * orthoWidth;
        this->getCamera()->setOrthoWindowWidth( orthoWidth );
    }
}

// ----------------------------------------------------------------------------

::Ogre::Real Negato2DInteractor::getTextureCoordinate_X(int mouseX)
{
    ::Ogre::Real leftScreenBordure = m_cameraPos.x - static_cast< ::Ogre::Real >(m_currentWidth) / 2;
    ::Ogre::Real newCoordinate     = static_cast< ::Ogre::Real >(leftScreenBordure) +
                                     static_cast< ::Ogre::Real >(mouseX) / m_renderWindowWidth * m_currentWidth;
    return newCoordinate;
}

// ----------------------------------------------------------------------------

::Ogre::Real Negato2DInteractor::getTextureCoordinate_Y(int mouseY)
{
    // Qt and Ogre's Y axis are oriented to the opposite of each other.
    ::Ogre::Real topScreenBordure = m_cameraPos.y + static_cast< ::Ogre::Real >(m_currentHeight) / 2;
    ::Ogre::Real newCoordinate    = topScreenBordure - (static_cast< ::Ogre::Real >(mouseY) /
                                                        m_renderWindowHeight * m_currentHeight);
    return newCoordinate;
}

// ----------------------------------------------------------------------------

} // namespace interactor
} // namespace fwRenderOgre
