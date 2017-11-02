/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_LOCATION_FOLDER_HPP__
#define __FWDATA_LOCATION_FOLDER_HPP__

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/location/ILocation.hpp"

#include <boost/filesystem.hpp>

fwCampAutoDeclareDataMacro((fwData)(location)(Folder), FWDATA_API);

namespace fwData
{
namespace location
{
/**
 * @brief This class defines a folder location.
 */
class FWDATA_CLASS_API Folder  : public ILocation
{
public:

    fwCoreClassDefinitionsWithNFactoriesMacro(
        (Folder)(ILocation),
        ((::fwData::factory::New< Folder >, () ))
            ((FolderFactory, ((::boost::filesystem::path))((bool)(false)) ))
        );

    fwCampMakeFriendDataMacro((fwData)(location)(Folder));

    /// Constructor
    FWDATA_API Folder( ::fwData::Object::Key key );

    /// Destructor
    FWDATA_API virtual ~Folder();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /// @brief Set folder filesystem path
    FWDATA_API void setFolder( PathType folder);

    /// @brief Get folder filesystem path
    FWDATA_API PathType getFolder() const;

    /// @brief Set the flag if folder location is recursive
    FWDATA_API void setRecursive( bool recursive);

    /// @brief Get the flag if folder location is recursive
    FWDATA_API bool getRecursive();

protected:

    FWDATA_API static sptr FolderFactory(PathType path, bool recursive = false );

    /// %Folder path
    PathType m_folder;

    /// Flag if folder is recursive
    bool m_isRecursive;

};

/**
 * @struct enableFolder
 * @brief This class is derived by reader/writer.
 *
 * Reader/Writer classes should only need to implement get/setLocation
 */
template<class RW> // reader or writer class should only need to implement get/setLocation
struct enableFolder
{
    /**
     * @brief constructor
     * @param[in] rw reader or writer
     */
    enableFolder(RW* rw) :
        m_rw(rw)
    {
        SLM_ASSERT("m_rw not instanced", m_rw);
    }

    /// @brief Set folder filesystem path
    void setFolder(ILocation::PathType folder)
    {
        getLocation<Folder>(m_rw)->setFolder(folder);
    }

    /// @brief Get folder filesystem path
    ILocation::PathType getFolder()
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

    private:
        /// Not implemented must use constructor with one parameter
        enableFolder();

        /// Reader or writer
        RW* m_rw;

};

}
}

#endif /* __FWDATA_LOCATION_FOLDER_HPP__ */
