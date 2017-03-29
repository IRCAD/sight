/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/IParameter.hpp"

#include "visuOgreAdaptor/defines.hpp"
#include "visuOgreAdaptor/SMaterial.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/Array.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/Color.hpp>
#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/Vector.hpp>

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/ArrayGetter.hpp>

#include <fwServices/macros.hpp>

#include <OgreGpuProgramParams.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgreTechnique.h>

namespace visuOgreAdaptor
{

//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType IParameter::s_SET_BOOL_PARAMETER_SLOT    = "setBoolParameter";
const ::fwCom::Slots::SlotKeyType IParameter::s_SET_COLOR_PARAMETER_SLOT   = "setColorParameter";
const ::fwCom::Slots::SlotKeyType IParameter::s_SET_DOUBLE_PARAMETER_SLOT  = "setDoubleParameter";
const ::fwCom::Slots::SlotKeyType IParameter::s_SET_DOUBLE2_PARAMETER_SLOT = "setDouble2Parameter";
const ::fwCom::Slots::SlotKeyType IParameter::s_SET_DOUBLE3_PARAMETER_SLOT = "setDouble3Parameter";
const ::fwCom::Slots::SlotKeyType IParameter::s_SET_INT_PARAMETER_SLOT     = "setIntParameter";
const ::fwCom::Slots::SlotKeyType IParameter::s_SET_INT2_PARAMETER_SLOT    = "setInt2Parameter";
const ::fwCom::Slots::SlotKeyType IParameter::s_SET_INT3_PARAMETER_SLOT    = "setInt3Parameter";

//------------------------------------------------------------------------------

IParameter::IParameter() throw() :
    m_shaderType(::Ogre::GPT_FRAGMENT_PROGRAM),
    m_dirty(true)
{
    newSlot(s_SET_BOOL_PARAMETER_SLOT, &IParameter::setBoolParameter, this);
    newSlot(s_SET_COLOR_PARAMETER_SLOT, &IParameter::setColorParameter, this);
    newSlot(s_SET_DOUBLE_PARAMETER_SLOT, &IParameter::setDoubleParameter, this);
    newSlot(s_SET_DOUBLE2_PARAMETER_SLOT, &IParameter::setDouble2Parameter, this);
    newSlot(s_SET_DOUBLE3_PARAMETER_SLOT, &IParameter::setDouble3Parameter, this);
    newSlot(s_SET_INT_PARAMETER_SLOT, &IParameter::setIntParameter, this);
    newSlot(s_SET_INT2_PARAMETER_SLOT, &IParameter::setInt2Parameter, this);
    newSlot(s_SET_INT3_PARAMETER_SLOT, &IParameter::setInt3Parameter, this);
}

//------------------------------------------------------------------------------

IParameter::~IParameter() throw()
{
}

//------------------------------------------------------------------------------

void IParameter::setShaderType(::Ogre::GpuProgramType shaderType)
{
    m_shaderType = shaderType;
}

//------------------------------------------------------------------------------

void IParameter::setParamName(const std::string& paramName)
{
    m_paramName = paramName;
}

//------------------------------------------------------------------------------

const std::string& IParameter::getParamName() const
{
    return m_paramName;
}

//------------------------------------------------------------------------------

void IParameter::doConfigure() throw(::fwTools::Failed)
{
    m_paramName = m_configuration->getAttributeValue("parameter");
    OSLM_ERROR_IF("parameter attribute not set", m_paramName.empty());

    m_techniqueName = m_configuration->getAttributeValue("technique");

    if ( m_configuration->hasAttribute("shaderType"))
    {
        std::string shaderType = m_configuration->getAttributeValue("shaderType");
        if (shaderType == "vertex")
        {
            m_shaderType = ::Ogre::GPT_VERTEX_PROGRAM;
        }
        else if (shaderType == "fragment")
        {
            m_shaderType = ::Ogre::GPT_FRAGMENT_PROGRAM;
        }
        else if (shaderType == "geometry")
        {
            m_shaderType = ::Ogre::GPT_GEOMETRY_PROGRAM;
        }
        else
        {
            OSLM_ERROR("This shader type " << shaderType << " isn't supported yet");
        }
    }
}

//------------------------------------------------------------------------------

void IParameter::doUpdate() throw(::fwTools::Failed)
{
    if(m_material.isNull() || !m_dirty)
    {
        return;
    }
    if(m_techniqueName.empty())
    {
        bool bSet = false;
        ::Ogre::Material::TechniqueIterator techIt = m_material->getTechniqueIterator();
        while( techIt.hasMoreElements())
        {
            ::Ogre::Technique* tech = techIt.getNext();
            SLM_ASSERT("Technique is not set", tech);

            bSet |= this->setParameter(*tech);
        }

        if( !bSet )
        {
            SLM_TRACE("Couldn't set parameter '" + m_paramName + "' in any technique of material '"
                      + m_material->getName() + "'");
        }
        else
        {
            this->requestRender();
        }
    }
    else
    {
        ::Ogre::Technique* tech = m_material->getTechnique(m_techniqueName);
        OSLM_FATAL_IF("Can't find technique " << m_techniqueName, !tech);

        if( this->setParameter(*tech) )
        {
            SLM_TRACE("Couldn't set parameter '" + m_paramName + "' in technique '" + m_techniqueName +
                      "' from material '" + m_material->getName() + "'");
        }
        else
        {
            this->requestRender();
        }
    }
    m_dirty = false;
}

//------------------------------------------------------------------------------

void IParameter::doStop() throw ( ::fwTools::Failed )
{
    m_material.setNull();
}

//------------------------------------------------------------------------------

bool IParameter::setParameter(::Ogre::Technique& technique)
{
    /// Contains the different parameters for the shader
    ::Ogre::GpuProgramParametersSharedPtr params;

    // Get the parameters
    auto pass = technique.getPass(0);
    if (m_shaderType == ::Ogre::GPT_VERTEX_PROGRAM)
    {
        params = pass->getVertexProgramParameters();
    }
    else if (m_shaderType == ::Ogre::GPT_FRAGMENT_PROGRAM && pass->hasFragmentProgram())
    {
        params = pass->getFragmentProgramParameters();
    }
    else if (m_shaderType == ::Ogre::GPT_GEOMETRY_PROGRAM)
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
    else if(objClass == "::fwData::PointList")
    {
        ::fwData::PointList::sptr pointListValue = ::fwData::PointList::dynamicCast(obj);
        SLM_ASSERT("The given pointList object is null", pointListValue);

        std::vector< ::fwData::Point::sptr > points = pointListValue->getPoints();
        int nbPoints                                = static_cast<int>(points.size());

        float* paramValues = new float[nbPoints * 3];

        for(int i = 0; i < nbPoints * 3; )
        {
            paramValues[i] = static_cast<float>(points[static_cast<size_t>(i)]->getCoord()[0]);
            i++;

            paramValues[i] = static_cast<float>(points[static_cast<size_t>(i)]->getCoord()[1]);
            i++;

            paramValues[i] = static_cast<float>(points[static_cast<size_t>(i)]->getCoord()[2]);
            i++;
        }

        params->setNamedConstant(m_paramName, paramValues, points.size(), static_cast<size_t>(3));

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
        ::fwData::Array::sptr arrayObject = ::fwData::Array::dynamicCast(obj);
        SLM_ASSERT("The object is NULL", arrayObject);

        size_t numComponents = arrayObject->getNumberOfComponents();
        if(numComponents <= 3)
        {
            ::fwDataTools::helper::ArrayGetter arrayHelper(arrayObject);

            if( arrayObject->getType() == ::fwTools::Type::s_FLOAT)
            {
                const float* floatValue = static_cast<const float*>(arrayHelper.getBuffer());
                params->setNamedConstant(m_paramName, floatValue, 1, numComponents);
            }
            else if( arrayObject->getType() == ::fwTools::Type::s_DOUBLE)
            {
                const double* doubleValue = static_cast<const double*>(arrayHelper.getBuffer());
                params->setNamedConstant(m_paramName, doubleValue, 1, numComponents);
            }
            else if( arrayObject->getType() == ::fwTools::Type::s_INT32)
            {
                const int* intValue = static_cast<const int*>(arrayHelper.getBuffer());
                params->setNamedConstant(m_paramName, intValue, 1, numComponents);
            }
            else
            {
                OSLM_ERROR("Array type not handled: " << arrayObject->getType());
            }
        }
        else
        {
            OSLM_ERROR("Array size not handled: " << arrayObject->getNumberOfComponents());
        }
    }
    // We allow to work on the SRender composite and interact with slots instead
    else if(objClass != "::fwData::Composite")
    {

        OSLM_ERROR("This Type " << objClass << " isn't supported.");
    }

    return true;
}

//------------------------------------------------------------------------------

void IParameter::setMaterial(const::Ogre::MaterialPtr& material)
{
    m_material = material;
}

//------------------------------------------------------------------------------

void IParameter::setBoolParameter(bool value, std::string name)
{
    if(name == m_paramName)
    {
        m_dirty = true;

        ::fwData::Object::sptr obj          = this->getObject();
        ::fwData::Boolean::sptr paramObject = ::fwData::Boolean::dynamicCast(obj);
        paramObject->setValue(value);

        this->updating();
    }
}

//------------------------------------------------------------------------------

void IParameter::setColorParameter(std::array<uint8_t, 4> color, std::string name)
{
    if(name == m_paramName)
    {
        m_dirty = true;

        ::fwData::Object::sptr obj        = this->getObject();
        ::fwData::Color::sptr paramObject = ::fwData::Color::dynamicCast(obj);
        paramObject->setRGBA(color[0] / 255.f, color[1] / 255.f, color[2] / 255.f, color[3] / 255.f);

        this->updating();
    }
}

//------------------------------------------------------------------------------

void IParameter::setIntParameter(int value, std::string name)
{
    if(name == m_paramName)
    {
        m_dirty = true;

        ::fwData::Object::sptr obj          = this->getObject();
        ::fwData::Integer::sptr paramObject = ::fwData::Integer::dynamicCast(obj);
        paramObject->setValue(value);

        this->updating();
    }
}

//------------------------------------------------------------------------------

void IParameter::setInt2Parameter(int value1, int value2, std::string name)
{
    if(name == m_paramName)
    {
        m_dirty = true;

        ::fwData::Object::sptr obj        = this->getObject();
        ::fwData::Array::sptr arrayObject = ::fwData::Array::dynamicCast(obj);

        if(arrayObject->empty())
        {
            ::fwTools::Type type = ::fwTools::Type::create< ::fwTools::Type::Int32Type>();
            arrayObject->resize( type, {1}, 2, true);
        }

        ::fwDataTools::helper::Array arrayHelper(arrayObject);
        int vec[2] = { value1, value2 };
        arrayHelper.setItem( {0}, vec);

        this->updating();
    }
}

//------------------------------------------------------------------------------

void IParameter::setInt3Parameter(int value1, int value2, int value3, std::string name)
{
    if(name == m_paramName)
    {
        m_dirty = true;

        ::fwData::Object::sptr obj        = this->getObject();
        ::fwData::Array::sptr arrayObject = ::fwData::Array::dynamicCast(obj);

        if(arrayObject->empty())
        {
            ::fwTools::Type type = ::fwTools::Type::create< ::fwTools::Type::Int32Type>();
            arrayObject->resize( type, {1}, 3, true);
        }

        ::fwDataTools::helper::Array arrayHelper(arrayObject);
        int vec[3] = { value1, value2, value3 };
        arrayHelper.setItem( {0}, vec);

        this->updating();
    }
}

//------------------------------------------------------------------------------

void IParameter::setDoubleParameter(double value, std::string name)
{
    if(name == m_paramName)
    {
        m_dirty = true;

        ::fwData::Object::sptr obj        = this->getObject();
        ::fwData::Float::sptr paramObject = ::fwData::Float::dynamicCast(obj);
        paramObject->setValue(static_cast<float>(value));

        this->updating();
    }
}

//------------------------------------------------------------------------------

void IParameter::setDouble2Parameter(double value1, double value2, std::string name)
{
    if(name == m_paramName)
    {
        m_dirty = true;

        ::fwData::Object::sptr obj        = this->getObject();
        ::fwData::Array::sptr arrayObject = ::fwData::Array::dynamicCast(obj);

        if(arrayObject->empty())
        {
            ::fwTools::Type type = ::fwTools::Type::create< ::fwTools::Type::DoubleType>();
            arrayObject->resize( type, {1}, 2, true);
        }

        ::fwDataTools::helper::Array arrayHelper(arrayObject);

        if( arrayObject->getType() == ::fwTools::Type::s_FLOAT)
        {
            float vec[2] = {  static_cast<float>(value1),  static_cast<float>(value2) };
            arrayHelper.setItem( {0}, vec);
        }
        else if( arrayObject->getType() == ::fwTools::Type::s_DOUBLE)
        {
            double vec[2] = { value1, value2 };
            arrayHelper.setItem( {0}, vec);
        }

        this->updating();
    }
}

//------------------------------------------------------------------------------

void IParameter::setDouble3Parameter(double value1, double value2, double value3, std::string name)
{
    if(name == m_paramName)
    {
        m_dirty = true;

        ::fwData::Object::sptr obj        = this->getObject();
        ::fwData::Array::sptr arrayObject = ::fwData::Array::dynamicCast(obj);

        if(arrayObject->empty())
        {
            ::fwTools::Type type = ::fwTools::Type::create< ::fwTools::Type::DoubleType>();
            arrayObject->resize( type, {1}, 3, true);
        }

        ::fwDataTools::helper::Array arrayHelper(arrayObject);

        if( arrayObject->getType() == ::fwTools::Type::s_FLOAT)
        {
            float vec[3] = { static_cast<float>(value1), static_cast<float>(value2), static_cast<float>(value3) };
            arrayHelper.setItem( {0}, vec);
        }
        else if( arrayObject->getType() == ::fwTools::Type::s_DOUBLE)
        {
            double vec[3] = { value1, value2, value3 };
            arrayHelper.setItem( {0}, vec);
        }
        this->updating();
    }
}

//------------------------------------------------------------------------------

} // namespace visuOgreAdaptor
