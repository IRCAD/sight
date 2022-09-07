/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <data/Color.hpp>
#include <data/StructureTraitsDictionary.hpp>
#include <data/StructureTraits.hpp>
#include <data/StructureTraitsHelper.hpp>

#include <core/runtime/operations.hpp>

#include "io/base/reader/DictionaryReader.hpp"
#include "io/base/reader/registry/macros.hpp"

SIGHT_REGISTER_IO_READER(sight::io::base::reader::DictionaryReader);

namespace sight::io::base
{

struct Line
{
    std::string type;
    double red {};
    double green {};
    double blue {};
    double alpha {};
    std::string category;
    std::string organClass;
    std::string attachment;
    std::string nativeExp;
    std::string nativeExpGeo;
    std::string anatomicRegion;
    std::string propertyCategory;
    std::string propertyType;
};

} // namespace sight::io::base

BOOST_FUSION_ADAPT_STRUCT(
    sight::io::base::Line,
    (std::string, type)
        (double, red)
        (double, green)
        (double, blue)
        (double, alpha)
        (std::string, category)
        (std::string, organClass)
        (std::string, attachment)
        (std::string, nativeExp)
        (std::string, nativeExpGeo)
        (std::string, anatomicRegion)
        (std::string, propertyCategory)
        (std::string, propertyType)
)

//------------------------------------------------------------------------------

inline std::string trim(std::string& s)
{
    return boost::algorithm::trim_copy(s);
}

//------------------------------------------------------------------------------

/// Reformat string in the following way :first letter is uppercase and the rest is lowercase).
std::string reformatString(std::string& expr)
{
    std::string trimStr = boost::algorithm::trim_copy(expr);
    std::string result  = boost::algorithm::to_upper_copy(trimStr.substr(0, 1))
                          + boost::algorithm::to_lower_copy(trimStr.substr(1));
    return result;
}

//------------------------------------------------------------------------------
/// Return the list of availabe value for the key of the map m.

template<typename MapType>
std::string getValues(const MapType& m)
{
    std::stringstream str;
    using const_iterator = typename MapType::const_iterator;
    const_iterator iter = m.begin();
    str << "( " << iter->first;
    for( ; iter != m.end() ; ++iter)
    {
        str << ", " << iter->first;
    }

    str << ") ";
    return str.str();
}

//------------------------------------------------------------------------------

