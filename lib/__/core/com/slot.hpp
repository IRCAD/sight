/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "core/com/slot_call.hpp"
#include "core/com/util/auto_bind.hpp"
#include "core/function.hpp"

#include <functional>
#include <utility>

namespace sight::core::com
{

template<typename F>
class slot;

//-----------------------------------------------------------------------------

template<typename R, typename ... A>
class SIGHT_CORE_CLASS_API slot<R(A ...)>: public slot_call<R(A ...)>
{
public:

    using signature_type = R(A ...);
    using self_t         = slot<signature_type>;
    using sptr           = std::shared_ptr<self_t>;
    using wptr           = std::weak_ptr<self_t>;

    slot();
};

//-----------------------------------------------------------------------------

template<typename R, typename ... A>
class SIGHT_CORE_CLASS_API slot<std::function<R(A ...)> >: public slot<R(A ...)>
{
public:

    using signature_type = R(A ...);
    using function_t     = std::function<signature_type>;

    template<typename FUNCTOR>
    explicit slot(FUNCTOR _f) :
        slot<R(A ...)>(),
        m_func(std::move(_f))
    {
    }

    ~slot() override
    = default;

    //------------------------------------------------------------------------------

    void run(A ... _a) const override
    {
        m_func(_a ...);
    }

    //------------------------------------------------------------------------------

    R call(A ... _a) const override
    {
        return m_func(_a ...);
    }

private:

    function_t m_func;
};

//-----------------------------------------------------------------------------

template<typename R, typename ... A>
class SIGHT_CORE_CLASS_API slot<slot<R(A ...)> >: public slot<std::function<R(A ...)> >
{
public:

    using signature_type = R(A ...);
    using function_t     = std::function<signature_type>;

    template<typename F>
    explicit slot(sight::sptr<slot_run<F> > _slot);

    template<typename F>
    explicit slot(sight::sptr<slot<F> > _slot);
};

//-----------------------------------------------------------------------------

template<typename F, typename, typename ... bindings>
sight::sptr<slot<typename core::com::util::convert_function_type<F>::type> > new_slot(F _f, bindings ... _bindings);

//-----------------------------------------------------------------------------

// Prototype used for lambdas functions
template<typename F, typename>
sight::sptr<slot<core::lambda_to_function_t<F> > > new_slot(F _f);

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

template<typename R, typename ... A>
slot<R(A ...)>::slot() :
    slot_call<R(A ...)>()
{
    // 'this->' is needed by gcc 4.2
    //NOLINTNEXTLINE(cppcoreguidelines-prefer-member-initializer)
    this->slot_base::m_signature = slot_base::get_type_name<R(A ...)>();
}

//-----------------------------------------------------------------------------

template<typename R, typename ... A>
template<typename F>
slot<slot<R(A ...)> >::slot(sight::sptr<slot_run<F> > _slot) :
    core::com::slot<function_t>(
        core::com::util::auto_bind<
            signature_type,
            boost::function_types::function_arity<F>::value
        >::wrap(&slot_run<F>::run, _slot.get()))
{
    static_assert(std::is_same_v<void, R>);
    this->set_worker(_slot->get_worker());
    this->m_source_slot = _slot;
}

//-----------------------------------------------------------------------------

template<typename R, typename ... A>
template<typename F>
slot<slot<R(A ...)> >::slot(sight::sptr<slot<F> > _slot) :
    core::com::slot<function_t>(
        core::com::util::auto_bind<
            signature_type,
            boost::function_types::function_arity<F>::value
        >::wrap(&core::com::slot<F>::call, _slot.get()))
{
    this->set_worker(_slot->get_worker());
    this->m_source_slot = _slot;
}

//-----------------------------------------------------------------------------

template<typename F, std::enable_if_t<std::is_function_v<typename core::com::util::convert_function_type<F>::type>,
                                      bool> = true,
         typename ... BINDING>
sight::sptr<slot<typename core::com::util::convert_function_type<F>::type> > new_slot(F _f, BINDING ... _binding)
{
#ifdef _DEBUG
    constexpr bool has_valid_nb_args = (sizeof...(_binding) < 2);
    SIGHT_ASSERT("Too many arguments", has_valid_nb_args);
#endif
    using function_t = std::function<typename core::com::util::convert_function_type<F>::type>;
    function_t func = core::com::util::autobind(_f, _binding ...);
    return std::make_shared<slot<function_t> >(func);
}

//-----------------------------------------------------------------------------

template<typename F>
sight::sptr<slot<core::lambda_to_function_t<F> > > new_slot(F _f)
requires(!std::is_function_v<typename core::com::util::convert_function_type<F>::type>)
{
    auto fn = lambda_to_function(_f);
    return std::make_shared<sight::core::com::slot<core::lambda_to_function_t<F> > >(fn);
}

} // namespace sight::core::com
