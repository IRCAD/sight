/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

// cspell:ignore NOLINTNEXTLINE NOLINT

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#ifdef DEBUG
static std::stringstream spiritDebugStream;
  #define BOOST_SPIRIT_DEBUG_OUT spiritDebugStream // NOLINT(cppcoreguidelines-macro-usage): needed by Boost
  #define BOOST_SPIRIT_DEBUG                       // NOLINT(cppcoreguidelines-macro-usage): needed by Boost
#endif

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/detail/case_conv.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_eol.hpp>
#include <boost/phoenix.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/phoenix/bind.hpp>
#include <boost/phoenix/statement.hpp>
#include <boost/phoenix/stl.hpp>

#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/fusion/container.hpp>
#include <boost/fusion/container/vector/vector.hpp>
#include <boost/fusion/include/vector.hpp>

#include <core/exceptionmacros.hpp>

#include <data/color.hpp>
#include <data/structure_traits_dictionary.hpp>
#include <data/structure_traits.hpp>
#include <data/structure_traits_helper.hpp>

#include <core/runtime/path.hpp>

#include "io/__/reader/dictionary_reader.hpp"
#include "io/__/reader/registry/macros.hpp"

SIGHT_REGISTER_IO_READER(sight::io::reader::dictionary_reader);

namespace sight::io
{

struct line
{
    std::string type;
    double red {};
    double green {};
    double blue {};
    double alpha {};
    std::string category;
    std::string organ_class;
    std::string attachment;
    std::string native_exp;
    std::string native_exp_geo;
    std::string anatomic_region;
    std::string property_category;
    std::string property_type;
};

} // namespace sight::io

BOOST_FUSION_ADAPT_STRUCT(
    sight::io::line,
    (std::string, type)
        (double, red)
        (double, green)
        (double, blue)
        (double, alpha)
        (std::string, category)
        (std::string, organ_class)
        (std::string, attachment)
        (std::string, native_exp)
        (std::string, native_exp_geo)
        (std::string, anatomic_region)
        (std::string, property_category)
        (std::string, property_type)
)

//------------------------------------------------------------------------------

inline std::string trim(std::string& _s)
{
    return boost::algorithm::trim_copy(_s);
}

//------------------------------------------------------------------------------

/// Reformat string in the following way :first letter is uppercase and the rest is lowercase).
std::string reformat_string(std::string& _expr)
{
    std::string trim_str = boost::algorithm::trim_copy(_expr);
    std::string result   = boost::algorithm::to_upper_copy(trim_str.substr(0, 1))
                           + boost::algorithm::to_lower_copy(trim_str.substr(1));
    return result;
}

//------------------------------------------------------------------------------
/// Return the list of availabe value for the key of the map m.

template<typename map_t>
std::string get_values(const map_t& _m)
{
    std::stringstream str;
    using const_iterator = typename map_t::const_iterator;
    const_iterator iter = _m.begin();
    str << "( " << iter->first;
    for( ; iter != _m.end() ; ++iter)
    {
        str << ", " << iter->first;
    }

    str << ") ";
    return str.str();
}

//------------------------------------------------------------------------------

