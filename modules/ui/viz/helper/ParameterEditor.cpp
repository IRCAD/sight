/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "modules/ui/viz/helper/ParameterEditor.hpp"

#include <data/Array.hpp>
#include <data/Color.hpp>
#include <data/Float.hpp>
#include <data/Integer.hpp>

namespace sight::module::ui::viz
{

namespace helper
{

//-----------------------------------------------------------------------------

template<typename T>
std::pair<T, T> getRange(T _value)
{
    static const T range = 10;
    T max;
    T min;

    // For _value > 1, we use [-_value * range;+_value *range]
    if(_value > 1)
    {
        max = +_value * range;
        min = -_value * range;
    }
    else if(_value < -1)
    // For _value < 0, we use [_value * range;-_value *range]
    {
        max = -_value * range;
        min = +_value * range;
    }
    else if(_value > 0 || _value < 0)
    // For -1 < value < 0 < _value < 1, we use [_value / range;+_value * range]
    {
        max = _value * range;
        min = _value / range;
    }
    else
    // For _value == 0, we use [0; 1]
    {
        max = 0;
        min = 1;
    }

    return std::make_pair(min, max);
}

//-----------------------------------------------------------------------------

service::IService::ConfigType ParameterEditor::createConfig(
    const sight::viz::scene3d::IParameter::csptr& _adaptor,
    const service::IService::csptr& _paramSrv,
    core::com::helper::SigSlotConnection& _connections
)
{
    service::IService::ConfigType paramConfig;

    /// Getting associated object infos
    const data::Object::csptr shaderObj =
        _adaptor->getInOut<data::Object>(sight::viz::scene3d::IParameter::s_PARAMETER_INOUT);

    const auto& objType = shaderObj->getClassname();

    if(objType == "sight::data::Boolean")
    {
        _connections.connect(_paramSrv, "boolChanged", _adaptor, "setBoolParameter");

        paramConfig.add("<xmlattr>.type", "bool");
        paramConfig.add("<xmlattr>.name", _adaptor->getParamName());
        paramConfig.add("<xmlattr>.key", _adaptor->getParamName());
        paramConfig.add("<xmlattr>.defaultValue", false);
    }
    else if(objType == "sight::data::Color")
    {
        _connections.connect(_paramSrv, "colorChanged", _adaptor, "setColorParameter");

        auto colorValue = data::Color::dynamicCast(shaderObj);

        int r = static_cast<unsigned char>(colorValue->red() * 255);
        int g = static_cast<unsigned char>(colorValue->green() * 255);
        int b = static_cast<unsigned char>(colorValue->blue() * 255);
        int a = static_cast<unsigned char>(colorValue->alpha() * 255);

        std::stringstream hexStr;
        hexStr << "#" << std::hex;
        hexStr << ((r < 0x10) ? "0" : "") << r;
        hexStr << ((g < 0x10) ? "0" : "") << g;
        hexStr << ((b < 0x10) ? "0" : "") << b;
        hexStr << ((a < 0x10) ? "0" : "") << a;

        paramConfig.add("<xmlattr>.type", "color");
        paramConfig.add("<xmlattr>.name", _adaptor->getParamName());
        paramConfig.add("<xmlattr>.key", _adaptor->getParamName());
        paramConfig.add("<xmlattr>.defaultValue", hexStr.str());
    }
    else if(objType == "sight::data::Float")
    {
        _connections.connect(_paramSrv, "doubleChanged", _adaptor, "setDoubleParameter");

        auto floatValue           = data::Float::dynamicCast(shaderObj);
        const double defaultValue = static_cast<double>(floatValue->value());
        const auto minmax         = getRange(defaultValue);
        const double min          = minmax.first;
        const double max          = minmax.second;

        paramConfig.add("<xmlattr>.type", "double");
        paramConfig.add("<xmlattr>.name", _adaptor->getParamName());
        paramConfig.add("<xmlattr>.key", _adaptor->getParamName());
        paramConfig.add("<xmlattr>.defaultValue", std::to_string(defaultValue));
        paramConfig.add("<xmlattr>.min", min);
        paramConfig.add("<xmlattr>.max", max);
    }
    else if(objType == "sight::data::Integer")
    {
        _connections.connect(_paramSrv, "intChanged", _adaptor, "setIntParameter");

        auto intValue          = data::Integer::dynamicCast(shaderObj);
        const int defaultValue = intValue->value();
        const auto minmax      = getRange(defaultValue);
        const int min          = minmax.first;
        const int max          = minmax.second;

        paramConfig.add("<xmlattr>.type", "int");
        paramConfig.add("<xmlattr>.name", _adaptor->getParamName());
        paramConfig.add("<xmlattr>.key", _adaptor->getParamName());
        paramConfig.add("<xmlattr>.defaultValue", std::to_string(defaultValue));
        paramConfig.add("<xmlattr>.min", min);
        paramConfig.add("<xmlattr>.max", max);
    }
    else if(objType == "sight::data::Array")
    {
        auto arrayObject         = data::Array::dynamicCast(shaderObj);
        const auto numComponents = arrayObject->getSize()[0];
        if(numComponents <= 3)
        {
            std::string strSize = std::to_string(numComponents);

            if(arrayObject->getType() == core::tools::Type::s_FLOAT
               || arrayObject->getType() == core::tools::Type::s_DOUBLE)
            {
                _connections.connect(
                    _paramSrv,
                    "double" + strSize + "Changed",
                    _adaptor,
                    "setDouble" + strSize + "Parameter"
                );

                // We can't give a default value for each component to SParameters :/
                // For now fill it with the first one
                const auto dumpLock = arrayObject->lock();

                double defaultValue;
                if(arrayObject->getType() == core::tools::Type::s_FLOAT)
                {
                    defaultValue = static_cast<double>(arrayObject->at<float>(0));
                }
                else
                {
                    defaultValue = arrayObject->at<double>(0);
                }

                const auto minmax = getRange(defaultValue);
                const double min  = minmax.first;
                const double max  = minmax.second;

                paramConfig.add("<xmlattr>.type", "double" + strSize);
                paramConfig.add("<xmlattr>.name", _adaptor->getParamName());
                paramConfig.add("<xmlattr>.key", _adaptor->getParamName());
                paramConfig.add("<xmlattr>.defaultValue", std::to_string(defaultValue));
                paramConfig.add("<xmlattr>.min", min);
                paramConfig.add("<xmlattr>.max", max);
            }
            else if(arrayObject->getType() == core::tools::Type::s_INT32)
            {
                _connections.connect(
                    _paramSrv,
                    "int" + strSize + "Changed",
                    _adaptor,
                    "setInt" + strSize + "Parameter"
                );
                const auto dumpLock = arrayObject->lock();

                const int defaultValue = arrayObject->at<std::int32_t>(0);
                const auto minmax      = getRange(defaultValue);
                const int min          = minmax.first;
                const int max          = minmax.second;

                paramConfig.add("<xmlattr>.type", "int" + strSize);
                paramConfig.add("<xmlattr>.name", _adaptor->getParamName());
                paramConfig.add("<xmlattr>.key", _adaptor->getParamName());
                paramConfig.add("<xmlattr>.defaultValue", std::to_string(defaultValue));
                paramConfig.add("<xmlattr>.min", min);
                paramConfig.add("<xmlattr>.max", max);
            }
            else
            {
                SIGHT_ERROR("Array type not handled: " << arrayObject->getType());
            }
        }
        else
        {
            SIGHT_ERROR("Array size not handled: " << numComponents);
        }
    }
    else
    {
        SIGHT_ERROR("No editor found for the object of type " << objType);
    }

    return paramConfig;
}

//-----------------------------------------------------------------------------

} // namespace helper

} // namespace sight::module::ui::viz
