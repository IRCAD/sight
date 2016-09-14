/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiVisuOgre/helper/ParameterEditor.hpp"

#include <fwComEd/helper/ArrayGetter.hpp>

#include "fwData/Array.hpp"
#include "fwData/Color.hpp"
#include "fwData/Float.hpp"
#include "fwData/Integer.hpp"

namespace uiVisuOgre
{

namespace helper
{

//-----------------------------------------------------------------------------

fwServices::IService::ConfigType ParameterEditor::createConfig(const visuOgreAdaptor::IParameter::csptr& _adaptor,
                                                               const ::fwServices::IService::csptr& _paramSrv,
                                                               ::fwServices::helper::SigSlotConnection& _connections)
{
    ::fwServices::IService::ConfigType paramConfig;

    /// Getting associated object infos
    const ::fwData::Object::csptr shaderObj = ::fwServices::IService::constCast(_adaptor)->getObject();
    const auto& objType                     = shaderObj->getClassname();

    if(objType == "::fwData::Boolean")
    {
        _connections.connect(_paramSrv, "boolChanged", _adaptor, "setBoolParameter");

        paramConfig.add("<xmlattr>.type", "bool");
        paramConfig.add("<xmlattr>.name", _adaptor->getParamName());
        paramConfig.add("<xmlattr>.key", _adaptor->getParamName());
        paramConfig.add("<xmlattr>.defaultValue", false);
    }
    else if(objType == "::fwData::Color")
    {
        _connections.connect(_paramSrv, "colorChanged", _adaptor, "setColorParameter");

        auto colorValue = ::fwData::Color::dynamicConstCast(shaderObj);

        int r = static_cast< unsigned char >(colorValue->red() * 255);
        int g = static_cast< unsigned char >(colorValue->green() * 255);
        int b = static_cast< unsigned char >(colorValue->blue() * 255);
        int a = static_cast< unsigned char >(colorValue->alpha() * 255);

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
    else if(objType == "::fwData::Float")
    {
        _connections.connect(_paramSrv, "doubleChanged", _adaptor, "setDoubleParameter");

        auto floatValue           = ::fwData::Float::dynamicConstCast(shaderObj);
        const double defaultValue = floatValue->value();
        const double max          = (defaultValue != 0.) ? defaultValue * 20. : 1.;
        const double min          = (defaultValue != 0.) ? max - defaultValue * 20. : 1.;

        paramConfig.add("<xmlattr>.type", "double");
        paramConfig.add("<xmlattr>.name", _adaptor->getParamName());
        paramConfig.add("<xmlattr>.key", _adaptor->getParamName());
        paramConfig.add("<xmlattr>.defaultValue", std::to_string(defaultValue));
        paramConfig.add("<xmlattr>.min", min);
        paramConfig.add("<xmlattr>.max", max);
    }
    else if(objType == "::fwData::Integer")
    {
        _connections.connect(_paramSrv, "intChanged", _adaptor, "setIntParameter");

        auto intValue          = ::fwData::Integer::dynamicConstCast(shaderObj);
        const int defaultValue = intValue->value();
        const int max          = (defaultValue != 0) ? defaultValue * 20 : 1;
        const int min          = (defaultValue != 0) ? max - defaultValue * 20 : 1;

        paramConfig.add("<xmlattr>.type", "int");
        paramConfig.add("<xmlattr>.name", _adaptor->getParamName());
        paramConfig.add("<xmlattr>.key", _adaptor->getParamName());
        paramConfig.add("<xmlattr>.defaultValue", std::to_string(defaultValue));
        paramConfig.add("<xmlattr>.min", min);
        paramConfig.add("<xmlattr>.max", max);
    }
    else if(objType == "::fwData::Array")
    {
        auto arrayObject = ::fwData::Array::dynamicConstCast(shaderObj);
        if(arrayObject->getNumberOfComponents() <= 3)
        {
            auto numComponents  = arrayObject->getNumberOfComponents();
            std::string strSize = std::to_string(numComponents);

            if( arrayObject->getType() == ::fwTools::Type::s_FLOAT ||
                arrayObject->getType() == ::fwTools::Type::s_DOUBLE)
            {
                _connections.connect(_paramSrv, "double" + strSize + "Changed",
                                     _adaptor, "setDouble" + strSize + "Parameter");

                // We can't give a default value for each component to SParameters :/
                // For now fill it with the first one
                ::fwComEd::helper::ArrayGetter arrayHelper(arrayObject);

                double defaultValue;
                if(arrayObject->getType() == ::fwTools::Type::s_FLOAT)
                {
                    defaultValue = arrayHelper.getItem< float >({0})[0];
                }
                else
                {
                    defaultValue = arrayHelper.getItem< double >({0})[0];
                }
                const double max = (defaultValue != 0.) ? defaultValue * 20. : 1.;
                const double min = (defaultValue != 0.) ? max - defaultValue * 20. : 1.;

                paramConfig.add("<xmlattr>.type", "double" + strSize);
                paramConfig.add("<xmlattr>.name", _adaptor->getParamName());
                paramConfig.add("<xmlattr>.key", _adaptor->getParamName());
                paramConfig.add("<xmlattr>.defaultValue", std::to_string(defaultValue));
                paramConfig.add("<xmlattr>.min", min);
                paramConfig.add("<xmlattr>.max", max);
            }
            else if( arrayObject->getType() == ::fwTools::Type::s_INT32)
            {
                _connections.connect(_paramSrv, "int" + strSize + "Changed",
                                     _adaptor, "setInt" + strSize + "Parameter");

                ::fwComEd::helper::ArrayGetter arrayHelper(arrayObject);
                const int defaultValue = arrayHelper.getItem< int >({0})[0];
                const int max          = (defaultValue != 0) ? defaultValue * 20 : 1;
                const int min          = (defaultValue != 0) ? max - defaultValue * 20 : 1;

                paramConfig.add("<xmlattr>.type", "int" + strSize);
                paramConfig.add("<xmlattr>.name", _adaptor->getParamName());
                paramConfig.add("<xmlattr>.key", _adaptor->getParamName());
                paramConfig.add("<xmlattr>.defaultValue", std::to_string(defaultValue));
                paramConfig.add("<xmlattr>.min", min);
                paramConfig.add("<xmlattr>.max", max);
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
    else
    {
        OSLM_ERROR("No editor found for the object of type " << objType);
    }

    return paramConfig;
}

//-----------------------------------------------------------------------------

} // namespace helper

} // namespace uiVisuOgre
