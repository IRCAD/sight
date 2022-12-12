/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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
#include "io/session/Helper.hpp"
#include "io/session/macros.hpp"

#include <data/TransferFunction.hpp>

namespace sight::io::session::detail::TransferFunction
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
constexpr static auto s_Pieces {"Pieces"};

//------------------------------------------------------------------------------

inline static void serializeTransferFunctionPiece(
    boost::property_tree::ptree& tree,
    const data::TransferFunctionPiece& transferFunction
)
{
    tree.put(s_InterpolationMode, static_cast<int>(transferFunction.interpolationMode()));
    tree.put(s_IsClamped, transferFunction.clamped());
    tree.put(s_Level, transferFunction.level());
    tree.put(s_Window, transferFunction.window());

    // Transfer function data
    boost::property_tree::ptree dataTree;

    std::size_t index = 0;
    for(const auto& value : transferFunction)
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

inline static void deserializeTransferFunctionPiece(
    const boost::property_tree::ptree& tree,
    data::TransferFunctionPiece& transferFunction
)
{
    // Deserialize attributes
    transferFunction.setLevel(tree.get<double>(s_Level));
    transferFunction.setWindow(tree.get<double>(s_Window));
    transferFunction.setInterpolationMode(
        static_cast<data::TransferFunction::InterpolationMode>(tree.get<int>(s_InterpolationMode))
    );
    transferFunction.setClamped(tree.get<bool>(s_IsClamped));

    // Transfer function data
    for(const auto& value : tree.get_child(s_TFData))
    {
        const auto& colorTree = value.second;
        data::TransferFunction::color_t color(
            colorTree.get<double>(s_Red),
            colorTree.get<double>(s_Green),
            colorTree.get<double>(s_Blue),
            colorTree.get<double>(s_Alpha)
        );

        transferFunction.insert({colorTree.get<double>(s_Value), color});
    }
}

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& /*unused*/,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto transferFunction = Helper::safe_cast<data::TransferFunction>(object);

    Helper::writeVersion<data::TransferFunction>(tree, 1);

    // Serialize attributes
    Helper::writeString(tree, s_Name, transferFunction->name());

    // Background color
    const auto& backgroundColor = transferFunction->backgroundColor();
    boost::property_tree::ptree backgroundColorTree;
    backgroundColorTree.put(s_Red, backgroundColor.r);
    backgroundColorTree.put(s_Green, backgroundColor.g);
    backgroundColorTree.put(s_Blue, backgroundColor.b);
    backgroundColorTree.put(s_Alpha, backgroundColor.a);
    tree.add_child(s_BackgroundColor, backgroundColorTree);

    tree.put(s_Level, transferFunction->level());
    tree.put(s_Window, transferFunction->window());

    boost::property_tree::ptree piecesTree;
    std::size_t index = 0;
    for(const auto& piece : transferFunction->pieces())
    {
        boost::property_tree::ptree pieceTree;
        serializeTransferFunctionPiece(pieceTree, *piece);
        piecesTree.add_child(s_Pieces + std::to_string(index++), pieceTree);
    }

    tree.add_child(s_Pieces, piecesTree);
}

//------------------------------------------------------------------------------

inline static data::TransferFunction::sptr read(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& /*unused*/,
    data::Object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto transferFunction = Helper::cast_or_create<data::TransferFunction>(object);

    // Background color
    const auto backgroundColorTree = tree.get_child(s_BackgroundColor);
    data::TransferFunction::color_t backgroundColor(
        backgroundColorTree.get<double>(s_Red),
        backgroundColorTree.get<double>(s_Green),
        backgroundColorTree.get<double>(s_Blue),
        backgroundColorTree.get<double>(s_Alpha)
    );
    transferFunction->setBackgroundColor(backgroundColor);
    transferFunction->setName(Helper::readString(tree, s_Name));

    const int version = Helper::readVersion<data::TransferFunction>(tree, 0, 1);

    if(version == -1)
    {
        auto& pieces = transferFunction->pieces();
        pieces.push_back(data::TransferFunctionPiece::New());

        deserializeTransferFunctionPiece(tree, *pieces.back());

        transferFunction->fitWindow();
    }
    else
    {
        transferFunction->setLevel(tree.get<double>(s_Level));
        transferFunction->setWindow(tree.get<double>(s_Window));

        auto& pieces = transferFunction->pieces();
        pieces.clear();

        for(const auto& pieceTree : tree.get_child(s_Pieces))
        {
            const auto newPiece = data::TransferFunctionPiece::New();
            pieces.push_back(newPiece);

            deserializeTransferFunctionPiece(pieceTree.second, *newPiece);
        }
    }

    return transferFunction;
}

SIGHT_REGISTER_SERIALIZER(data::TransferFunction, write, read);

} // namespace sight::io::session::detail::TransferFunction
