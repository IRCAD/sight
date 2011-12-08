/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifdef DEBUG
  #define BOOST_SPIRIT_DEBUG
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <exception>

#include <boost/cstdint.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_eol.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/home/phoenix/statement/sequence.hpp>
#include <boost/spirit/home/phoenix/container.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

#include <boost/fusion/include/adapt_struct.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include <fwData/Object.hpp>
#include <fwData/Color.hpp>
#include <fwData/StructureTraitsDictionary.hpp>
#include <fwData/StructureTraits.hpp>
#include <fwData/StructureTraitsHelper.hpp>

#include "fwDataIO/reader/DictionaryReader.hpp"

#include <string>

REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::reader::IObjectReader , ::fwDataIO::reader::DictionaryReader, ::fwDataIO::reader::DictionaryReader );

namespace fwDataIO
{
struct line
{
    std::string type;
    double red;
    double green;
    double blue;
    double alpha;
    std::string catgegory;
    std::string organClass;
    std::string attachment;
    std::string nativeExp;
    std::string nativeExpGeo;
};
}

BOOST_FUSION_ADAPT_STRUCT(
    ::fwDataIO::line,
    (std::string, type)
    (double, red)
    (double, green)
    (double, blue)
    (double, alpha)
    (std::string, catgegory)
    (std::string, organClass)
    (std::string, attachment)
    (std::string, nativeExp)
    (std::string, nativeExpGeo)
)

inline std::string trim ( std::string &s )
{
    return ::boost::algorithm::trim_copy(s);
}

namespace fwDataIO
{

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;


template <typename Iterator>
struct line_parser : qi::grammar<Iterator, std::vector <line>() >
{
    line_parser() : line_parser::base_type(lines)
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

        lines = +( line[phx::push_back(qi::_val, qi::_1)] | comment ) >> eoi  ;
        comment =  *blank >> lit('#') >> *(char_- eol)>> +qi::eol;

        line = trimmedString >> lit(';')
                  >> omit[*blank]>> lit('(')
                    >>  dbl   >> lit(',')
                    >>  dbl   >> lit(',')
                    >>  dbl   >> lit(',')
                    >>  dbl
                >> lit(')') >> omit[*blank]
                >> lit(';')
                >>  stringSet >> lit(';')
                >>  trimmedString >> lit(';')
                >>  trimmedString >> lit(';')
                >>  trimmedStringExpr >> lit(';')
                >>  trimmedStringExpr
                >> +qi::eol;

        trimmedString =   str[qi::_val = phx::bind(trim, qi::_1)] ;
        str =   *( (alnum|char_("_"))[qi::_val += qi::_1] | blank[qi::_val += " "]) ;

        trimmedStringExpr =   stringExpr[qi::_val = phx::bind(trim, qi::_1)] ;
        stringExpr =   *( (alnum|char_("()_,.+-"))[qi::_val += qi::_1] | blank[qi::_val += " "] ) ;

        stringSet =   stringWithComma[qi::_val = phx::bind(trim, qi::_1)] ;
        stringWithComma =   *( (alnum| char_(",_"))[qi::_val += qi::_1] | blank[qi::_val += " "] ) ;


        dbl = omit[*blank] >> double_ >> omit[*blank];

    #ifdef BOOST_SPIRIT_DEBUG
      BOOST_SPIRIT_DEBUG_NODE(comment);
      BOOST_SPIRIT_DEBUG_NODE(trimmedString);
      BOOST_SPIRIT_DEBUG_NODE(trimmedStringExpr);
      BOOST_SPIRIT_DEBUG_NODE(stringSet);
      BOOST_SPIRIT_DEBUG_NODE(dbl);
      BOOST_SPIRIT_DEBUG_NODE(line);
      BOOST_SPIRIT_DEBUG_NODE(lines);
    #endif

