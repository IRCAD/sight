/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <memory>

#include <fwCore/base.hpp>

#include "fwWX/wxZipFolder.hpp"

namespace fwWX
{

//------------------------------------------------------------------------------

wxDirTraverserZIP::wxDirTraverserZIP( wxZipOutputStream & _stream, const wxString & _baseDir) :
    wxDirTraverser(),
    m_stream ( _stream ),
    m_baseDir ( _baseDir )
{}

//------------------------------------------------------------------------------

wxDirTraverseResult wxDirTraverserZIP::OnFile( const wxString & _filename )
{
    do
    {
        wxFileName newFileName( _filename );
        newFileName.MakeRelativeTo( m_baseDir );
        if ( ! m_stream.PutNextEntry( newFileName.GetFullPath() ) )
        {
            break;
        }

        wxFileInputStream in(_filename);
        if(!in.IsOk())
        {
            break;
        }

        wxString extension = newFileName.GetExt();
        if( extension == _("trian") )
        {
            m_stream.SetLevel(9);
            m_stream.Write(in);
            m_stream.SetLevel(0);
        }
        else
        {
            m_stream.Write(in);
        }

        return wxDIR_CONTINUE;
    }
    while(false);
    return wxDIR_STOP;
}

//------------------------------------------------------------------------------

wxDirTraverseResult wxDirTraverserZIP::OnDir( const wxString & _dirname )
{
    return wxDIR_CONTINUE;
}

//------------------------------------------------------------------------------

wxDirTraverseResult wxDirTraverserZIP::OnOpenError( const wxString & _openerrorname )
{
    return wxDIR_STOP;
}

//------------------------------------------------------------------------------

bool wxZipFolder::packFolder( const wxString & _srcFolderName, const wxString & _destZipFileName )
{
    do
    {
        wxFFileOutputStream out(_destZipFileName);
        if(!out.IsOk()) break;
        wxZipOutputStream zip(out, 0);
        if(!zip.IsOk()) break;
        wxDirTraverserZIP traverser(zip, _srcFolderName);
        wxDir srcDir(_srcFolderName);
        if(srcDir.Traverse(traverser, wxEmptyString) == (size_t)-1) break;
        return true;
    }
    while(false);
    return false;
}

//------------------------------------------------------------------------------

bool wxZipFolder::unpackFolder( const wxString & _srcZipFileName, const wxString & _destFolderName )
{
    std::auto_ptr<wxZipEntry> entry;

    wxFFileInputStream in(_srcZipFileName);
    assert( in.IsOk() );

    wxZipInputStream zip(in);
    assert( zip.IsOk() );

    while ( entry.reset( zip.GetNextEntry() ), entry.get() != NULL )
    {
        if ( ! entry->IsDir() )
        {
            // access meta-data
            wxString name = _destFolderName + "/" + entry->GetName();
            OSLM_DEBUG( "File Path = " << name.mb_str() );

            // File Name
            wxFileName filePath ( name );
            OSLM_DEBUG( "Dir Path = " << filePath.GetPath().mb_str() );

            // Create Directories
            wxFileName dirPath = wxFileName::DirName( filePath.GetPath() );
            bool mkdir = dirPath.Mkdir( wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL );
            assert( mkdir );

            // Read zip and writ it in file
            wxFileOutputStream fileStream ( name );
            assert( fileStream.IsOk() );
            fileStream << zip;
            fileStream.Close();
        }
    }

    return true;
}

//------------------------------------------------------------------------------

} //namespace fwWX
