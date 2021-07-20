/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#pragma once

#include "io/session/config.hpp"
#include "io/session/detail/Helper.hpp"

#include <data/TransferFunction.hpp>

namespace sight::io::session
{

namespace detail::TransferFunction
{

constexpr static auto s_Level {"Level"};
constexpr static auto s_Window {"Window"};
constexpr static auto s_Name {"Name"};
constexpr static auto s_Red {"Red"};
constexpr static auto s_Green {"Green"};
constexpr static auto s_Blue {"Blue"};
constexpr static auto s_Alpha {"Alpha"};
constexpr static auto s_BackgroundColor {"BackgroundColor"};
constexpr static auto s_InterpolationMode {"InterpolationMode"};
constexpr static auto s_IsClamped {"IsClamped"};
constexpr static auto s_TFData {"TFData"};
constexpr static auto s_Value {"Value"};
constexpr static auto s_Color {"Color"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter&,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>&,
    const core::crypto::secure_string& password = ""
)
{
    const auto transferFunction = Helper::safeCast<data::TransferFunction>(object);

    // Serialize attributes
    tree.put(s_Level, transferFunction->getLevel());
    tree.put(s_Window, transferFunction->getWindow());
    Helper::writeString(tree, s_Name, transferFunction->getName(), password);
    tree.put(s_InterpolationMode, transferFunction->getInterpolationMode());
    tree.put(s_IsClamped, transferFunction->getIsClamped());

    // Background color
    const auto& backgroundColor = transferFunction->getBackgroundColor();
    boost::property_tree::ptree backgroundColorTree;
    backgroundColorTree.put(s_Red, backgroundColor.r);
    backgroundColorTree.put(s_Green, backgroundColor.g);
    backgroundColorTree.put(s_Blue, backgroundColor.b);
    backgroundColorTree.put(s_Alpha, backgroundColor.a);
    tree.add_child(s_BackgroundColor, backgroundColorTree);

    // Transfer function data
    boost::property_tree::ptree dataTree;

    std::size_t index = 0;
    for(const auto& value : transferFunction->getTFData())
    {
        boost::property_tree::ptree valueTree;
        valueTree.put(s_Value, std::to_string(value.first));
        valueTree.put(s_Red, value.second.r);
        valueTree.put(s_Green, value.second.g);
        valueTree.put(s_Blue, value.second.b);
        valueTree.put(s_Alpha, value.second.a);
        dataTree.add_child(s_Value + std::to_string(index++), valueTree);
    }

    tree.add_child(s_TFData, dataTree);
}

//------------------------------------------------------------------------------

inline static data::TransferFunction::sptr deserialize(
    zip::ArchiveReader&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>&,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto transferFunction = Helper::safeCast<data::TransferFunction>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::TransferFunction>(tree, 0, 1);

    // Deserialize attributes
    transferFunction->setLevel(tree.get<double>(s_Level));
    transferFunction->setWindow(tree.get<double>(s_Window));
    transferFunction->setName(Helper::readString(tree, s_Name, password));
    transferFunction->setInterpolationMode(
        static_cast<data::TransferFunction::InterpolationMode>(tree.get<int>(s_InterpolationMode))
    );
    transferFunction->setIsClamped(tree.get<bool>(s_IsClamped));

    // Background color
    const auto backgroundColorTree = tree.get_child(s_BackgroundColor);
    data::TransferFunction::TFColor backgroundColor(
        backgroundColorTree.get<double>(s_Red),
        backgroundColorTree.get<double>(s_Green),
        backgroundColorTree.get<double>(s_Blue),
        backgroundColorTree.get<double>(s_Alpha)
    );
    transferFunction->setBackgroundColor(backgroundColor);

    // Transfer function data
    for(const auto& value : tree.get_child(s_TFData))
    {
        const auto& colorTree = value.second;
        data::TransferFunction::TFColor color(
            colorTree.get<double>(s_Red),
            colorTree.get<double>(s_Green),
            colorTree.get<double>(s_Blue),
            colorTree.get<double>(s_Alpha)
        );

        transferFunction->addTFColor(colorTree.get<double>(s_Value), color);
    }

    return transferFunction;
}

} // namespace detail::TransferFunction

} // namespace sight::io
