/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "viz/scene3d/IParameter.hpp"

#include <core/com/Slots.hxx>

#include <data/Array.hpp>
#include <data/Boolean.hpp>
#include <data/Color.hpp>
#include <data/Float.hpp>
#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/Matrix4.hpp>
#include <data/mt/ObjectReadLock.hpp>
#include <data/Point.hpp>
#include <data/PointList.hpp>

#include <service/macros.hpp>

#include <viz/scene3d/ogre.hpp>

#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTextureManager.h>

namespace sight::viz::scene3d
{

//------------------------------------------------------------------------------

const core::com::Slots::SlotKeyType IParameter::s_SET_BOOL_PARAMETER_SLOT    = "setBoolParameter";
const core::com::Slots::SlotKeyType IParameter::s_SET_COLOR_PARAMETER_SLOT   = "setColorParameter";
const core::com::Slots::SlotKeyType IParameter::s_SET_DOUBLE_PARAMETER_SLOT  = "setDoubleParameter";
const core::com::Slots::SlotKeyType IParameter::s_SET_DOUBLE2_PARAMETER_SLOT = "setDouble2Parameter";
const core::com::Slots::SlotKeyType IParameter::s_SET_DOUBLE3_PARAMETER_SLOT = "setDouble3Parameter";
const core::com::Slots::SlotKeyType IParameter::s_SET_INT_PARAMETER_SLOT     = "setIntParameter";
const core::com::Slots::SlotKeyType IParameter::s_SET_INT2_PARAMETER_SLOT    = "setInt2Parameter";
const core::com::Slots::SlotKeyType IParameter::s_SET_INT3_PARAMETER_SLOT    = "setInt3Parameter";

const std::string IParameter::s_PARAMETER_INOUT = "parameter";

//------------------------------------------------------------------------------

IParameter::IParameter() noexcept :
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

IParameter::~IParameter() noexcept
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

service::IService::KeyConnectionsMap IParameter::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push(s_PARAMETER_INOUT, data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void IParameter::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    m_paramName = config.get<std::string>("parameter", "");
    SIGHT_ERROR_IF("parameter attribute not set", m_paramName.empty());

    m_techniqueName = config.get<std::string>("technique", "");

    if(config.count("shaderType"))
    {
        const std::string shaderType = config.get<std::string>("shaderType");
        if(shaderType == "vertex")
        {
            m_shaderType = ::Ogre::GPT_VERTEX_PROGRAM;
        }
        else if(shaderType == "fragment")
        {
            m_shaderType = ::Ogre::GPT_FRAGMENT_PROGRAM;
        }
        else if(shaderType == "geometry")
        {
            m_shaderType = ::Ogre::GPT_GEOMETRY_PROGRAM;
        }
        else
        {
            SIGHT_ERROR("This shader type " << shaderType << " isn't supported yet");
        }
    }
}

//------------------------------------------------------------------------------

void IParameter::updating()
{
    if(!m_material || !m_dirty)
    {
        return;
    }

    this->getRenderService()->makeCurrent();
    if(m_techniqueName.empty())
    {
        bool bSet                                      = false;
        const ::Ogre::Material::Techniques& techniques = m_material->getTechniques();

        for(const auto tech : techniques)
        {
            SIGHT_ASSERT("Technique is not set", tech);

            bSet |= this->setParameter(*tech);
        }

        if(!bSet)
        {
            SIGHT_DEBUG(
                "Couldn't set parameter '" + m_paramName + "' in any technique of material '"
                + m_material->getName() + "'"
            );
        }
        else
        {
            this->requestRender();
        }
    }
    else
    {
        ::Ogre::Technique* tech = m_material->getTechnique(m_techniqueName);
        SIGHT_FATAL_IF("Can't find technique " << m_techniqueName, !tech);

        if(this->setParameter(*tech))
        {
            SIGHT_DEBUG(
                "Couldn't set parameter '" + m_paramName + "' in technique '" + m_techniqueName
                + "' from material '" + m_material->getName() + "'"
            );
        }
        else
        {
            this->requestRender();
        }
    }

    m_dirty = false;
}

//------------------------------------------------------------------------------

void IParameter::stopping()
{
    this->getRenderService()->makeCurrent();
    m_material.reset();

    if(m_texture)
    {
        ::Ogre::TextureManager::getSingleton().remove(m_texture);
        m_texture.reset();
    }
}

//------------------------------------------------------------------------------

bool IParameter::setParameter(::Ogre::Technique& technique)
{
    /// Contains the different parameters for the shader
    ::Ogre::GpuProgramParametersSharedPtr params;

    // Get the parameters
    auto pass = technique.getPass(0);
    if(m_shaderType == ::Ogre::GPT_VERTEX_PROGRAM)
    {
        params = pass->getVertexProgramParameters();
    }
    else if(m_shaderType == ::Ogre::GPT_FRAGMENT_PROGRAM && pass->hasFragmentProgram())
    {
        params = pass->getFragmentProgramParameters();
    }
    else if(m_shaderType == ::Ogre::GPT_GEOMETRY_PROGRAM)
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

    data::Object::sptr obj = this->getInOut<data::Object>(s_PARAMETER_INOUT);

    // Set shader parameters
    std::string objClass = obj->getClassname();
    if(objClass == "sight::data::Integer")
    {
        data::Integer::sptr intValue = data::Integer::dynamicCast(obj);
        SIGHT_ASSERT("The given integer object is null", intValue);

        params->setNamedConstant(m_paramName, static_cast<int>(intValue->value()));
    }
    else if(objClass == "sight::data::Float")
    {
        data::Float::sptr floatValue = data::Float::dynamicCast(obj);
        SIGHT_ASSERT("The given float object is null", floatValue);

        params->setNamedConstant(m_paramName, floatValue->value());
    }
    else if(objClass == "sight::data::Boolean")
    {
        data::Boolean::sptr booleanValue = data::Boolean::dynamicCast(obj);
        SIGHT_ASSERT("The given boolean object is null", booleanValue);

        params->setNamedConstant(m_paramName, static_cast<int>(booleanValue->value()));
    }
    else if(objClass == "sight::data::Color")
    {
        data::Color::sptr colorValue = data::Color::dynamicCast(obj);
        SIGHT_ASSERT("The given color object is null", colorValue);

        float paramValues[4];

        paramValues[0] = colorValue->red();
        paramValues[1] = colorValue->green();
        paramValues[2] = colorValue->blue();
        paramValues[3] = colorValue->alpha();

        ::Ogre::ColourValue color(paramValues[0], paramValues[1], paramValues[2], paramValues[3]);

        params->setNamedConstant(m_paramName, color);
    }
    else if(objClass == "sight::data::PointList")
    {
        data::PointList::sptr pointListValue = data::PointList::dynamicCast(obj);
        SIGHT_ASSERT("The given pointList object is null", pointListValue);

        std::vector<data::Point::sptr> points = pointListValue->getPoints();
        int nbPoints                          = static_cast<int>(points.size());

        float* paramValues = new float [static_cast<unsigned long long>(nbPoints * 3)];

        for(int i = 0 ; i < nbPoints * 3 ; )
        {
            paramValues[i] = static_cast<float>(points[static_cast<size_t>(i)]->getCoord()[0]);
            i++;

            paramValues[i] = static_cast<float>(points[static_cast<size_t>(i)]->getCoord()[1]);
            i++;

            paramValues[i] = static_cast<float>(points[static_cast<size_t>(i)]->getCoord()[2]);
            i++;
        }

        params->setNamedConstant(m_paramName, paramValues, points.size(), static_cast<size_t>(3));

        delete[] paramValues;
    }
    else if(objClass == "sight::data::Matrix4")
    {
        data::Matrix4::sptr transValue = data::Matrix4::dynamicCast(obj);
        SIGHT_ASSERT("The given Matrix4 object is null", transValue);

        float paramValues[16];

        for(int i = 0 ; i < 16 ; i++)
        {
            paramValues[i] = static_cast<float>(transValue->getCoefficients()[static_cast<size_t>(i)]);
        }

        params->setNamedConstant(m_paramName, paramValues, static_cast<size_t>(16), static_cast<size_t>(1));
    }
    else if(objClass == "sight::data::Array")
    {
        data::Array::sptr arrayObject = data::Array::dynamicCast(obj);
        SIGHT_ASSERT("The object is nullptr", arrayObject);

        const size_t numComponents = arrayObject->getSize()[0];
        if(numComponents <= 3)
        {
            const auto dumpLock = arrayObject->lock();

            if(arrayObject->getType() == core::tools::Type::s_FLOAT)
            {
                const float* floatValue = static_cast<const float*>(arrayObject->getBuffer());
                params->setNamedConstant(m_paramName, floatValue, 1, numComponents);
            }
            else if(arrayObject->getType() == core::tools::Type::s_DOUBLE)
            {
                const double* doubleValue = static_cast<const double*>(arrayObject->getBuffer());
                params->setNamedConstant(m_paramName, doubleValue, 1, numComponents);
            }
            else if(arrayObject->getType() == core::tools::Type::s_INT32)
            {
                const int* intValue = static_cast<const int*>(arrayObject->getBuffer());
                params->setNamedConstant(m_paramName, intValue, 1, numComponents);
            }
            else
            {
                SIGHT_ERROR("Array type not handled: " << arrayObject->getType());
            }
        }
        else
        {
            SIGHT_ERROR("Array size not handled: " << arrayObject->getSize()[0]);
        }
    }
    else if(objClass == "sight::data::Image")
    {
        data::Image::sptr image = data::Image::dynamicCast(obj);
        SIGHT_ASSERT("The object is nullptr", image);

        data::mt::ObjectReadLock lock(image);

        if(!m_texture)
        {
            m_texture = ::Ogre::TextureManager::getSingleton().create(
                this->getID() + "_TextureParam",
                viz::scene3d::RESOURCE_GROUP,
                true
            );
        }

        // We can reach this code for an another reason than an image modification, for instance when the compositor
        // is resized. However I don't know how to discriminate the two cases so for now we always copy the image. :/
        if(image->getSizeInBytes())
        {
            viz::scene3d::Utils::loadOgreTexture(image, m_texture, ::Ogre::TEX_TYPE_2D, true);

            ::Ogre::TextureUnitState* texState = pass->getTextureUnitState(m_paramName);
            texState->setTexture(m_texture);

            auto texUnitIndex = pass->getTextureUnitStateIndex(texState);
            params->setNamedConstant(m_paramName, texUnitIndex);
        }
    }
    // We allow to work on the SRender composite and interact with slots instead
    else if(objClass != "sight::data::Composite")
    {
        SIGHT_ERROR("This Type " << objClass << " isn't supported.");
    }

    return true;
}

//------------------------------------------------------------------------------

void IParameter::setMaterial(const ::Ogre::MaterialPtr& material)
{
    m_material = material;
}

//------------------------------------------------------------------------------

void IParameter::setBoolParameter(bool value, std::string name)
{
    if(name == m_paramName)
    {
        m_dirty = true;

        data::Boolean::sptr paramObject = this->getInOut<data::Boolean>(s_PARAMETER_INOUT);
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

        data::Color::sptr paramObject = this->getInOut<data::Color>(s_PARAMETER_INOUT);
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

        data::Integer::sptr paramObject = this->getInOut<data::Integer>(s_PARAMETER_INOUT);
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

        data::Array::sptr arrayObject = this->getInOut<data::Array>(s_PARAMETER_INOUT);

        if(arrayObject->empty())
        {
            arrayObject->resize({2}, core::tools::Type::s_INT32);
        }

        const auto dumpLock = arrayObject->lock();
        arrayObject->at<std::uint32_t>(0) = static_cast<std::uint32_t>(value1);
        arrayObject->at<std::uint32_t>(1) = static_cast<std::uint32_t>(value2);

        this->updating();
    }
}

//------------------------------------------------------------------------------

void IParameter::setInt3Parameter(int value1, int value2, int value3, std::string name)
{
    if(name == m_paramName)
    {
        m_dirty = true;

        data::Array::sptr arrayObject = this->getInOut<data::Array>(s_PARAMETER_INOUT);

        if(arrayObject->empty())
        {
            arrayObject->resize({3}, core::tools::Type::s_INT32);
        }

        const auto dumpLock = arrayObject->lock();
        arrayObject->at<std::uint32_t>(0) = static_cast<std::uint32_t>(value1);
        arrayObject->at<std::uint32_t>(1) = static_cast<std::uint32_t>(value2);
        arrayObject->at<std::uint32_t>(2) = static_cast<std::uint32_t>(value3);

        this->updating();
    }
}

//------------------------------------------------------------------------------

void IParameter::setDoubleParameter(double value, std::string name)
{
    if(name == m_paramName)
    {
        m_dirty = true;

        data::Float::sptr paramObject = this->getInOut<data::Float>(s_PARAMETER_INOUT);
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

        data::Array::sptr arrayObject = this->getInOut<data::Array>(s_PARAMETER_INOUT);
        if(arrayObject->empty())
        {
            core::tools::Type type = core::tools::Type::create<core::tools::Type::DoubleType>();
            arrayObject->resize({2}, core::tools::Type::s_DOUBLE);
        }

        const auto dumpLock = arrayObject->lock();

        if(arrayObject->getType() == core::tools::Type::s_FLOAT)
        {
            arrayObject->at<float>(0) = static_cast<float>(value1);
            arrayObject->at<float>(1) = static_cast<float>(value2);
        }
        else if(arrayObject->getType() == core::tools::Type::s_DOUBLE)
        {
            arrayObject->at<double>(0) = value1;
            arrayObject->at<double>(1) = value2;
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

        data::Array::sptr arrayObject = this->getInOut<data::Array>(s_PARAMETER_INOUT);

        if(arrayObject->empty())
        {
            core::tools::Type type = core::tools::Type::create<core::tools::Type::DoubleType>();
            arrayObject->resize({3}, core::tools::Type::s_DOUBLE);
        }

        const auto dumpLock = arrayObject->lock();

        if(arrayObject->getType() == core::tools::Type::s_FLOAT)
        {
            arrayObject->at<float>(0) = static_cast<float>(value1);
            arrayObject->at<float>(1) = static_cast<float>(value2);
            arrayObject->at<float>(2) = static_cast<float>(value3);
        }
        else if(arrayObject->getType() == core::tools::Type::s_DOUBLE)
        {
            arrayObject->at<double>(0) = value1;
            arrayObject->at<double>(1) = value2;
            arrayObject->at<double>(2) = value3;
        }

        this->updating();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d
