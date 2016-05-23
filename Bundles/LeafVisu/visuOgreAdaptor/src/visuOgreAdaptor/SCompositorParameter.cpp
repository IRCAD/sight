/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SCompositorParameter.hpp"

#include "visuOgreAdaptor/defines.hpp"
#include "visuOgreAdaptor/SMaterial.hpp"

#include <fwComEd/helper/Array.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Array.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/Color.hpp>
#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/String.hpp>
#include <fwData/Vector.hpp>

#include <fwRenderOgre/compositor/ChainManager.hpp>

#include <fwServices/macros.hpp>

#include <algorithm>

#include <OgreCompositorChain.h>
#include <OgreCompositorManager.h>
#include <OgreCompositorInstance.h>
#include <OgreGpuProgramParams.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgreTechnique.h>

namespace visuOgreAdaptor
{

fwServicesRegisterMacro( ::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SCompositorParameter, ::fwData::Object);

//------------------------------------------------------------------------------

class CompositorListener : public ::Ogre::CompositorInstance::Listener
{
public:
    CompositorListener() : m_viewport(nullptr), m_adaptor(nullptr)
    {
    }
    CompositorListener(::Ogre::Viewport* _vp, SCompositorParameter::sptr _adaptor)
        : m_viewport(_vp), m_adaptor(_adaptor)
    {
    }
    ~CompositorListener()
    {
    }

