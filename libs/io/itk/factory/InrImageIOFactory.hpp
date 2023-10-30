/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include <itkImageIOBase.h>
#include <itkObjectFactoryBase.h>

namespace itk
{

/**
 * \class InrImageIOFactory
 * \brief Create instances of InrImageIO objects using an object factory.
 */
class ITK_EXPORT inr_image_io_factory : public ObjectFactoryBase
{
public:

    /** Standard class typedefs. */
    using self_t       = inr_image_io_factory;
    using superclass_t = ObjectFactoryBase;
    using Pointer      = SmartPointer<self_t>;

    inr_image_io_factory(const self_t&) = delete; //purposely not implemented
    void operator=(const self_t&)       = delete; //purposely not implemented

    /** Class methods used to interface with the registered factories. */
    const char* GetITKSourceVersion() const override;
    const char* GetDescription() const override;

    /** Method for class instantiation. */
    itkFactorylessNewMacro(self_t)

    /** Run-time type information (and related methods). */
    itkTypeMacro(InrImageIOFactory, ObjectFactoryBase)

    /** Register one factory of this type  */
    static void register_one_factory()
    {
        inr_image_io_factory::Pointer meta_factory = inr_image_io_factory::New();
        ObjectFactoryBase::RegisterFactory(meta_factory);
    }

protected:

    inr_image_io_factory();
    ~inr_image_io_factory() override;
    void PrintSelf(std::ostream& _os, Indent _indent) const override;
};

} // end namespace itk