namespace sight::io::base
{

namespace qi    = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

template<typename Iterator>
struct LineParser : qi::grammar<Iterator,
                                std::vector<Line>()>
{
    LineParser() :
        LineParser::base_type(lines)
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

        line = trimmedString >> lit(';')
               >> omit[*blank] >> lit('(')
               >> dbl >> lit(',')
               >> dbl >> lit(',')
               >> dbl >> lit(',')
               >> dbl
               >> lit(')') >> omit[*blank]
               >> lit(';')
               >> stringSet >> lit(';')
               >> trimmedString >> lit(';')
               >> trimmedString >> lit(';')
               >> trimmedStringExpr >> lit(';')
               >> trimmedStringExpr >> lit(';')
               >> trimmedString >> lit(';')
               >> trimmedString >> lit(';')
               >> trimmedString
               >> +qi::eol;

        trimmedString = str[qi::_val = phx::bind(trim, qi::_1)];
        str           = *((alnum | char_("_"))[qi::_val += qi::_1] | blank[qi::_val += " "]);

        trimmedStringExpr = stringExpr[qi::_val = phx::bind(trim, qi::_1)];
        stringExpr        = *((alnum | char_("()_,.+-"))[qi::_val += qi::_1] | blank[qi::_val += " "]);

        stringSet       = stringWithComma[qi::_val = phx::bind(trim, qi::_1)];
        stringWithComma = *((alnum | char_(",_"))[qi::_val += qi::_1] | blank[qi::_val += " "]);

        dbl = omit[*blank] >> double_ >> omit[*blank];

    #ifdef BOOST_SPIRIT_DEBUG
        BOOST_SPIRIT_DEBUG_NODE(comment);
        BOOST_SPIRIT_DEBUG_NODE(trimmedString);
        BOOST_SPIRIT_DEBUG_NODE(trimmedStringExpr);
        BOOST_SPIRIT_DEBUG_NODE(stringSet);
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
    qi::rule<Iterator, std::string()> stringExpr;
    qi::rule<Iterator, std::string()> stringWithComma;

    qi::rule<Iterator, std::string()> comment;
    qi::rule<Iterator, std::string()> trimmedStringExpr;
    qi::rule<Iterator, std::string()> trimmedString;
    qi::rule<Iterator, std::string()> stringSet;

    qi::rule<Iterator, io::base::Line()> line;
    qi::rule<Iterator, std::vector<io::base::Line>()> lines;
    std::stringstream error;
};

namespace reader
{

//------------------------------------------------------------------------------

std::pair<bool, std::string> parse(std::string& buf, std::vector<io::base::Line>& lines)
{
    using boost::spirit::ascii::space;
    using boost::spirit::ascii::blank;
    using boost::spirit::qi::eol;

    using boost::spirit::qi::phrase_parse;

    using iterator_type = std::string::const_iterator;
    using line_parser   = io::base::LineParser<iterator_type>;

    iterator_type iter = buf.begin();
    iterator_type end  = buf.end();

    line_parser grammar; // Our grammar

    bool result     = phrase_parse(iter, end, grammar, space - blank - eol, lines);
    bool success    = result && (iter == end);
    std::string msg = grammar.error.str();
    return std::make_pair(success, msg);
}

//------------------------------------------------------------------------------

DictionaryReader::DictionaryReader(io::base::reader::IObjectReader::Key /*unused*/)
{
}

//------------------------------------------------------------------------------

DictionaryReader::~DictionaryReader()
= default;

//------------------------------------------------------------------------------

void DictionaryReader::read()
{
    std::filesystem::path path = this->getFile();

    SIGHT_INFO("[DictionaryReader::read] dictionary file: " << path.string());
    SIGHT_ASSERT("Empty path for dictionary file", !path.empty());

    // Reading of the file
    std::string buf;
    std::ifstream file;
    file.open(path.string().c_str(), std::ios::binary);

    std::string errorOpen = "Unable to open " + path.string();
    SIGHT_THROW_IF(errorOpen, !file.is_open());

    file.seekg(0, std::ios::end);
    const auto length = file.tellg();
    file.seekg(0, std::ios::beg);

    buf.resize(static_cast<std::size_t>(length));
    char* buffer = buf.data();

    file.read(buffer, static_cast<std::streamsize>(length));
    file.close();

    std::vector<io::base::Line> dico_lines;
    std::pair<bool, std::string> result = parse(buf, dico_lines);

    std::string error = "Unable to parse " + path.string() + " : Bad file format.Error : " + result.second;
    SIGHT_THROW_IF(error, !result.first);

    // File the dictionary Structure
    data::StructureTraitsDictionary::sptr structDico = getConcreteObject();

    for(io::base::Line line : dico_lines)
    {
        data::StructureTraits::sptr newOrgan = data::StructureTraits::New();
        newOrgan->setType(line.type);

        std::string classReformated = reformatString(
            line.organClass
        );
        data::StructureTraitsHelper::ClassTranslatorType::right_const_iterator strClassIter =
            data::StructureTraitsHelper::s_CLASSTRANSLATOR.right.find(classReformated);
        std::string availableValues = getValues(data::StructureTraitsHelper::s_CLASSTRANSLATOR.right);
        error =
            std::string("Organ class ").append(classReformated).append(" isn't available. Authorized type are ")
            .append(availableValues);
        SIGHT_THROW_IF(error, !(strClassIter != data::StructureTraitsHelper::s_CLASSTRANSLATOR.right.end()));
        newOrgan->setClass(strClassIter->second);

        newOrgan->setColor(
            data::Color::New(
                static_cast<float>(line.red) / 255.0F,
                static_cast<float>(line.green) / 255.0F,
                static_cast<float>(line.blue) / 255.0F,
                static_cast<float>(line.alpha) / 100.0F
            )
        );
        std::vector<std::string> categorylist;
        boost::algorithm::split(categorylist, line.category, boost::algorithm::is_any_of(","));
        data::StructureTraits::CategoryContainer categories;
        for(std::string category : categorylist)
        {
            std::string catReformated = reformatString(
                category
            );
            data::StructureTraitsHelper::CategoryTranslatorType::right_const_iterator strCategoryIter =
                data::StructureTraitsHelper::s_CATEGORYTRANSLATOR.right.find(catReformated);
            availableValues = getValues(
                data::StructureTraitsHelper::s_CATEGORYTRANSLATOR.right
            );
            error =
                std::string("Category ").append(catReformated).append(" isn't available. Authorized type are ")
                .append(availableValues);
            SIGHT_THROW_IF(error, !(strCategoryIter != data::StructureTraitsHelper::s_CATEGORYTRANSLATOR.right.end()));
            categories.push_back(strCategoryIter->second);
        }

        newOrgan->setCategories(categories);
        newOrgan->setAttachmentType(line.attachment);
        newOrgan->setNativeExp(line.nativeExp);
        newOrgan->setNativeGeometricExp(line.nativeExpGeo);
        newOrgan->setAnatomicRegion(line.anatomicRegion);
        newOrgan->setPropertyCategory(line.propertyCategory);
        newOrgan->setPropertyType(line.propertyType);
        structDico->addStructure(newOrgan);
    }
}

//------------------------------------------------------------------------------

std::string DictionaryReader::extension() const
{
    return ".dic";
}

//------------------------------------------------------------------------------

std::filesystem::path DictionaryReader::getDefaultDictionaryPath()
{
    return core::runtime::getLibraryResourceFilePath("io_base/OrganDictionary.dic");
}

//------------------------------------------------------------------------------

} // namespace reader

} // namespace sight::io::base
