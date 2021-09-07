/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "data/config.hpp"
#include "data/Object.hpp"

#include <core/base.hpp>

#include <ostream>
#include <string>

namespace sight::data
{

/**
 * @brief GenericFieldBase define common operation on simple data.
 */
class DATA_CLASS_API GenericFieldBase : public Object
{
public:

    SIGHT_DECLARE_CLASS(GenericFieldBase);

    DATA_API virtual bool operator==(const GenericFieldBase& lf) = 0;
    DATA_API virtual bool operator!=(const GenericFieldBase& lf) = 0;
    DATA_API virtual bool operator<(const GenericFieldBase& lf)  = 0;
    DATA_API virtual bool operator>(const GenericFieldBase& lf)  = 0;
    DATA_API virtual bool operator<=(const GenericFieldBase& lf) = 0;
    DATA_API virtual bool operator>=(const GenericFieldBase& lf) = 0;
    DATA_API friend std::ostream& operator<<(std::ostream&, const GenericFieldBase&);

    DATA_API virtual ::std::string toString() const               = 0;
    DATA_API virtual void fromString(const ::std::string& _value) = 0;

private:

    DATA_API virtual std::ostream& toOStream(std::ostream& _os) const = 0;
};

} // namespace sight::data
