/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_WXZIPFOLDER_HPP_
#define _FWWX_WXZIPFOLDER_HPP_

#include <wx/wx.h>
#include <wx/dir.h>
#include <wx/zipstrm.h>
#include <wx/wfstream.h>

#include "fwWX/config.hpp"

namespace fwWX
{

//------------------------------------------------------------------------------

/**
 * @brief   This interface defines static functions to zip or unzip a folder.
 * @class   wxZipFolder.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 *
 * This class provides static method to zip or unzip folder. Methods use wxWidgets API to archive data. The compression rate is equal to 0 ( simple archive ).
 */
class FWWX_CLASS_API wxZipFolder
{

public:

    /**
     * @brief This method is used to compress (zip format) a folder
     * @param[in] srcFolderName source folder path
     * @param[in] destZipFileName file path resulting to folder compression
     * @return true if folder compression is a success
     * @see this method use wxDirTraverserZIP class to realize the directory exploration
     * @todo Set in parameter the compression rate.
     */
    FWWX_API static bool packFolder( const wxString & _srcFolderName, const wxString & _destZipFileName );

    /**
     * @brief This method is used to uncompress (zip format) a folder
     * @param[in] srcZipFileName source zip folder path
     * @param[in] destFolderName file path resulting to folder decompression
     * @return true if folder decompression is a success
     */
    FWWX_API static bool unpackFolder( const wxString & _srcZipFileName, const wxString & _destFolderName );

};

//------------------------------------------------------------------------------

/**
 * @brief   Class used to explore in depth directory and to archive collected data
 * @class   wxDirTraverserZIP.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */
class FWWX_CLASS_API wxDirTraverserZIP : public wxDirTraverser
{

public:

    /**
     * @brief Dir Traverser constructor. Do nothing, just sets class members.
     * @param[in] _stream to set wxDirTraverserZIP::m_stream
     * @param[in] _baseDir to set wxDirTraverserZIP::m_baseDir
     */
    FWWX_API wxDirTraverserZIP( wxZipOutputStream & _stream, const wxString & _baseDir );


    /**
     * @brief This function is called for each file during exploration.
     * @param[in] _filename filename path
     * @return wxDIR_CONTINUE if the file copy is a success, else wxDIR_STOP.
     * This method open filename and copy the content file in wxDirTraverserZIP::m_stream.
     */
    FWWX_API virtual wxDirTraverseResult OnFile( const wxString& _filename );

    /**
     * @brief This function is called for each directory during exploration. This method do nothing on directories.
     * @param[in] _dirname directory path
     * @return Always return wxDIR_CONTINUE to continue the exploration
     */
    FWWX_API virtual wxDirTraverseResult OnDir( const wxString& _dirname );

    /**
     * @brief This function is called for each directory which we failed to open for enumerating.
     * @param[in] _openerrorname directory path which we failed to open
     * @return Always return wxDIR_STOP to stop the exploration
     */
    FWWX_API virtual wxDirTraverseResult OnOpenError( const wxString& _openerrorname );

private:

    /// Zip stream used to archive data during exploration
    wxZipOutputStream & m_stream;

    /**
     * @brief Base directory path of compressed folder.
     *
     * It is used to store all compressed files with a relative path in zip. This class parameter is used in OnFile method. this method gives in parameter the full filename path.
     */
    wxString m_baseDir;
};

//------------------------------------------------------------------------------

} //namespace fwWX

#endif // _FWWX_WXZIPFOLDER_HPP_
