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

constexpr static auto LEVEL {"Level"};
constexpr static auto WINDOW {"Window"};
constexpr static auto NAME {"Name"};
constexpr static auto RED {"Red"};
constexpr static auto GREEN {"Green"};
constexpr static auto BLUE {"Blue"};
constexpr static auto ALPHA {"Alpha"};
constexpr static auto BACKGROUND_COLOR {"BackgroundColor"};
constexpr static auto INTERPOLATION_MODE {"InterpolationMode"};
constexpr static auto IS_CLAMPED {"IsClamped"};
constexpr static auto TF_DATA {"TFData"};
constexpr static auto VALUE {"Value"};
constexpr static auto COLOR {"Color"};
constexpr static auto PIECES {"Pieces"};

//------------------------------------------------------------------------------

inline static void serialize_transfer_function_piece(
    boost::property_tree::ptree& _tree,
    const data::transfer_function_piece& _transfer_function
)
{
    _tree.put(INTERPOLATION_MODE, static_cast<int>(_transfer_function.get_interpolation_mode()));
    _tree.put(IS_CLAMPED, _transfer_function.clamped());
    _tree.put(LEVEL, _transfer_function.level());
    _tree.put(WINDOW, _transfer_function.window());

    // Transfer function data
    boost::property_tree::ptree data_tree;

    std::size_t index = 0;
    for(const auto& value : _transfer_function)
    {
        boost::property_tree::ptree value_tree;
        value_tree.put(VALUE, std::to_string(value.first));
        value_tree.put(RED, value.second.r);
        value_tree.put(GREEN, value.second.g);
        value_tree.put(BLUE, value.second.b);
        value_tree.put(ALPHA, value.second.a);
        data_tree.add_child(VALUE + std::to_string(index++), value_tree);
    }

    _tree.add_child(TF_DATA, data_tree);
}

//------------------------------------------------------------------------------

inline static void deserialize_transfer_function_piece(
    const boost::property_tree::ptree& _tree,
    data::transfer_function_piece& _transfer_function
)
{
    // Deserialize attributes
    _transfer_function.set_level(_tree.get<double>(LEVEL));
    _transfer_function.set_window(_tree.get<double>(WINDOW));
    _transfer_function.set_interpolation_mode(
        static_cast<data::transfer_function::interpolation_mode>(_tree.get<int>(INTERPOLATION_MODE))
    );
    _transfer_function.set_clamped(_tree.get<bool>(IS_CLAMPED));

    // Transfer function data
    for(const auto& value : _tree.get_child(TF_DATA))
    {
        const auto& color_tree = value.second;
        data::transfer_function::color_t color(
            color_tree.get<double>(RED),
            color_tree.get<double>(GREEN),
            color_tree.get<double>(BLUE),
            color_tree.get<double>(ALPHA)
        );

        _transfer_function.insert({color_tree.get<double>(VALUE), color});
    }
}

//------------------------------------------------------------------------------

inline static void write(
    zip::archive_writer& /*unused*/,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& /*unused*/,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto transfer_function = helper::safe_cast<data::transfer_function>(_object);

    helper::write_version<data::transfer_function>(_tree, 1);

    // Serialize attributes
    helper::write_string(_tree, NAME, transfer_function->name());

    // Background color
    const auto& background_color = transfer_function->background_color();
    boost::property_tree::ptree background_color_tree;
    background_color_tree.put(RED, background_color.r);
    background_color_tree.put(GREEN, background_color.g);
    background_color_tree.put(BLUE, background_color.b);
    background_color_tree.put(ALPHA, background_color.a);
    _tree.add_child(BACKGROUND_COLOR, background_color_tree);

    _tree.put(LEVEL, transfer_function->level());
    _tree.put(WINDOW, transfer_function->window());

    boost::property_tree::ptree pieces_tree;
    std::size_t index = 0;
    for(const auto& piece : transfer_function->pieces())
    {
        boost::property_tree::ptree piece_tree;
        serialize_transfer_function_piece(piece_tree, *piece);
        pieces_tree.add_child(PIECES + std::to_string(index++), piece_tree);
    }

    _tree.add_child(PIECES, pieces_tree);
}

//------------------------------------------------------------------------------

inline static data::transfer_function::sptr read(
    zip::archive_reader& /*unused*/,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& /*unused*/,
    data::object::sptr _object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto transfer_function = helper::cast_or_create<data::transfer_function>(_object);

    // Background color
    const auto background_color_tree = _tree.get_child(BACKGROUND_COLOR);
    data::transfer_function::color_t background_color(
        background_color_tree.get<double>(RED),
        background_color_tree.get<double>(GREEN),
        background_color_tree.get<double>(BLUE),
        background_color_tree.get<double>(ALPHA)
    );
    transfer_function->set_background_color(background_color);
    transfer_function->set_name(helper::read_string(_tree, NAME));

    const int version = helper::read_version<data::transfer_function>(_tree, 0, 1);

    if(version == -1)
    {
        auto& pieces = transfer_function->pieces();
        pieces.push_back(std::make_shared<data::transfer_function_piece>());

        deserialize_transfer_function_piece(_tree, *pieces.back());

        transfer_function->fit_window();
    }
    else
    {
        transfer_function->set_level(_tree.get<double>(LEVEL));
        transfer_function->set_window(_tree.get<double>(WINDOW));

        auto& pieces = transfer_function->pieces();
        pieces.clear();

        for(const auto& piece_tree : _tree.get_child(PIECES))
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
