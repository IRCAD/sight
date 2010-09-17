/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>
#include <boost/tokenizer.hpp>

#include <QFileDialog>
#include <QString>
#include <QApplication>

#include <fwTools/ClassRegistrar.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/MultiFiles.hpp>

#include <fwGui/dialog/ILocationDialog.hpp>

#include "fwGuiQt/dialog/LocationDialog.hpp"



REGISTER_BINDING( ::fwGui::dialog::ILocationDialog,
        ::fwGuiQt::dialog::LocationDialog,
        ::fwGui::dialog::ILocationDialog::FactoryRegistryKeyType ,
        ::fwGui::dialog::ILocationDialog::REGISTRY_KEY );


namespace fwGuiQt
{
namespace dialog
{
//------------------------------------------------------------------------------

LocationDialog::LocationDialog() :
        m_style(::fwGui::dialog::ILocationDialog::NONE),
        m_type(::fwGui::dialog::ILocationDialog::SINGLE_FILE)
{
}

//------------------------------------------------------------------------------

void LocationDialog::setTitle(const std::string &title)
{
    m_title = title;
}

//------------------------------------------------------------------------------

::fwData::location::ILocation::sptr LocationDialog::show()
{
    QWidget *parent = qApp->activeWindow();
    QString caption = QString::fromStdString(m_title);
    QString path = QString::fromStdString(m_path.parent_path().string());
    QString filter = this->fileFilters();
    //QString selectedFilter; // TODO

    QFileDialog dialog( parent, caption, path, filter );
    dialog.setWindowTitle( QString::fromStdString(m_title) );

    if ( m_style & ::fwGui::dialog::ILocationDialog::WRITE )
    {
        dialog.setAcceptMode(QFileDialog::AcceptSave);
    }
    else if ( m_style & ::fwGui::dialog::ILocationDialog::READ )
    {
        dialog.setAcceptMode(QFileDialog::AcceptOpen);
    }

    if (m_style & ::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST)
    {
        dialog.setFileMode(QFileDialog::ExistingFile);
    }

    if (m_type == ::fwGui::dialog::ILocationDialog::FOLDER)
    {
        dialog.setFileMode(QFileDialog::DirectoryOnly);
    }

    ::fwData::location::ILocation::sptr location;

    if( dialog.exec() )
    {
        if (m_type == ::fwGui::dialog::ILocationDialog::SINGLE_FILE)
        {
            ::boost::filesystem::path bpath( dialog.selectedFiles().at(0).toStdString()  );
            location = ::fwData::location::SingleFile::New(bpath);
        }
        else if (m_type == ::fwGui::dialog::ILocationDialog::FOLDER)
        {
            ::boost::filesystem::path bpath( dialog.selectedFiles().at(0).toStdString()  );
            location = ::fwData::location::Folder::New(bpath);
        }
        else if (m_type == ::fwGui::dialog::ILocationDialog::MULTI_FILES)
        {
            ::fwData::location::MultiFiles::NewSptr multifiles;
            std::vector< ::boost::filesystem::path > paths;
            BOOST_FOREACH (QString filename, dialog.selectedFiles())
            {
                ::boost::filesystem::path bpath( filename.toStdString() );
                paths.push_back(bpath);
            }

            multifiles->setPaths(paths);
            location = multifiles;
        }

    }
   return location;
}

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

void LocationDialog::setType( ::fwGui::dialog::ILocationDialog::Types type )
{
    m_type = type;
}

//------------------------------------------------------------------------------

::fwGui::dialog::ILocationDialog&  LocationDialog::setOption( ::fwGui::dialog::ILocationDialog::Options option)
{
    if ( option == ::fwGui::dialog::ILocationDialog::WRITE )
    {
        m_style = (::fwGui::dialog::ILocationDialog::Options) (m_style & ~ ::fwGui::dialog::ILocationDialog::READ) ;
        m_style = (::fwGui::dialog::ILocationDialog::Options) (m_style | ::fwGui::dialog::ILocationDialog::WRITE);
    }
    else if ( option == ::fwGui::dialog::ILocationDialog::READ )
    {
        m_style = (::fwGui::dialog::ILocationDialog::Options) (m_style & ~::fwGui::dialog::ILocationDialog::WRITE) ;
        m_style = (::fwGui::dialog::ILocationDialog::Options) (m_style | ::fwGui::dialog::ILocationDialog::READ) ;
    }
    else if ( option == ::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST )
    {
        m_style = (::fwGui::dialog::ILocationDialog::Options) (m_style | ::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST)  ;
    }

    return *this;
}

//------------------------------------------------------------------------------

// exemple ( addFilter("images","*.png *.jpg");
void LocationDialog::addFilter(const std::string &filterName, const std::string &wildcardList )
{
    m_filters.insert( std::make_pair( filterName, wildcardList ));
}

//------------------------------------------------------------------------------

// "BMP and GIF files (*.bmp *.gif)|*.bmp *.gif|PNG files (*.png)|*.png"
QString LocationDialog::fileFilters()
{
    std::string result;
    std::multimap< std::string, std::string >::const_iterator iter;
    for ( iter = m_filters.begin(); iter!= m_filters.end(); ++iter)
    {
        std::string filterName = iter->first;
        std::string rawWildcards = iter->second;

        if (iter!=m_filters.begin() )
        {
            result += ";;";
        }
        result += filterName +" (" +  rawWildcards +")" ;
    }
    return QString::fromStdString(result);
}

//------------------------------------------------------------------------------
} // namespace dialog
} //namespace fwGuiQt