      qi::on_error< qi::fail>
      (
              line
              , phx::ref( (std::ostream &)error )
                    << phx::val("Error! Expecting ")
                    << qi::_4                      // what failed?
                    << phx::val(" here: \"")
                    << phx::construct<std::string>(qi::_3, qi::_2)   // iterators to error-pos, end
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

    qi::rule<Iterator, ::fwDataIO::line()> line;
    qi::rule<Iterator, std::vector< ::fwDataIO::line >() > lines;
    std::stringstream error;
};

namespace reader
{
template <typename Iterator>
std::pair<bool,std::string> parse(Iterator first,  Iterator last, std::string& buf, std::vector<fwDataIO::line>& lines)
{
    using boost::spirit::ascii::space;
    using boost::spirit::ascii::blank;
    using boost::spirit::qi::eol;

    using boost::spirit::qi::phrase_parse;

    typedef std::string::const_iterator iterator_type;
    typedef ::fwDataIO::line_parser<iterator_type> line_parser;

    iterator_type iter = buf.begin();
    iterator_type end = buf.end();

    line_parser grammar; // Our grammar

    bool result = phrase_parse(iter, end, grammar,  space - blank - eol, lines);
    bool success =  result && (iter == end);
    std::string msg = grammar.error.str();
    return std::make_pair( success, msg );
}

//------------------------------------------------------------------------------

DictionaryReader::DictionaryReader()
    : ::fwData::location::enableSingleFile< IObjectReader >(this)

{
}

//------------------------------------------------------------------------------

DictionaryReader::~DictionaryReader()
{}

//------------------------------------------------------------------------------

void DictionaryReader::read()
{
    SLM_TRACE_FUNC();
    assert( ::boost::dynamic_pointer_cast< ::fwData::location::SingleFile >(m_location) );
    ::boost::filesystem::path path = ::boost::dynamic_pointer_cast< ::fwData::location::SingleFile >(m_location)->getPath();

    OSLM_INFO( "[DictionaryReader::read] dictionary file: " << path.string());
    SLM_ASSERT("Empty path for TriangularMesh file", !path.empty());

    // Reading of the file
    size_t length;
    std::string buf;
    std::ifstream file;
    file.open(path.string().c_str(), std::ios::binary );

    if (!file.is_open())
    {
        OSLM_ERROR( "Dictionary file loading error for " << path.string());
        std::string error  = "Unable to open " + path.string();
        throw std::exception(error.c_str());
    }

    file.seekg (0, std::ios::end);
    length = file.tellg();
    file.seekg (0, std::ios::beg);


    buf.resize(length);
    char *buffer = &buf[0];

    file.read (buffer, length);
    file.close();


    std::vector < ::fwDataIO::line > dicolines;
    std::pair<bool,std::string> result = parse(buffer, buffer+length, buf, dicolines);
    if (!result.first)
    {
        OSLM_ERROR( "Bad file format : " << path.string() << " Msg : " << result.second);
        std::string error = "Unable to parse " + path.string() + " : Bad file format.Error : " + result.second;
        throw std::exception(error.c_str());
    }

    // File the dictionary Structure
    ::fwData::StructureTraitsDictionary::sptr structDico = getConcreteObject();

    BOOST_FOREACH(::fwDataIO::line line, dicolines)
    {
        ::fwData::StructureTraits::NewSptr newOrgan;
        newOrgan->setType(line.type);

        ::fwData::StructureTraitsHelper::ClassTranslatorType::right_const_iterator strClassIter = ::fwData::StructureTraitsHelper::s_CLASSTRANSLATOR.right.find(line.organClass);
        SLM_ASSERT("Class "<<line.organClass<<" not found.", strClassIter != ::fwData::StructureTraitsHelper::s_CLASSTRANSLATOR.right.end());
        newOrgan->setClass(strClassIter->second);

        newOrgan->setColor(::fwData::Color::New(line.red/255.0f, line.green/255.0f, line.blue/255.0f, line.alpha/100.0f));
        std::vector<std::string> categorylist;
        ::boost::algorithm::split( categorylist, line.catgegory, ::boost::algorithm::is_any_of(",") );
        ::fwData::StructureTraits::CategoryContainer categories;
        BOOST_FOREACH(std::string category, categorylist)
        {
            std::string cat = ::boost::algorithm::trim_copy(category);
            ::fwData::StructureTraitsHelper::CategoryTranslatorType::right_const_iterator strCategoryIter = ::fwData::StructureTraitsHelper::s_CATEGORYTRANSLATOR.right.find(cat);
            SLM_ASSERT("Category "<<cat <<" not found.", strCategoryIter != ::fwData::StructureTraitsHelper::s_CATEGORYTRANSLATOR.right.end());
            categories.push_back(strCategoryIter->second);
        }
        newOrgan->setCategories(categories);
        newOrgan->setAttachmentType(line.attachment);
        newOrgan->setNativeExp(line.nativeExp);
        newOrgan->setNativeGeometricExp(line.nativeExpGeo);
        structDico->addStructure(newOrgan);
    }
}

//------------------------------------------------------------------------------

std::string  DictionaryReader::extension()
{
    SLM_TRACE_FUNC();
    return (".dic");
}

//------------------------------------------------------------------------------

} // namespace reader
} // namespace fwDataIO
