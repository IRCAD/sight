/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWDATA_LANDMARKS_HPP__
#define __FWDATA_LANDMARKS_HPP__

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Object.hpp"

#include <array>
#include <map>
#include <vector>

fwCampAutoDeclareDataMacro((fwData)(Landmarks), FWDATA_API);

namespace fwData
{
/**
 * @brief   This class defines a list of 3D points inside groups.
 */
class FWDATA_CLASS_API Landmarks : public Object
{

public:

    typedef std::array< float, 4 > ColorType;
    typedef std::array< double, 3 > PointType;
    typedef float SizeType;
    typedef std::vector< PointType > PointContainer;
    typedef std::vector< std::string > GroupNameContainer;

    enum class Shape
    {
        SPHERE,
        CUBE
    };

    struct LandmarksGroup
    {
        LandmarksGroup(ColorType color, SizeType size, Shape shape, bool visibility) :
            m_color(color),
            m_size(size),
            m_shape(shape),
            m_visibility(visibility)
        {
        }

        ColorType m_color;
        SizeType m_size;
        Shape m_shape;
        bool m_visibility;
        PointContainer m_points;
    };

    fwCoreClassDefinitionsWithFactoryMacro( (Landmarks)(::fwData::Object), (()), ::fwData::factory::New< Landmarks > );

    fwCampMakeFriendDataMacro((fwData)(Landmarks));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Landmarks(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~Landmarks();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /**
     * @brief Add a new landmark group
     *
     * @throw ::fwData::Exception if a group exists with the given name
     */
    FWDATA_API void addGroup(const std::string& name, const ColorType& color = {{1.0f, 1.0f, 1.0f, 1.0f}},
                             const SizeType size = 1.0f, const Shape shape = Shape::SPHERE,
                             const bool visibility = true);

    /// Return all group names
    FWDATA_API const GroupNameContainer getGroupNames() const;

    /**
     * @brief Get the group properties
     *
     * @throw ::fwData::Exception if the group does not exist
     */
    FWDATA_API const LandmarksGroup& getGroup(const std::string& name) const;

    /**
     * @brief Get the group properties
     *
     * @throw ::fwData::Exception if the group does not exist
     */
    FWDATA_API LandmarksGroup& getGroup(const std::string& name);

    /**
     * @brief Rename the group
     *
     * @throw ::fwData::Exception if the group does not exist or if a group exists with the new name
     */
    FWDATA_API void renameGroup(const std::string& oldName, const std::string& newName);

    /**
     * @brief Remove the group
     *
     * @throw ::fwData::Exception if the group does not exist
     */
    FWDATA_API void removeGroup(const std::string& name);

    /**
     * @brief Set the group's color.
     *
     * The color is encoded as a rgba float array ranging from 0 to 1
     *
     * @throw ::fwData::Exception if the group does not exist
     */
    FWDATA_API void setGroupColor(const std::string& name, const ColorType& color);

    /**
     * @brief Set the group's landmark size
     *
     * @throw ::fwData::Exception if the group does not exist
     */
    FWDATA_API void setGroupSize(const std::string& name, const SizeType size);

    /**
     * @brief Set the group's shape (SPHERE or CUBE)
     *
     * @throw ::fwData::Exception if the group does not exist
     */
    FWDATA_API void setGroupShape(const std::string& name, const Shape shape);

    /**
     * @brief Set the group's visibility
     *
     * @throw ::fwData::Exception if the group does not exist
     */
    FWDATA_API void setGroupVisibility(const std::string& name, const bool visibility);

    /**
     * @brief Add a point to the group
     *
     * @throw ::fwData::Exception if the group does not exist
     */
    FWDATA_API void addPoint(const std::string& name, const PointType& point);

    /**
     * @brief Insert a point to the group at the given index
     *
     * @throw ::fwData::Exception if the group does not exist
     */
    FWDATA_API void insertPoint(const std::string& name, const size_t index, const PointType& point);

    /**
     * @brief Get the point at the given index
     *
     * @throw ::fwData::Exception if the group does not exist
     */
    FWDATA_API const PointType& getPoint(const std::string& name, size_t index) const;

    /**
     * @brief Get the point at the given index
     *
     * @throw ::fwData::Exception if the group does not exist
     */
    FWDATA_API PointType& getPoint(const std::string& name, size_t index);

    /**
     * @brief Get all group's points
     *
     * @throw ::fwData::Exception if the group does not exist
     */
    FWDATA_API const PointContainer& getPoints(const std::string& name) const;

    /**
     * @brief Remove the point at the given index
     *
     * @throw ::fwData::Exception if the group does not exist
     */
    FWDATA_API void removePoint(const std::string& name, size_t index);

    /**
     * @brief Remove all the group's points
     *
     * @throw ::fwData::Exception if the group does not exist
     */
    FWDATA_API void clearPoints(const std::string& name);

    /// Return the number of groups
    size_t getNumberOfGroups() const;

    /// Return the number of points in the entire structure
    FWDATA_API size_t getNumberOfPoints() const;

    /// Return the number of points in the group
    FWDATA_API size_t getNumberOfPoints(const std::string& name) const;

    /**
     * @name Signals
     * @{
     */
    /// Type of signal when a group is added
    typedef ::fwCom::Signal< void (std::string name) > GroupAddedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_GROUP_ADDED_SIG;

    /// Type of signal when a group is removed
    typedef ::fwCom::Signal< void (std::string name) > GroupRemovedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_GROUP_REMOVED_SIG;

    /// Type of signal when a point is added
    typedef ::fwCom::Signal< void (std::string name) > PointAddedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_POINT_ADDED_SIG;

    /// Type of signal when a point is removed
    typedef ::fwCom::Signal< void (std::string name, size_t index) > PointRemovedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_POINT_REMOVED_SIG;

    /// Type of signal when a point is inserted
    typedef ::fwCom::Signal< void (std::string name, size_t index) > PointInsertedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_POINT_INSERTED_SIG;

    /// Type of signal when a point is modified
    typedef ::fwCom::Signal< void (std::string name, size_t index) > PointModifiedSigType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_POINT_MODIFIED_SIG;

    /// Type of signal when group properties changed
    typedef ::fwCom::Signal< void (std::string name) > GroupModifiedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_GROUP_MODIFIED_SIG;

    /// Type of signal when a group is renamed
    typedef ::fwCom::Signal< void (std::string oldName, std::string newName) > GroupRenamedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_GROUP_RENAMED_SIG;

    /// Type of signal when point is selected
    typedef ::fwCom::Signal< void (std::string name, size_t index) > PointSelectedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_POINT_SELECTED_SIG;

    /// Type of signal when point is deselected
    typedef ::fwCom::Signal< void (std::string name, size_t index) > PointDeselectedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_POINT_DESELECTED_SIG;

    /**
     * @}
     */

private:

    typedef std::map< std::string, LandmarksGroup > LandmarksContainer;
    LandmarksContainer m_landmarks;
}; // end class Landmarks

//------------------------------------------------------------------------------

inline size_t Landmarks::getNumberOfGroups() const
{
    return m_landmarks.size();
}

} // end namespace fwData

#endif // __FWDATA_LANDMARKS_HPP__

