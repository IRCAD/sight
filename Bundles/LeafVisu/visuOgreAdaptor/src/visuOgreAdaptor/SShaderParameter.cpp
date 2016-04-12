/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SShaderParameter.hpp"

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

#include <fwServices/Base.hpp>

#include <OgreGpuProgramParams.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgreTechnique.h>

#include "visuOgreAdaptor/defines.hpp"
#include "visuOgreAdaptor/SMaterial.hpp"

namespace visuOgreAdaptor
{

fwServicesRegisterMacro( ::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SShaderParameter, ::fwData::Object);

const ::fwCom::Slots::SlotKeyType SShaderParameter::s_SET_INT_PARAMETER_SLOT   = "setIntParameter";
const ::fwCom::Slots::SlotKeyType SShaderParameter::s_SET_FLOAT_PARAMETER_SLOT = "setFloatParameter";

//------------------------------------------------------------------------------

SShaderParameter::SShaderParameter() throw() :
    m_shaderType(VERTEX)
{
    newSlot(s_SET_INT_PARAMETER_SLOT, &SShaderParameter::setIntParameter, this);
    newSlot(s_SET_FLOAT_PARAMETER_SLOT, &SShaderParameter::setFloatParameter, this);
}

//------------------------------------------------------------------------------

SShaderParameter::~SShaderParameter() throw()
{
}

//------------------------------------------------------------------------------

void SShaderParameter::setShaderType(std::string shaderType)
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

void SShaderParameter::setShaderType(ShaderEnumType shaderType)
{
    m_shaderType = shaderType;
}

//------------------------------------------------------------------------------

void SShaderParameter::setMaterialName(std::string matName)
{
    m_materialName = matName;
}

//------------------------------------------------------------------------------

void SShaderParameter::setParamName(std::string paramName)
{
    m_paramName = paramName;
}

//------------------------------------------------------------------------------

void SShaderParameter::doStart() throw(::fwTools::Failed)
{
    this->updateValue(nullptr);
}

//------------------------------------------------------------------------------

void SShaderParameter::doStop() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SShaderParameter::doSwap() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SShaderParameter::doConfigure() throw(::fwTools::Failed)
{
    SLM_ASSERT("Not a \"config\" configuration", m_configuration->getName() == "config");

    m_materialName = m_configuration->getAttributeValue("materialName");
    OSLM_ERROR_IF("material attribute not set", m_materialName.empty());

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

void SShaderParameter::doUpdate() throw(::fwTools::Failed)
{
    this->updateValue(nullptr);
    this->requestRender();
}

//------------------------------------------------------------------------------

void SShaderParameter::updateValue(const fwData::Object::sptr& paramObject)
{
    // Retrieves the associated material
    ::Ogre::MaterialPtr material = ::Ogre::MaterialManager::getSingleton().getByName(m_materialName);

    if(m_techniqueName.empty())
    {
        bool bSet = false;
        ::Ogre::Material::TechniqueIterator techIt = material->getTechniqueIterator();
        while( techIt.hasMoreElements())
        {
            ::Ogre::Technique* tech = techIt.getNext();
            SLM_ASSERT("Technique is not set", tech);

            bSet |= this->setParameter(*tech, paramObject);
        }

        if( !bSet )
        {
            SLM_ERROR("Couldn't set parameter '" + m_paramName + "' in any technique of material '"
                      + m_materialName + "'");
        }
    }
    else
    {
        ::Ogre::Technique* tech = nullptr;
        tech                    = material->getTechnique(m_techniqueName);
        OSLM_FATAL_IF("Can't find technique " << m_techniqueName, !tech);

        if( this->setParameter(*tech, paramObject) )
        {
            SLM_ERROR("Couldn't set parameter '" + m_paramName + "' in technique '" + m_techniqueName +
                      "' from material '" + m_materialName + "'");
        }
    }
}

//------------------------------------------------------------------------------

bool SShaderParameter::setParameter(::Ogre::Technique& technique, const fwData::Object::sptr& paramObject)
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
    if(paramObject != nullptr)
    {
        obj = paramObject;
    }

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
//        ::fwData::Point::sptr pointValue = ::fwData::Point::dynamicCast(obj);
//        SLM_ASSERT("Sorry, the object is null", pointValue);

//        m_paramValues = new float[3];

//        m_paramValues[0] = static_cast<float>(pointValue->getCoord()[0]);
//        m_paramValues[1] = static_cast<float>(pointValue->getCoord()[1]);
//        m_paramValues[2] = static_cast<float>(pointValue->getCoord()[2]);

//        m_paramNbElem = 3;
//        ::Ogre::Vector3 point(m_paramValues);
//        m_params->setNamedConstant(m_paramName, point);
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
//        ::fwData::Array::sptr arrayValue = ::fwData::Array::dynamicCast(obj);
//        SLM_ASSERT("Sorry, the object is null", arrayValue);

//        ::fwComEd::helper::Array arrayHelper(arrayValue);
//        float* floatValue = static_cast<float *>(arrayHelper.getBuffer());
//        int dimension = arrayValue->getNumberOfComponents();
//        m_paramValues = new float[dimension];
//        for(int i = 0; i < dimension; i++)
//        {
//            m_paramValues[i] = floatValue[i];
//        }

//        m_paramNbElem = dimension;
//        m_params->setNamedConstant(m_paramName, m_paramValues, static_cast<size_t>(m_paramNbElem, 1));
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

void SShaderParameter::setIntParameter(int value)
{
    if(m_paramObject == nullptr)
    {
        m_paramObject = ::fwData::Integer::New();
    }
    ::fwData::Integer::sptr paramObject = ::fwData::Integer::dynamicCast(m_paramObject);
    paramObject->setValue(value);

    this->updateValue(paramObject);
}

//------------------------------------------------------------------------------

void SShaderParameter::setFloatParameter(float value)
{
    if(m_paramObject == nullptr)
    {
        m_paramObject = ::fwData::Float::New();
    }
    ::fwData::Float::sptr paramObject = ::fwData::Float::dynamicCast(m_paramObject);
    paramObject->setValue(value);

    this->updateValue(paramObject);
}

//------------------------------------------------------------------------------

} // namespace visuOgreAdaptor
