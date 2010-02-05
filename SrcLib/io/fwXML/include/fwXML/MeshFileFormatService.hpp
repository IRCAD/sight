/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */



#ifndef MeshFILEFORMAT_H_
#define MeshFILEFORMAT_H_



#include "fwXML/config.hpp"
#include "fwXML/IFileFormatService.hpp"


namespace fwXML
{

/**
 * @brief The purpose of this class is to provide an io service to save data
 * @TODO allow specialisation for a specific saver itk DICOM ETC... AS SUBCLASS or strategy pattern .
 */
class FWXML_CLASS_API MeshFileFormatService : public IFileFormatService
{
public:
    fwCoreServiceClassDefinitionsMacro ( (MeshFileFormatService)(::fwXML::IFileFormatService::Baseclass) ) ;
    FWXML_API MeshFileFormatService();

    FWXML_API virtual ~MeshFileFormatService();

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

#endif /*MeshFILEFORMAT_H_*/
