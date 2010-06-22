/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>
#include<boost/tokenizer.hpp>


#include <fwTools/ClassRegistrar.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/location/Folder.hpp>

#include <fwGui/ILocationDialog.hpp>
#include "fwWX/LocationDialog.hpp"
#include "fwWX/convert.hpp"



REGISTER_BINDING( ::fwGui::ILocationDialog, ::fwWX::LocationDialog, ::fwGui::ILocationDialog::FactoryRegistryKeyType , ::fwGui::ILocationDialog::factoryRegistryKey );


namespace fwWX
{

LocationDialog::LocationDialog() : m_style(wxFD_DEFAULT_STYLE)
{
}



void LocationDialog::setTitle(const std::string &title)
{
    m_title = title;
}



::fwData::location::ILocation::sptr LocationDialog::show()
{
    wxString file = wxFileSelector(
            ::fwWX::std2wx(m_title) ,
            ::fwWX::std2wx( m_path.parent_path().string() ),
            wxT(""),
            wxT(""),
            fileFilters(),
            m_style,
            wxTheApp->GetTopWindow() );

    if( file.IsEmpty() == false )
    {
        ::boost::filesystem::path bpath( ::fwWX::wx2std(file)  );
        return  ::fwData::location::SingleFile::New(bpath);
    }
    else
    {
        return  ::fwData::location::ILocation::sptr();
    }
}



void LocationDialog::setDefaultLocation( ::fwData::location::ILocation::csptr loc)
{
    ::fwData::location::SingleFile::csptr singleFile;
    singleFile = ::fwData::location::SingleFile::dynamicConstCast(loc);
    if (singleFile)
    {
        m_path = singleFile->getPath() ;
    }

    ::fwData::location::Folder::csptr folder;
    folder = ::fwData::location::Folder::dynamicConstCast(loc);
    if (folder)
    {
        m_path = folder->getFolder() ;
    }

    SLM_FATAL_IF( "unsupported location",  !singleFile && !folder );

}


::fwGui::ILocationDialog&  LocationDialog::setOption( ::fwGui::ILocationDialog::Options option)
{
    if ( option == ::fwGui::ILocationDialog::WRITE )
    {
        m_style &= ~wxFD_OPEN ;
        m_style |= wxFD_SAVE ;
    }
    else if ( option == ::fwGui::ILocationDialog::READ )
    {
        m_style &= ~wxFD_SAVE ;
        m_style |= wxFD_OPEN ;
    }
    else if ( option == ::fwGui::ILocationDialog::FILE_MUST_EXIST )
    {
        m_style |= wxFD_FILE_MUST_EXIST ;
    }

return *this;
}


// exemple ( addFilter("images","*.png *.jpg");
void LocationDialog::addFilter(const std::string &filterName, const std::string &wildcardList )
{
    m_filters.insert( std::make_pair( filterName, wildcardList ));
}

// "BMP and GIF files (*.bmp;*.gif)|*.bmp;*.gif|PNG files (*.png)|*.png"
wxString LocationDialog::fileFilters()
{
    std::string result;
    std::multimap< std::string, std::string >::const_iterator iter;
    for ( iter = m_filters.begin(); iter!= m_filters.end(); ++iter)
    {
        std::string filterName = iter->first;
        std::string rawWildcards = iter->second;
        // set rawWildcards in correct format
        std::string wildcards;
        typedef ::boost::tokenizer< boost::char_separator<char> > Tokenizer;
        Tokenizer tok(rawWildcards, boost::char_separator<char>(" "));
        for( Tokenizer::iterator beg=tok.begin(); beg!=tok.end();++beg)
        {
             if  (beg!=tok.begin())
             {
                 wildcards += ";";
             }
             wildcards += *beg;
        }

        if (iter!=m_filters.begin() )
        {
            result += "|";
        }
        result += filterName +" (" +  wildcards +")|" + wildcards;
    }
    return ::fwWX::std2wx(result);
}



}
