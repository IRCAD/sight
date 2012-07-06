/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _DATA_LOCATION_FOLDER_HPP_
#define _DATA_LOCATION_FOLDER_HPP_


#include <boost/filesystem.hpp>

#include "fwData/config.hpp"
#include "fwData/location/ILocation.hpp"
#include "fwData/Factory.hpp"

namespace fwData
{
namespace location
{
/**
 * @class Folder
 * @brief This class defines a folder %location.
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API Folder  : public ILocation
{
public:

    fwCoreClassDefinitionsWithNFactoriesMacro(
            (Folder)(ILocation),
            ((::fwData::Factory::New< Folder > ,() ))
            ((FolderFactory ,((::boost::filesystem::path)) ((bool)(false)) ))
    );

    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( Folder::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( Folder::csptr _source );

    /// @brief Set folder filesystem path
    FWDATA_API void setFolder( ::boost::filesystem::path folder);

    /// @brief Get folder filesystem path
    FWDATA_API ::boost::filesystem::path getFolder() const;

    /// @brief Set the flag if folder location is recursive
    FWDATA_API void setRecursive( bool recursive);

    /// @brief Get the flag if folder location is recursive
    FWDATA_API bool getRecursive();

protected :
    /// Constructor
    FWDATA_API Folder();

    FWDATA_API static sptr FolderFactory(::boost::filesystem::path _path, bool recursive=false );

    /// Destructor
    FWDATA_API virtual ~Folder();

    /// %Folder path
    ::boost::filesystem::path m_folder;

    /// Flag if folder is recursive
    bool m_isRecursive;

};

/**
 * @struct enableFolder
 * @brief This class is derivated by reader/writter.
 *
 * Reader/Writter classes should only need to implement get/setLocation
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
template<class RW> // reader or writter class should only need to implement get/setLocation
struct enableFolder
{
    /**
     * @brief constructor
     * @param[in] rw reader or writer
     */
    enableFolder(RW *rw) : m_rw(rw) { SLM_ASSERT("m_rw not instanced", m_rw); }

    /// @brief Set folder filesystem path
    void setFolder(::boost::filesystem::path folder)
    {
        getLocation<Folder>(m_rw)->setFolder(folder);
    }

    /// @brief Get folder filesystem path
    ::boost::filesystem::path getFolder()
    {
        return getLocation<Folder>(m_rw)->getFolder();
    }

    /// @brief Set the flag if folder location is recursive
    void setRecursive( bool val)
    {
        getLocation<Folder>(m_rw)->setRecursive(val);
    }

    /// @brief Get the flag if folder location is recursive
    bool getRecursive()
    {
        return getLocation<Folder>(m_rw)->getRecursive();
    }

private :
    /// Not implemented must use constructor with one parameter
    enableFolder();

    /// Reader or writer
    RW *m_rw;

};



}
}

#endif /* _DATA_LOCATION_FOLDER_HPP_ */
