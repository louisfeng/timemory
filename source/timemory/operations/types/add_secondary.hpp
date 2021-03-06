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
 * \file timemory/operations/types/add_secondary.hpp
 * \brief Definition for various functions for add_secondary in operations
 */

#pragma once

#include "timemory/operations/declaration.hpp"
#include "timemory/operations/macros.hpp"
#include "timemory/operations/types.hpp"
#include "timemory/settings/declaration.hpp"

namespace tim
{
namespace operation
{
namespace internal
{
//
//--------------------------------------------------------------------------------------//
//
template <typename Tp, bool>
struct add_secondary;
//
template <typename Tp>
struct add_secondary<Tp, true>
{
    using type     = Tp;
    using string_t = std::string;

    TIMEMORY_DEFAULT_OBJECT(add_secondary)

    //----------------------------------------------------------------------------------//
    // if secondary data explicitly specified
    //
    template <typename Storage, typename Iterator>
    add_secondary(Storage* _storage, Iterator _itr, const type& _rhs)
    {
        (*this)(_storage, _itr, _rhs);
    }

    //----------------------------------------------------------------------------------//
    // add_secondary called on type
    //
    template <typename... Args>
    add_secondary(type& _rhs, Args&&... args)
    {
        (*this)(_rhs, std::forward<Args>(args)...);
    }

    //----------------------------------------------------------------------------------//
    // if secondary data explicitly specified
    //
    template <typename Storage, typename Iterator, typename Up>
    auto operator()(Storage* _storage, Iterator _itr, const Up& _rhs) const
    {
        if(!trait::runtime_enabled<Tp>::get() || _storage == nullptr ||
           !settings::add_secondary())
            return;

        using value_type       = typename type::value_type;
        using secondary_data_t = std::tuple<Iterator, const string_t&, value_type>;
        for(const auto& _data : _rhs.get_secondary())
            _storage->append(secondary_data_t{ _itr, _data.first, _data.second });
    }

    //----------------------------------------------------------------------------------//
    // add_secondary called on type
    //
    template <typename... Args>
    auto operator()(type& _rhs, Args&&... args) const
    {
        return sfinae(_rhs, 0, std::forward<Args>(args)...);
    }

private:
    //----------------------------------------------------------------------------------//
    //  If the component has a add_secondary(Args...) member function
    //
    template <typename Up, typename... Args>
    auto sfinae(Up& _obj, int, Args&&... args) const
        -> decltype(_obj.add_secondary(std::forward<Args>(args)...), void())
    {
        if(!trait::runtime_enabled<Tp>::get() || !settings::add_secondary())
            return;

        _obj.add_secondary(std::forward<Args>(args)...);
    }

    //----------------------------------------------------------------------------------//
    //  If the component does not have a add_secondary(Args...) member function
    //
    template <typename Up, typename... Args>
    void sfinae(Up&, long, Args&&...) const
    {}
};
//
//
//--------------------------------------------------------------------------------------//
//
template <typename Tp>
struct add_secondary<Tp, false>
{
    using type     = Tp;
    using string_t = std::string;

    TIMEMORY_DEFAULT_OBJECT(add_secondary)

    //----------------------------------------------------------------------------------//
    // check if secondary data implicitly specified
    //
    template <typename Storage, typename Iterator>
    add_secondary(Storage* _storage, Iterator _itr, const type& _rhs)
    {
        (*this)(_storage, _itr, _rhs);
    }

    //----------------------------------------------------------------------------------//
    // add_secondary called on type
    //
    template <typename... Args>
    add_secondary(type& _rhs, Args&&... args)
    {
        (*this)(_rhs, std::forward<Args>(args)...);
    }

    //----------------------------------------------------------------------------------//
    // if secondary data explicitly specified
    //
    template <typename Storage, typename Iterator>
    auto operator()(Storage* _storage, Iterator _itr, const type& _rhs) const
    {
        return storage_sfinae(_storage, _itr, _rhs, 0);
    }

    //----------------------------------------------------------------------------------//
    // add_secondary called on type
    //
    template <typename... Args>
    auto operator()(type& _rhs, Args&&... args) const
    {
        return sfinae(_rhs, 0, std::forward<Args>(args)...);
    }

private:
    //----------------------------------------------------------------------------------//
    //  If the component has a get_secondary() member function
    //
    template <typename Storage, typename Iterator, typename Up,
              typename value_type = typename type::value_type>
    auto storage_sfinae(Storage* _storage, Iterator _itr, const Up& _rhs, int) const
        -> decltype(_rhs.get_secondary(), void())
    {
        if(!trait::runtime_enabled<Tp>::get() || _storage == nullptr ||
           !settings::add_secondary())
            return;

        using secondary_data_t = std::tuple<Iterator, const string_t&, value_type>;
        for(const auto& _data : _rhs.get_secondary())
            _storage->append(secondary_data_t{ _itr, _data.first, _data.second });
    }

    //----------------------------------------------------------------------------------//
    //  If the component does not have a get_secondary() member function
    //
    template <typename Storage, typename Iterator>
    void storage_sfinae(Storage*, Iterator, const type&, long) const
    {}

    //----------------------------------------------------------------------------------//
    //  If the component has a add_secondary(Args...) member function
    //
    template <typename Up, typename... Args>
    auto sfinae(Up& _obj, int, Args&&... args) const
        -> decltype(_obj.add_secondary(std::forward<Args>(args)...), void())
    {
        if(!trait::runtime_enabled<Tp>::get() || !settings::add_secondary())
            return;

        _obj.add_secondary(std::forward<Args>(args)...);
    }

    //----------------------------------------------------------------------------------//
    //  If the component does not have a add_secondary(Args...) member function
    //
    template <typename Up, typename... Args>
    void sfinae(Up&, long, Args&&...) const
    {}
};
//
}  // namespace internal
//
//--------------------------------------------------------------------------------------//
//
///
/// \struct tim::operation::add_secondary
/// \brief
/// component contains secondary data resembling the original data
/// but should be another node entry in the graph. These types
/// must provide a get_secondary() member function and that member function
/// must return a pair-wise iterable container, e.g. std::map, of types:
///     - std::string
///     - value_type
///
//
//--------------------------------------------------------------------------------------//
//
template <typename Tp>
struct add_secondary
: public internal::add_secondary<Tp, trait::secondary_data<Tp>::value>
{
    using type      = Tp;
    using string_t  = std::string;
    using base_type = internal::add_secondary<Tp, trait::secondary_data<Tp>::value>;

    add_secondary(const type& _rhs, typename type::storage_type* _storage)
    : base_type(_storage, _rhs.get_iterator(), _rhs)
    {}

    //----------------------------------------------------------------------------------//
    // if secondary data explicitly specified
    //
    template <typename Storage, typename Iterator>
    add_secondary(Storage* _storage, Iterator _itr, const type& _rhs)
    : base_type(_storage, _itr, _rhs)
    {}

    //----------------------------------------------------------------------------------//
    // add_secondary called on type
    //
    template <typename... Args>
    add_secondary(type& _rhs, Args&&... args)
    : base_type(_rhs, std::forward<Args>(args)...)
    {}
};
//
//--------------------------------------------------------------------------------------//
//
}  // namespace operation
}  // namespace tim
