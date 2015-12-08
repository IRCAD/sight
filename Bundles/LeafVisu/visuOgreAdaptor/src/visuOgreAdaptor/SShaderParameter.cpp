/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SShaderParameter.hpp"

#include <fwComEd/helper/Array.hpp>

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

//------------------------------------------------------------------------------

SShaderParameter::SShaderParameter() throw() :
    m_paramValues(nullptr),
    m_paramNbElem(0),
    m_paramElemMultiple(1),
    m_materialName(SMaterial::DEFAULT_MATERIAL_TEMPLATE_NAME),
    m_paramName("defaultParam"),
    m_shaderType(VERTEX)
{
}

//------------------------------------------------------------------------------

SShaderParameter::~SShaderParameter() throw()
{
    if (m_paramValues)
    {
        delete m_paramValues;
    }
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
    if ( m_configuration->hasAttribute("material"))
    {
        m_materialName = m_configuration->getAttributeValue("material");
    }

    if ( m_configuration->hasAttribute("parameterName"))
    {
        m_paramName = m_configuration->getAttributeValue("parameterName");
    }

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
    this->updateValue();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SShaderParameter::updateValue()
{
    // Retrieves the associated material
    ::Ogre::MaterialPtr material = ::Ogre::MaterialManager::getSingleton().getByName(m_materialName);

    // Loads the shader containing the parameter
    if (m_shaderType == VERTEX)
    {
        m_params = material->getTechnique(0)->getPass(0)->getVertexProgramParameters();
    }
    else if (m_shaderType == FRAGMENT)
    {
        m_params = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    }
    else if (m_shaderType == GEOMETRY)
    {
        m_params = material->getTechnique(0)->getPass(0)->getGeometryProgramParameters();
    }

    // Set shader parameters
    ::fwData::Object::sptr obj = this->getObject();
    std::string objClass = obj->getClassname();
    int m_paramNbElem    = 0;

    if(objClass == "::fwData::Integer")
    {
        ::fwData::Integer::sptr intValue = ::fwData::Integer::dynamicCast(obj);
        SLM_ASSERT("The given integer object is null", intValue);

        m_paramValues  = new float;
        *m_paramValues = static_cast<float>(intValue->value());

        m_paramNbElem = 1;
        m_params->setNamedConstant(m_paramName, static_cast<int>(*m_paramValues));
    }
    else if(objClass == "::fwData::Float")
    {
        ::fwData::Float::sptr floatValue = ::fwData::Float::dynamicCast(obj);
        SLM_ASSERT("The given float object is null", floatValue);

        m_paramValues  = new float;
        *m_paramValues = floatValue->value();

        m_paramNbElem = 1;
        m_params->setNamedConstant(m_paramName, *m_paramValues);
    }
    else if(objClass == "::fwData::Boolean")
    {
        ::fwData::Boolean::sptr booleanValue = ::fwData::Boolean::dynamicCast(obj);
        SLM_ASSERT("The given boolean object is null", booleanValue);

        m_paramValues  = new float;
        *m_paramValues = booleanValue->value();

        m_paramNbElem = 1;
        m_params->setNamedConstant(m_paramName, static_cast< int>(*m_paramValues));
    }
    else if(objClass == "::fwData::Color")
    {
        ::fwData::Color::sptr colorValue = ::fwData::Color::dynamicCast(obj);
        SLM_ASSERT("The given color object is null", colorValue);

        m_paramValues = new float[4];

        m_paramValues[0] = colorValue->red();
        m_paramValues[1] = colorValue->green();
        m_paramValues[2] = colorValue->blue();
        m_paramValues[3] = colorValue->alpha();

        m_paramNbElem = 4;
        ::Ogre::ColourValue color(m_paramValues[0],
                                  m_paramValues[1],
                                  m_paramValues[2],
                                  m_paramValues[3]);

        m_params->setNamedConstant(m_paramName, color);
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

        m_paramValues = new float[nbPoints * 3];

        for(int i = 0; i < nbPoints * 3; i++)
        {
            m_paramValues[i] = static_cast<float>(points[static_cast<size_t>(i)]->getCoord()[0]);
            i++;

            m_paramValues[i] = static_cast<float>(points[static_cast<size_t>(i)]->getCoord()[1]);
            i++;

            m_paramValues[i] = static_cast<float>(points[static_cast<size_t>(i)]->getCoord()[2]);
            i++;
        }

        m_paramNbElem       = nbPoints;
        m_paramElemMultiple = 3;
        m_params->setNamedConstant(m_paramName, m_paramValues, static_cast<size_t>(m_paramNbElem),
                                   static_cast<size_t>(m_paramElemMultiple));
    }
    else if(objClass == "::fwData::TransformationMatrix3D")
    {
        ::fwData::TransformationMatrix3D::sptr transValue = ::fwData::TransformationMatrix3D::dynamicCast(obj);
        SLM_ASSERT("The given TransformationMatrix3D object is null", transValue);

        m_paramValues = new float[4 * 4];

        for(int i = 0; i < 4 * 4; i++)
        {
            m_paramValues[i] = static_cast<float>(transValue->getCoefficients()[static_cast<size_t>(i)]);
        }

        m_paramNbElem = 16;
        m_params->setNamedConstant(m_paramName, m_paramValues, static_cast<size_t>(m_paramNbElem),
                                   static_cast<size_t>(m_paramElemMultiple));
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
    else
    {
        OSLM_ERROR("This Type  " << objClass << " isn't supported yet.");
    }
}

//------------------------------------------------------------------------------

} // namespace visuOgreAdaptor
