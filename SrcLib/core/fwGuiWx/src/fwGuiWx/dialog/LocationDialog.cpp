/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/app.h>
#include <wx/dirdlg.h>

#include <boost/filesystem/path.hpp>
#include <boost/tokenizer.hpp>

#include <fwTools/ClassRegistrar.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/MultiFiles.hpp>

#include <fwGui/dialog/ILocationDialog.hpp>
#include <fwWX/convert.hpp>

#include "fwGuiWx/dialog/LocationDialog.hpp"


REGISTER_BINDING( ::fwGui::dialog::ILocationDialog, ::fwGuiWx::dialog::LocationDialog, ::fwGui::dialog::ILocationDialog::FactoryRegistryKeyType , ::fwGui::dialog::ILocationDialog::REGISTRY_KEY );


namespace fwGuiWx
{
namespace dialog
{
//------------------------------------------------------------------------------

LocationDialog::LocationDialog() :
        m_style(wxFD_DEFAULT_STYLE),
        m_type(::fwGui::dialog::ILocationDialog::SINGLE_FILE)
{}

//------------------------------------------------------------------------------

::fwData::location::ILocation::sptr LocationDialog::show()
{
    ::fwData::location::ILocation::sptr location;
    const ::boost::filesystem::path defaultPath = this->getDefaultLocation();
    wxString path = ::fwWX::std2wx( defaultPath.parent_path().string() );
    wxString title = ::fwWX::std2wx(this->getTitle());

    if (m_type == ::fwGui::dialog::ILocationDialog::SINGLE_FILE)
    {
        wxString file = wxFileSelector(
                title,
                path,
                wxT(""),
                wxT(""),
                fileFilters(),
                m_style,
                wxTheApp->GetTopWindow() );

        if( file.IsEmpty() == false )
        {
            ::boost::filesystem::path bpath( ::fwWX::wx2std(file) );
            location = ::fwData::location::SingleFile::New(bpath);
        }
    }
    else if (m_type == ::fwGui::dialog::ILocationDialog::FOLDER)
    {
        wxString file = wxDirSelector(
                title,
                path,
                m_style,
                wxDefaultPosition,
                wxTheApp->GetTopWindow() );

        if( file.IsEmpty() == false )
        {
            ::boost::filesystem::path bpath( ::fwWX::wx2std(file) );
            location = ::fwData::location::Folder::New(bpath);
        }
    }
    else if (m_type == ::fwGui::dialog::ILocationDialog::MULTI_FILES)
    {
        wxFileDialog *fileDialog = new wxFileDialog(wxTheApp->GetTopWindow(),
                title,
                path,
                "",
                fileFilters(),
                m_style | wxFD_MULTIPLE);

        if (fileDialog->ShowModal() != wxID_CANCEL)
        {
            wxArrayString paths;
            fileDialog->GetPaths(paths);
            std::vector < ::boost::filesystem::path > vPaths;
            for (unsigned int i=0 ; i< paths.GetCount() ; i++)
            {
                ::boost::filesystem::path bpath( ::fwWX::wx2std(paths[i]) );
                vPaths.push_back(bpath);
            }
            ::fwData::location::MultiFiles::NewSptr multiFiles;
            multiFiles->setPaths(vPaths);
            location = multiFiles;
        }
    }
    return location;
}

//------------------------------------------------------------------------------

::fwGui::dialog::ILocationDialog&  LocationDialog::setOption( ::fwGui::dialog::ILocationDialog::Options option)
{
    if ( option == ::fwGui::dialog::ILocationDialog::WRITE )
    {
        m_style &= ~wxFD_OPEN ;
        m_style |= wxFD_SAVE ;
    }
    else if ( option == ::fwGui::dialog::ILocationDialog::READ )
    {
        m_style &= ~wxFD_SAVE ;
        m_style |= wxFD_OPEN ;
    }
    else if ( option == ::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST )
    {
        m_style |= wxFD_FILE_MUST_EXIST ;
    }

return *this;
}

//------------------------------------------------------------------------------

void LocationDialog::setType( ::fwGui::dialog::ILocationDialog::Types type )
{
    m_type = type;
}

//------------------------------------------------------------------------------

// exemple ( addFilter("images","*.png *.jpg");
void LocationDialog::addFilter(const std::string &filterName, const std::string &wildcardList )
{
    m_filters.insert( std::make_pair( filterName, wildcardList ));
}

//------------------------------------------------------------------------------

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

} // namespace dialog
} // namespace fwGuiWx
