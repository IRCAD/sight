/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef IMAGEFILEFORMAT_H_
#define IMAGEFILEFORMAT_H_



#include "fwXML/config.hpp"
#include "fwXML/IFileFormatService.hpp"


namespace fwXML
{

/**
 * @brief The purpose of this class is to provide an io service to save data
 * @TODO allow specialisation for a specific saver itk DICOM ETC... AS SUBCLASS or strategy pattern .
 */
class FWXML_CLASS_API ImageFileFormatService : public IFileFormatService
{
public:
    fwCoreServiceClassDefinitionsMacro ( (ImageFileFormatService)(::fwXML::IFileFormatService::Baseclass) ) ;
    FWXML_API ImageFileFormatService();

    FWXML_API virtual ~ImageFileFormatService();

    FWXML_API virtual void load();

    FWXML_API virtual void save();

protected :

    /**
     * @brief install default policies if no explicitly set :
     * @li firstt try to use INR RW : our application must provide libitkIO ( hard link, bundles started etc...)
     * @li if not set and INR RW policy install failed use gzBufferRW
     */
    void RWPoliciesInstall();





};

}

#endif /*IMAGEFILEFORMAT_H_*/