namespace sight::io
{

namespace qi    = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

template<typename Iterator>
struct line_parser : qi::grammar<Iterator,
                                 std::vector<line>()>
{
    line_parser() :
        line_parser::base_type(lines)
    {
        using qi::int_;
        using qi::lit;
        using qi::_1;
        using qi::double_;
        using qi::blank;
        using qi::alnum;
        using qi::omit;
        using ascii::char_;
        using boost::spirit::qi::eol;
        using boost::spirit::qi::eoi;
        using boost::phoenix::construct;
        namespace phx = boost::phoenix;

        error.clear();

        // NOLINTNEXTLINE(cppcoreguidelines-prefer-member-initializer)
        lines   = +(line[phx::push_back(qi::_val, qi::_1)] | comment) >> eoi;
        comment = *blank >> lit('#') >> *(char_ - eol) >> +qi::eol;

        line = trimmed_string >> lit(';')
               >> omit[*blank] >> lit('(')
               >> dbl >> lit(',')
               >> dbl >> lit(',')
               >> dbl >> lit(',')
               >> dbl
               >> lit(')') >> omit[*blank]
               >> lit(';')
               >> string_set >> lit(';')
               >> trimmed_string >> lit(';')
               >> trimmed_string >> lit(';')
               >> trimmed_string_expr >> lit(';')
               >> trimmed_string_expr >> lit(';')
               >> trimmed_string >> lit(';')
               >> trimmed_string >> lit(';')
               >> trimmed_string
               >> +qi::eol;

        trimmed_string = str[qi::_val = phx::bind(trim, qi::_1)];
        str            = *((alnum | char_("_"))[qi::_val += qi::_1] | blank[qi::_val += " "]);

        trimmed_string_expr = string_expr[qi::_val = phx::bind(trim, qi::_1)];
        string_expr         = *((alnum | char_("()_,.+-"))[qi::_val += qi::_1] | blank[qi::_val += " "]);

        string_set        = string_with_comma[qi::_val = phx::bind(trim, qi::_1)];
        string_with_comma = *((alnum | char_(",_"))[qi::_val += qi::_1] | blank[qi::_val += " "]);

        dbl = omit[*blank] >> double_ >> omit[*blank];

    #ifdef BOOST_SPIRIT_DEBUG
        BOOST_SPIRIT_DEBUG_NODE(comment);
        BOOST_SPIRIT_DEBUG_NODE(trimmed_string);
        BOOST_SPIRIT_DEBUG_NODE(trimmed_string_expr);
        BOOST_SPIRIT_DEBUG_NODE(string_set);
        BOOST_SPIRIT_DEBUG_NODE(dbl);
        BOOST_SPIRIT_DEBUG_NODE(line);
        BOOST_SPIRIT_DEBUG_NODE(lines);
        SIGHT_DEBUG(spiritDebugStream.str());
        spiritDebugStream.str(std::string());
    #endif

        qi::on_error<qi::fail>
        (
            line
            ,
            phx::ref((std::ostream&) error)
            << phx::val("Error! Expecting ")
            << qi::_4 // what failed?
            << phx::val(" here: \"")
            << phx::construct<std::string>(qi::_3, qi::_2) // iterators to error-pos, end
            << phx::val("\"")
            << std::endl
        );
    }

    qi::rule<Iterator, double()> dbl;
    qi::rule<Iterator, std::string()> str;
    qi::rule<Iterator, std::string()> string_expr;
    qi::rule<Iterator, std::string()> string_with_comma;

    qi::rule<Iterator, std::string()> comment;
    qi::rule<Iterator, std::string()> trimmed_string_expr;
    qi::rule<Iterator, std::string()> trimmed_string;
    qi::rule<Iterator, std::string()> string_set;

    qi::rule<Iterator, io::line()> line;
    qi::rule<Iterator, std::vector<io::line>()> lines;
    std::stringstream error;
};

namespace reader
{

//------------------------------------------------------------------------------

std::pair<bool, std::string> parse(std::string& _buf, std::vector<io::line>& _lines)
{
    using boost::spirit::ascii::space;
    using boost::spirit::ascii::blank;
    using boost::spirit::qi::eol;

    using boost::spirit::qi::phrase_parse;

    using iterator_type = std::string::const_iterator;
    using line_parser   = io::line_parser<iterator_type>;

    iterator_type iter = _buf.begin();
    iterator_type end  = _buf.end();

    line_parser grammar; // Our grammar

    bool result     = phrase_parse(iter, end, grammar, space - blank - eol, _lines);
    bool success    = result && (iter == end);
    std::string msg = grammar.error.str();
    return std::make_pair(success, msg);
}

//------------------------------------------------------------------------------

void dictionary_reader::read()
{
    std::filesystem::path path = this->get_file();

    SIGHT_INFO("[dictionary_reader::read] dictionary file: " << path.string());
    SIGHT_ASSERT("Empty path for dictionary file", !path.empty());

    // Reading of the file
    std::string buf;
    std::ifstream file;
    file.open(path.string().c_str(), std::ios::binary);

    std::string error_open = "Unable to open " + path.string();
    SIGHT_THROW_IF(error_open, !file.is_open());

    file.seekg(0, std::ios::end);
    const auto length = file.tellg();
    file.seekg(0, std::ios::beg);

    buf.resize(static_cast<std::size_t>(length));
    char* buffer = buf.data();

    file.read(buffer, static_cast<std::streamsize>(length));
    file.close();

    std::vector<io::line> dico_lines;
    std::pair<bool, std::string> result = parse(buf, dico_lines);

    std::string error = "Unable to parse " + path.string() + " : Bad file format.Error : " + result.second;
    SIGHT_THROW_IF(error, !result.first);

    // File the dictionary Structure
    data::structure_traits_dictionary::sptr struct_dico = get_concrete_object();

    for(io::line line : dico_lines)
    {
        data::structure_traits::sptr new_organ = std::make_shared<data::structure_traits>();
        new_organ->set_type(line.type);

        std::string class_reformated = reformat_string(
            line.organ_class
        );
        data::structure_traits_helper::class_translator_t::right_const_iterator str_class_iter =
            data::structure_traits_helper::CLASSTRANSLATOR.right.find(class_reformated);
        std::string available_values = get_values(data::structure_traits_helper::CLASSTRANSLATOR.right);
        error =
            std::string("Organ class ") + class_reformated + " isn't available. Authorized type are "
            + available_values;
        SIGHT_THROW_IF(error, !(str_class_iter != data::structure_traits_helper::CLASSTRANSLATOR.right.end()));
        new_organ->set_class(str_class_iter->second);

        new_organ->set_color(
            std::make_shared<data::color>(
                static_cast<float>(line.red) / 255.0F,
                static_cast<float>(line.green) / 255.0F,
                static_cast<float>(line.blue) / 255.0F,
                static_cast<float>(line.alpha) / 100.0F
            )
        );
        std::vector<std::string> categorylist;
        boost::algorithm::split(categorylist, line.category, boost::algorithm::is_any_of(","));
        data::structure_traits::category_container_t categories;
        for(std::string category : categorylist)
        {
            std::string cat_reformated = reformat_string(
                category
            );
            data::structure_traits_helper::category_translator_t::right_const_iterator str_category_iter =
                data::structure_traits_helper::CATEGORYTRANSLATOR.right.find(cat_reformated);
            available_values = get_values(
                data::structure_traits_helper::CATEGORYTRANSLATOR.right
            );
            error =
                std::string("Category ") + cat_reformated + " isn't available. Authorized type are "
                + available_values;
            SIGHT_THROW_IF(
                error,
                !(str_category_iter != data::structure_traits_helper::CATEGORYTRANSLATOR.right.end())
            );
            categories.push_back(str_category_iter->second);
        }

        new_organ->set_categories(categories);
        new_organ->set_attachment_type(line.attachment);
        new_organ->set_native_exp(line.native_exp);
        new_organ->set_native_geometric_exp(line.native_exp_geo);
        new_organ->set_anatomic_region(line.anatomic_region);
        new_organ->set_property_category(line.property_category);
        new_organ->set_property_type(line.property_type);
        struct_dico->add_structure(new_organ);
    }
}

//------------------------------------------------------------------------------

std::string dictionary_reader::extension() const
{
    return ".dic";
}

//------------------------------------------------------------------------------

std::filesystem::path dictionary_reader::get_default_dictionary_path()
{
    return core::runtime::get_library_resource_file_path("io/OrganDictionary.dic");
}

//------------------------------------------------------------------------------

} // namespace reader

} // namespace sight::io
