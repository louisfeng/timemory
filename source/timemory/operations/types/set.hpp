// MIT License
//
// Copyright (c) 2020, The Regents of the University of California,
// through Lawrence Berkeley National Laboratory (subject to receipt of any
// required approvals from the U.S. Dept. of Energy).  All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/**
 * \file timemory/operations/types/set_prefix.hpp
 * \brief Definition for various functions for set_prefix in operations
 */

#pragma once

#include "timemory/operations/declaration.hpp"
#include "timemory/operations/macros.hpp"
#include "timemory/operations/types.hpp"

namespace tim
{
namespace operation
{
//
//--------------------------------------------------------------------------------------//
//
//
//
//--------------------------------------------------------------------------------------//
//
template <typename Tp>
struct set_prefix
{
    using type     = Tp;
    using string_t = string_view_t;

    TIMEMORY_DELETED_OBJECT(set_prefix)

    TIMEMORY_HOT_INLINE set_prefix(type& obj, const string_t& _prefix);
    TIMEMORY_HOT_INLINE set_prefix(type& obj, uint64_t _nhash, const string_t& _prefix);

private:
    //  If the component has a set_prefix(const string_t&) member function
    template <typename U>
    TIMEMORY_HOT_INLINE auto sfinae_str(U& obj, int, int, int, const string_t& _prefix)
        -> decltype(obj.set_prefix(_prefix))
    {
        return obj.set_prefix(_prefix);
    }

    template <typename U, typename S>
    TIMEMORY_HOT_INLINE auto sfinae_str(U& obj, int, int, long, const S& _prefix)
        -> decltype(obj.set_prefix(_prefix.c_str()))
    {
        return obj.set_prefix(_prefix.c_str());
    }

    template <typename U, typename S>
    TIMEMORY_HOT_INLINE auto sfinae_str(U& obj, int, long, long, const S& _prefix)
        -> decltype(obj.set_prefix(_prefix.data()))
    {
        return obj.set_prefix(_prefix.data());
    }

    //  If the component does not have a set_prefix(const string_t&) member function
    template <typename U>
    TIMEMORY_INLINE void sfinae_str(U&, long, long, long, const string_t&)
    {}

private:
    //  If the component has a set_prefix(uint64_t) member function
    template <typename U>
    TIMEMORY_HOT_INLINE auto sfinae_hash(U& obj, int, uint64_t _nhash)
        -> decltype(obj.set_prefix(_nhash))
    {
        return obj.set_prefix(_nhash);
    }

    //  If the component does not have a set_prefix(uint64_t) member function
    template <typename U>
    TIMEMORY_INLINE void sfinae_hash(U&, long, uint64_t)
    {}
};
//
//--------------------------------------------------------------------------------------//
//
//
//
//--------------------------------------------------------------------------------------//
//
template <typename Tp>
struct set_scope
{
    using type     = Tp;
    using string_t = std::string;

    TIMEMORY_DELETED_OBJECT(set_scope)

    TIMEMORY_HOT_INLINE set_scope(type& obj, scope::config _data);

private:
    //  If the component has a set_scope(...) member function
    template <typename T>
    TIMEMORY_HOT_INLINE auto sfinae(T& obj, int, scope::config _data)
        -> decltype(obj.set_scope(_data))
    {
        return obj.set_scope(_data);
    }

    //  If the component does not have a set_scope(...) member function
    template <typename T>
    TIMEMORY_INLINE void sfinae(T&, long, scope::config)
    {}
};
//
//--------------------------------------------------------------------------------------//
//
template <typename Tp>
set_prefix<Tp>::set_prefix(type& obj, const string_t& _prefix)
{
    if(!trait::runtime_enabled<type>::get())
        return;

    sfinae_str(obj, 0, 0, 0, _prefix);
}
//
//--------------------------------------------------------------------------------------//
//
template <typename Tp>
set_prefix<Tp>::set_prefix(type& obj, uint64_t _nhash, const string_t& _prefix)
{
    if(!trait::runtime_enabled<type>::get())
        return;

    sfinae_hash(obj, 0, _nhash);
    sfinae_str(obj, 0, 0, 0, _prefix);
}
//
//--------------------------------------------------------------------------------------//
//
template <typename Tp>
set_scope<Tp>::set_scope(type& obj, scope::config _data)
{
    if(!trait::runtime_enabled<type>::get())
        return;

    sfinae(obj, 0, _data);
}
//
//--------------------------------------------------------------------------------------//
//
}  // namespace operation
}  // namespace tim