    void notifyMaterialRender(::Ogre::uint32 pass_id, ::Ogre::MaterialPtr& mat)
    {
        m_adaptor->updateValue(mat);
    }

private:
    /// Ogre viewport
    ::Ogre::Viewport* m_viewport;
    /// Associated f4s adaptor
    ::visuOgreAdaptor::SCompositorParameter::sptr m_adaptor;
};

//------------------------------------------------------------------------------

SCompositorParameter::SCompositorParameter() throw() :
    m_shaderType(VERTEX)
{
}

//------------------------------------------------------------------------------

SCompositorParameter::~SCompositorParameter() throw()
{
}

//------------------------------------------------------------------------------

void SCompositorParameter::setShaderType(std::string shaderType)
{
    if (shaderType == "vp")
    {
        m_shaderType = ShaderEnum::VERTEX;
    }
    else if (shaderType == "fp")
    {
        m_shaderType = ShaderEnum::FRAGMENT;
    }
    else if (shaderType == "gp")
    {
        m_shaderType = ShaderEnum::GEOMETRY;
    }
    else
    {
        OSLM_ERROR("Shader type : " << shaderType << " not supported");
    }
}

//------------------------------------------------------------------------------

void SCompositorParameter::setShaderType(ShaderEnumType shaderType)
{
    m_shaderType = shaderType;
}

//------------------------------------------------------------------------------

void SCompositorParameter::setMaterialName(std::string matName)
{
    m_compositorName = matName;
}

//------------------------------------------------------------------------------

void SCompositorParameter::setParamName(std::string paramName)
{
    m_paramName = paramName;
}

//------------------------------------------------------------------------------

void SCompositorParameter::doStart() throw(::fwTools::Failed)
{
    ::fwRenderOgre::Layer::sptr layer =
        this->getRenderService()->getLayer();
    ::fwRenderOgre::compositor::ChainManager::CompositorChainType compositorChain = layer->getCompositorChain();

    std::pair< std::string, bool> compositorPair(m_compositorName,true);

    auto result = std::find(compositorChain.begin(), compositorChain.end(),compositorPair);
    SLM_ASSERT("The given compositor doesn't exist in the compositor chain", result != compositorChain.end());

    ::Ogre::CompositorChain* compChain =
        ::Ogre::CompositorManager::getSingletonPtr()->getCompositorChain(layer->getViewport());

    // Association of a listener attached to this adaptor to the configured compositor
    compChain->getCompositor(m_compositorName)->addListener(new CompositorListener(layer->getViewport(),
                                                                                   SCompositorParameter::dynamicCast(
                                                                                       this->getSptr())));
}

//------------------------------------------------------------------------------

void SCompositorParameter::doStop() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SCompositorParameter::doSwap() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SCompositorParameter::doConfigure() throw(::fwTools::Failed)
{
    SLM_ASSERT("Not a \"config\" configuration", m_configuration->getName() == "config");

    m_compositorName = m_configuration->getAttributeValue("compositorName");
    OSLM_ERROR_IF("material attribute not set", m_compositorName.empty());

    m_paramName = m_configuration->getAttributeValue("parameter");
    OSLM_ERROR_IF("parameter attribute not set", m_paramName.empty());

    m_techniqueName = m_configuration->getAttributeValue("technique");

    if ( m_configuration->hasAttribute("shaderType"))
    {
        std::string shaderType = m_configuration->getAttributeValue("shaderType");
        if (shaderType == "vertex")
        {
            m_shaderType = VERTEX;
        }
        else if (shaderType == "fragment")
        {
            m_shaderType = FRAGMENT;
        }
        else if (shaderType == "geometry")
        {
            m_shaderType = GEOMETRY;
        }
        else
        {
            OSLM_ERROR("This shader type " << shaderType << " isn't supported yet");
        }
    }
}

//------------------------------------------------------------------------------

void SCompositorParameter::doUpdate() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SCompositorParameter::updateValue(Ogre::MaterialPtr& _mat)
{
    if(m_techniqueName.empty())
    {
        bool bSet = false;
        ::Ogre::Material::TechniqueIterator techIt = _mat->getTechniqueIterator();
        while( techIt.hasMoreElements())
        {
            ::Ogre::Technique* tech = techIt.getNext();
            SLM_ASSERT("Technique is not set", tech);

            bSet |= this->setParameter(*tech);
        }

        if( !bSet )
        {
            SLM_ERROR("Couldn't set parameter '" + m_paramName + "' in any technique of compositor '"
                      + m_compositorName + "'");
        }
    }
    else
    {
        ::Ogre::Technique* tech = nullptr;
        tech                    = _mat->getTechnique(m_techniqueName);
        OSLM_FATAL_IF("Can't find technique " << m_techniqueName, !tech);

        if( this->setParameter(*tech) )
        {
            SLM_ERROR("Couldn't set parameter '" + m_paramName + "' in technique '" + m_techniqueName +
                      "' from compositor '" + m_compositorName + "'");
        }
    }
}

//------------------------------------------------------------------------------

bool SCompositorParameter::setParameter(::Ogre::Technique& technique)
{
    /// Contains the different parameters for the shader
    ::Ogre::GpuProgramParametersSharedPtr params;

    // Get the parameters
    auto pass = technique.getPass(0);
    if (m_shaderType == VERTEX)
    {
        params = pass->getVertexProgramParameters();
    }
    else if (m_shaderType == FRAGMENT && pass->hasFragmentProgram())
    {
        params = pass->getFragmentProgramParameters();
    }
    else if (m_shaderType == GEOMETRY)
    {
        params = pass->getGeometryProgramParameters();
    }
    else
    {
        return false;
    }

    if(!params->_findNamedConstantDefinition(m_paramName))
    {
        return false;
    }

    ::fwData::Object::sptr obj = this->getObject();

    // Set shader parameters
    std::string objClass = obj->getClassname();
    if(objClass == "::fwData::Integer")
    {
        ::fwData::Integer::sptr intValue = ::fwData::Integer::dynamicCast(obj);
        SLM_ASSERT("The given integer object is null", intValue);

        params->setNamedConstant(m_paramName, intValue->value());
    }
    else if(objClass == "::fwData::Float")
    {
        ::fwData::Float::sptr floatValue = ::fwData::Float::dynamicCast(obj);
        SLM_ASSERT("The given float object is null", floatValue);

        params->setNamedConstant(m_paramName,  floatValue->value());
    }
    else if(objClass == "::fwData::Boolean")
    {
        ::fwData::Boolean::sptr booleanValue = ::fwData::Boolean::dynamicCast(obj);
        SLM_ASSERT("The given boolean object is null", booleanValue);

        params->setNamedConstant(m_paramName, static_cast< int>(booleanValue->value()));
    }
    else if(objClass == "::fwData::Color")
    {
        ::fwData::Color::sptr colorValue = ::fwData::Color::dynamicCast(obj);
        SLM_ASSERT("The given color object is null", colorValue);

        float paramValues[4];

        paramValues[0] = colorValue->red();
        paramValues[1] = colorValue->green();
        paramValues[2] = colorValue->blue();
        paramValues[3] = colorValue->alpha();

        ::Ogre::ColourValue color(paramValues[0], paramValues[1], paramValues[2], paramValues[3]);

        params->setNamedConstant(m_paramName, color);
    }
    else if(objClass == "::fwData::Point")
    {
        OSLM_ERROR("This Type  " << objClass << " isn't supported yet.");
    }
    else if(objClass == "::fwData::PointList")
    {
        ::fwData::PointList::sptr pointListValue = ::fwData::PointList::dynamicCast(obj);
        SLM_ASSERT("The given pointList object is null", pointListValue);

        std::vector< ::fwData::Point::sptr > points = pointListValue->getPoints();
        int nbPoints                                = static_cast<int>(points.size());

        float* paramValues = new float[nbPoints * 3];

        for(int i = 0; i < nbPoints * 3; i++)
        {
            paramValues[i] = static_cast<float>(points[static_cast<size_t>(i)]->getCoord()[0]);
            i++;

            paramValues[i] = static_cast<float>(points[static_cast<size_t>(i)]->getCoord()[1]);
            i++;

            paramValues[i] = static_cast<float>(points[static_cast<size_t>(i)]->getCoord()[2]);
            i++;
        }

        params->setNamedConstant(m_paramName, paramValues, static_cast<size_t>(nbPoints), static_cast<size_t>(3));

        delete [] paramValues;
    }
    else if(objClass == "::fwData::TransformationMatrix3D")
    {
        ::fwData::TransformationMatrix3D::sptr transValue = ::fwData::TransformationMatrix3D::dynamicCast(obj);
        SLM_ASSERT("The given TransformationMatrix3D object is null", transValue);

        float paramValues[16];

        for(int i = 0; i < 16; i++)
        {
            paramValues[i] = static_cast<float>(transValue->getCoefficients()[static_cast<size_t>(i)]);
        }

        params->setNamedConstant(m_paramName, paramValues, static_cast<size_t>(16), static_cast<size_t>(1));
    }
    else if(objClass == "::fwData::Array")
    {
        OSLM_ERROR("This Type  " << objClass << " isn't supported yet.");
    }
    else if(objClass == "::fwData::Vector")
    {
        ::fwData::Vector::sptr vectorValue = ::fwData::Vector::dynamicCast(obj);
        SLM_ASSERT("The given vector object is null", vectorValue);
        OSLM_ERROR("This Type  " << objClass << " isn't supported yet.");
    }
    // We allow to work on the SRender composite and interact with slots instead
    else if(objClass != "::fwData::Composite")
    {
        OSLM_ERROR("This Type  " << objClass << " isn't supported.");
    }

    return true;
}

//------------------------------------------------------------------------------

} // namespace visuOgreAdaptor
