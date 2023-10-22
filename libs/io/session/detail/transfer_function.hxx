/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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
#include "io/session/helper.hpp"
#include "io/session/macros.hpp"

#include <data/transfer_function.hpp>

namespace sight::io::session::detail::transfer_function
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

inline static void serialize_transfer_function_piece(
    boost::property_tree::ptree& _tree,
    const data::transfer_function_piece& _transfer_function
)
{
    _tree.put(s_InterpolationMode, static_cast<int>(_transfer_function.interpolationMode()));
    _tree.put(s_IsClamped, _transfer_function.clamped());
    _tree.put(s_Level, _transfer_function.level());
    _tree.put(s_Window, _transfer_function.window());

    // Transfer function data
    boost::property_tree::ptree data_tree;

    std::size_t index = 0;
    for(const auto& value : _transfer_function)
    {
        boost::property_tree::ptree value_tree;
        value_tree.put(s_Value, std::to_string(value.first));
        value_tree.put(s_Red, value.second.r);
        value_tree.put(s_Green, value.second.g);
        value_tree.put(s_Blue, value.second.b);
        value_tree.put(s_Alpha, value.second.a);
        data_tree.add_child(s_Value + std::to_string(index++), value_tree);
    }

    _tree.add_child(s_TFData, data_tree);
}

//------------------------------------------------------------------------------

inline static void deserialize_transfer_function_piece(
    const boost::property_tree::ptree& _tree,
    data::transfer_function_piece& _transfer_function
)
{
    // Deserialize attributes
    _transfer_function.setLevel(_tree.get<double>(s_Level));
    _transfer_function.setWindow(_tree.get<double>(s_Window));
    _transfer_function.setInterpolationMode(
        static_cast<data::transfer_function::InterpolationMode>(_tree.get<int>(s_InterpolationMode))
    );
    _transfer_function.setClamped(_tree.get<bool>(s_IsClamped));

    // Transfer function data
    for(const auto& value : _tree.get_child(s_TFData))
    {
        const auto& color_tree = value.second;
        data::transfer_function::color_t color(
            color_tree.get<double>(s_Red),
            color_tree.get<double>(s_Green),
            color_tree.get<double>(s_Blue),
            color_tree.get<double>(s_Alpha)
        );

        _transfer_function.insert({color_tree.get<double>(s_Value), color});
    }
}

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& /*unused*/,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto transfer_function = helper::safe_cast<data::transfer_function>(_object);

    helper::write_version<data::transfer_function>(_tree, 1);

    // Serialize attributes
    helper::write_string(_tree, s_Name, transfer_function->name());

    // Background color
    const auto& background_color = transfer_function->backgroundColor();
    boost::property_tree::ptree background_color_tree;
    background_color_tree.put(s_Red, background_color.r);
    background_color_tree.put(s_Green, background_color.g);
    background_color_tree.put(s_Blue, background_color.b);
    background_color_tree.put(s_Alpha, background_color.a);
    _tree.add_child(s_BackgroundColor, background_color_tree);

    _tree.put(s_Level, transfer_function->level());
    _tree.put(s_Window, transfer_function->window());

    boost::property_tree::ptree pieces_tree;
    std::size_t index = 0;
    for(const auto& piece : transfer_function->pieces())
    {
        boost::property_tree::ptree piece_tree;
        serialize_transfer_function_piece(piece_tree, *piece);
        pieces_tree.add_child(s_Pieces + std::to_string(index++), piece_tree);
    }

    _tree.add_child(s_Pieces, pieces_tree);
}

//------------------------------------------------------------------------------

inline static data::transfer_function::sptr read(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& /*unused*/,
    data::object::sptr _object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto transfer_function = helper::cast_or_create<data::transfer_function>(_object);

    // Background color
    const auto background_color_tree = _tree.get_child(s_BackgroundColor);
    data::transfer_function::color_t background_color(
        background_color_tree.get<double>(s_Red),
        background_color_tree.get<double>(s_Green),
        background_color_tree.get<double>(s_Blue),
        background_color_tree.get<double>(s_Alpha)
    );
    transfer_function->setBackgroundColor(background_color);
    transfer_function->setName(helper::read_string(_tree, s_Name));

    const int version = helper::read_version<data::transfer_function>(_tree, 0, 1);

    if(version == -1)
    {
        auto& pieces = transfer_function->pieces();
        pieces.push_back(std::make_shared<data::transfer_function_piece>());

        deserialize_transfer_function_piece(_tree, *pieces.back());

        transfer_function->fitWindow();
    }
    else
    {
        transfer_function->setLevel(_tree.get<double>(s_Level));
        transfer_function->setWindow(_tree.get<double>(s_Window));

        auto& pieces = transfer_function->pieces();
        pieces.clear();

        for(const auto& piece_tree : _tree.get_child(s_Pieces))
        {
            const auto new_piece = std::make_shared<data::transfer_function_piece>();
            pieces.push_back(new_piece);

            deserialize_transfer_function_piece(piece_tree.second, *new_piece);
        }
    }

    return transfer_function;
}

SIGHT_REGISTER_SERIALIZER(data::transfer_function, write, read);

} // namespace sight::io::session::detail::transfer_function
