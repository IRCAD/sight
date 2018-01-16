/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Object.hpp"
#include "fwData/PointList.hpp"

#include <vector>

fwCampAutoDeclareDataMacro((fwData)(Tag), FWDATA_API);

namespace fwData
{

/**
 * @brief This class represent a tag. A tag contains a list of points.
 * @see ::fwData::PointList
 */
class FWDATA_CLASS_API Tag : public Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (Tag)(::fwData::Object), (()), ::fwData::factory::New< Tag >);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Tag(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~Tag();

    fwCampMakeFriendDataMacro((fwData)(Tag));

    /**
     * @{
     * @brief Get/Set value of the pointList.
     */
    ::fwData::PointList::sptr & getRefPointList();
    const ::fwData::PointList::sptr& getPointList() const;
    void setPointList (const ::fwData::PointList::sptr& _pointList);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the Type.
     */
    std::string& getRefType ();
    const std::string& getType () const;
    void setType (const std::string& _sType);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the Tag size.
     */
    double& getRefSize ();
    double getSize () const;
    void setSize (const double _size);
    /// @}

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& source, DeepCopyCacheType& cache) override;

protected:

    /// list of points
    ::fwData::PointList::sptr m_pointList;

    /// type of tag
    std::string m_sType;

    /// size of tag
    double m_size;

}; // end class Tag

//-----------------------------------------------------------------------------

inline ::fwData::PointList::sptr& Tag::getRefPointList()
{
    return this->m_pointList;
}

//-----------------------------------------------------------------------------

inline const ::fwData::PointList::sptr& Tag::getPointList() const
{
    return this->m_pointList;
}

//-----------------------------------------------------------------------------

inline void Tag::setPointList (const ::fwData::PointList::sptr& _pointList)
{
    this->m_pointList = _pointList;
}

//-----------------------------------------------------------------------------

inline std::string& Tag::getRefType ()
{
    return m_sType;
}

//-----------------------------------------------------------------------------

inline const std::string& Tag::getType () const
{
    return m_sType;
}

//-----------------------------------------------------------------------------

inline void Tag::setType (const std::string& _sType)
{
    this->m_sType = _sType;
}

//-----------------------------------------------------------------------------

inline double Tag::getSize () const
{
    return m_size;
}

//-----------------------------------------------------------------------------

inline double& Tag::getRefSize ()
{
    return m_size;
}

//-----------------------------------------------------------------------------

inline void Tag::setSize (const double _size)
{
    m_size = _size;
}

//-----------------------------------------------------------------------------

} // end namespace fwData

