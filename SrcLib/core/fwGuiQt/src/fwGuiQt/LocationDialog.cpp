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

#include <fwGui/ILocationDialog.hpp>

#include "fwGuiQt/LocationDialog.hpp"



REGISTER_BINDING( ::fwGui::ILocationDialog,
        ::fwGuiQt::LocationDialog,
        ::fwGui::ILocationDialog::FactoryRegistryKeyType ,
        ::fwGui::ILocationDialog::REGISTRY_KEY );


namespace fwGuiQt
{


LocationDialog::LocationDialog() : m_style(::fwGui::ILocationDialog::NONE)
{
}



void LocationDialog::setTitle(const std::string &title)
{
    m_title = title;
}



::fwData::location::ILocation::sptr LocationDialog::show()
{
    QWidget *parent = qApp->activeWindow();
    QString caption = QString::fromStdString(m_title);
    QString path = QString::fromStdString(m_path.parent_path().string());
    QString filter = this->fileFilters();
    //QString selectedFilter; // TODO

    QFileDialog dialog( parent, caption, path, filter );
    dialog.setWindowTitle( QString::fromStdString(m_title) );

    if ( m_style & ::fwGui::ILocationDialog::WRITE )
    {
        dialog.setAcceptMode(QFileDialog::AcceptSave);
    }
    else if ( m_style & ::fwGui::ILocationDialog::READ )
    {
        dialog.setAcceptMode(QFileDialog::AcceptOpen);
    }

    if (m_style & ::fwGui::ILocationDialog::FILE_MUST_EXIST)
    {
        dialog.setFileMode(QFileDialog::ExistingFile);
    }

    if( dialog.exec() )
    {
        ::boost::filesystem::path bpath( dialog.selectedFiles().at(0).toStdString()  );
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
        m_style = (::fwGui::ILocationDialog::Options) (m_style & ~ ::fwGui::ILocationDialog::READ) ;
        m_style = (::fwGui::ILocationDialog::Options) (m_style | ::fwGui::ILocationDialog::WRITE);
    }
    else if ( option == ::fwGui::ILocationDialog::READ )
    {
        m_style = (::fwGui::ILocationDialog::Options) (m_style & ~::fwGui::ILocationDialog::WRITE) ;
        m_style = (::fwGui::ILocationDialog::Options) (m_style | ::fwGui::ILocationDialog::READ) ;
    }
    else if ( option == ::fwGui::ILocationDialog::FILE_MUST_EXIST )
    {
        m_style = (::fwGui::ILocationDialog::Options) (m_style | ::fwGui::ILocationDialog::FILE_MUST_EXIST)  ;
    }

    return *this;
}


// exemple ( addFilter("images","*.png *.jpg");
void LocationDialog::addFilter(const std::string &filterName, const std::string &wildcardList )
{
    m_filters.insert( std::make_pair( filterName, wildcardList ));
}

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



} //namespace fwGuiQt
