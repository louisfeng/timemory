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
//

/** \file timemory/variadic/auto_list.hpp
 * \headerfile timemory/variadic/auto_list.hpp "timemory/variadic/auto_list.hpp"
 */

#pragma once

#include "timemory/mpl/filters.hpp"
#include "timemory/runtime/types.hpp"
#include "timemory/settings/declaration.hpp"
#include "timemory/utility/macros.hpp"
#include "timemory/utility/utility.hpp"
#include "timemory/variadic/component_list.hpp"
#include "timemory/variadic/macros.hpp"
#include "timemory/variadic/types.hpp"

#include <cstdint>
#include <string>

namespace tim
{
//--------------------------------------------------------------------------------------//
/// \class tim::auto_list
/// \tparam Types Specification of the component types to bundle together
///
/// \brief This is a variadic component wrapper where all components are optional
/// at runtime. Accept unlimited number of parameters. The constructor starts the
/// components, the destructor stops the components. The default behavior is
/// to query the TIMEMORY_AUTO_LIST_INIT environment variable once (the first
/// time the bundle is used) and use that list of components (if any) to
/// initialize the components which are part of it's template parameters.
/// This behavior can be modified by assigning a new lambda/functor to the
/// reference which is returned from \ref tim::auto_list<Types...>::get_initializer().
/// Assignment is not thread-safe since this is relatively unnecessary... if a different
/// set of components are required on a particular thread, just create a different
/// type with those particular components or pass the initialization functor to the
/// constructor.
///
/// \code{.cpp}
/// using bundle_t = tim::auto_list<wall_clock, cpu_clock, peak_rss>;
///
/// void foo()
/// {
///     setenv("TIMEMORY_AUTO_LIST_COMPONENTS", "wall_clock", 0);
///
///     auto bar = bundle_t("bar");
///
///     bundle_t::get_initializer() = [](bundle_t& b)
///     {
///         b.initialize<cpu_clock, peak_rss>();
///     };
///
///     auto qix = bundle_t("qix");
///
///     auto local_init = [](bundle_t& b)
///     {
///         b.initialize<thread_cpu_clock, peak_rss>();
///     };
///
///     auto spam = bundle_t("spam", ..., local_init);
///
/// }
/// \endcode
///
/// The above code will record wall-clock timer on first use of "bar", and
/// will record cpu-clock, peak-rss at "qix", and peak-rss at "spam". If foo()
/// is called a second time, "bar" will record cpu-clock and peak-rss. "spam" will
/// always use the local initialized. If none of these initializers are set, wall-clock
/// will be recorded for all of them. The intermediate storage will happen on the heap and
/// when the destructor is called, it will add itself to the call-graph
template <typename... Types>
class auto_list
: public concepts::wrapper
, public concepts::variadic
, public concepts::auto_wrapper
, public concepts::heap_wrapper
{
public:
    using this_type           = auto_list<Types...>;
    using base_type           = component_list<Types...>;
    using auto_type           = this_type;
    using component_type      = typename base_type::component_type;
    using data_type           = typename component_type::data_type;
    using tuple_type          = typename component_type::tuple_type;
    using sample_type         = typename component_type::sample_type;
    using type                = convert_t<typename component_type::type, auto_list<>>;
    using initializer_type    = std::function<void(this_type&)>;
    using string_t            = std::string;
    using captured_location_t = typename component_type::captured_location_t;

    static constexpr bool has_gotcha_v      = component_type::has_gotcha_v;
    static constexpr bool has_user_bundle_v = component_type::has_user_bundle_v;

public:
    //----------------------------------------------------------------------------------//
    //
    static void init_storage() { component_type::init_storage(); }

    //----------------------------------------------------------------------------------//
    //
    /// \fn auto& get_initializer()
    /// \brief
    static auto& get_initializer()
    {
        static auto env_enum = enumerate_components(
            tim::delimit(tim::get_env<string_t>("TIMEMORY_AUTO_LIST_INIT", "")));

        static initializer_type _instance = [=](this_type& cl) {
            ::tim::initialize(cl, env_enum);
        };
        return _instance;
    }

public:
    template <typename Func = initializer_type>
    explicit auto_list(const string_t&, scope::config = scope::get_default(),
                       bool report_at_exit = settings::destructor_report(),
                       const Func&         = get_initializer());

    template <typename Func = initializer_type>
    explicit auto_list(const captured_location_t&, scope::config = scope::get_default(),
                       bool report_at_exit = settings::destructor_report(),
                       const Func&         = get_initializer());

    template <typename Func = initializer_type>
    explicit auto_list(size_t, scope::config = scope::get_default(),
                       bool report_at_exit = settings::destructor_report(),
                       const Func&         = get_initializer());

    explicit auto_list(component_type& tmp, scope::config = scope::get_default(),
                       bool            report_at_exit = settings::destructor_report());
    ~auto_list();

    // copy and move
    auto_list(const this_type&)     = default;
    auto_list(this_type&&) noexcept = default;
    auto_list& operator=(const auto_list&) = default;
    auto_list& operator=(auto_list&&) noexcept = default;

    static constexpr std::size_t size() { return component_type::size(); }

public:
    // public member functions
    component_type&       get_component() { return m_temporary; }
    const component_type& get_component() const { return m_temporary; }

    operator component_type&() { return m_temporary; }
    operator const component_type&() const { return m_temporary; }

    // partial interface to underlying component_list
    void push()
    {
        if(m_enabled)
            m_temporary.push();
    }
    void pop()
    {
        if(m_enabled)
            m_temporary.pop();
    }
    template <typename... Args>
    void measure(Args&&... args)
    {
        if(m_enabled)
            m_temporary.measure(std::forward<Args>(args)...);
    }
    template <typename... Args>
    void sample(Args&&... args)
    {
        if(m_enabled)
            m_temporary.sample(std::forward<Args>(args)...);
    }
    template <typename... Args>
    void start(Args&&... args)
    {
        if(m_enabled)
            m_temporary.start(std::forward<Args>(args)...);
    }
    template <typename... Args>
    void stop(Args&&... args)
    {
        if(m_enabled)
            m_temporary.stop(std::forward<Args>(args)...);
    }
    template <typename... Args>
    void assemble(Args&&... args)
    {
        if(m_enabled)
            m_temporary.assemble(std::forward<Args>(args)...);
    }
    template <typename... Args>
    void derive(Args&&... args)
    {
        if(m_enabled)
            m_temporary.derive(std::forward<Args>(args)...);
    }
    template <typename... Args>
    void mark(Args&&... args)
    {
        if(m_enabled)
            m_temporary.mark(std::forward<Args>(args)...);
    }
    template <typename... Args>
    void mark_begin(Args&&... args)
    {
        if(m_enabled)
            m_temporary.mark_begin(std::forward<Args>(args)...);
    }
    template <typename... Args>
    void mark_end(Args&&... args)
    {
        if(m_enabled)
            m_temporary.mark_end(std::forward<Args>(args)...);
    }
    template <typename... Args>
    void store(Args&&... args)
    {
        if(m_enabled)
            m_temporary.store(std::forward<Args>(args)...);
    }
    template <typename... Args>
    void audit(Args&&... args)
    {
        if(m_enabled)
            m_temporary.audit(std::forward<Args>(args)...);
    }
    template <typename... Args>
    void add_secondary(Args&&... args)
    {
        if(m_enabled)
            m_temporary.add_secondary(std::forward<Args>(args)...);
    }
    template <template <typename> class OpT, typename... Args>
    void invoke(Args&&... _args)
    {
        if(m_enabled)
            m_temporary.template invoke<OpT>(std::forward<Args>(_args)...);
    }
    template <typename... Args>
    auto get(Args&&... args) const
    {
        return m_temporary.get(std::forward<Args>(args)...);
    }
    template <typename... Args>
    auto get_labeled(Args&&... args) const
    {
        return m_temporary.get_labeled(std::forward<Args>(args)...);
    }

    TIMEMORY_NODISCARD bool enabled() const { return m_enabled; }
    void                    report_at_exit(bool val) { m_report_at_exit = val; }
    TIMEMORY_NODISCARD bool report_at_exit() const { return m_report_at_exit; }

    TIMEMORY_NODISCARD bool store() const { return m_temporary.store(); }
    data_type&              data() { return m_temporary.data(); }
    const data_type&        data() const { return m_temporary.data(); }
    TIMEMORY_NODISCARD int64_t laps() const { return m_temporary.laps(); }
    auto                       key() const { return m_temporary.key(); }
    TIMEMORY_NODISCARD uint64_t hash() const { return m_temporary.hash(); }
    void                        rekey(const string_t& _key) { m_temporary.rekey(_key); }

public:
    template <typename Tp>
    decltype(auto) get()
    {
        return m_temporary.template get<Tp>();
    }

    template <typename Tp>
    decltype(auto) get() const
    {
        return m_temporary.template get<Tp>();
    }

    void get(void*& ptr, size_t _hash) { m_temporary.get(ptr, _hash); }

    template <typename T>
    auto get_component()
        -> decltype(std::declval<component_type>().template get_component<T>())
    {
        return m_temporary.template get_component<T>();
    }

    template <typename Tp, typename... Args>
    void init(Args&&... _args)
    {
        m_temporary.template init<Tp>(std::forward<Args>(_args)...);
    }

    template <typename... Tp, typename... Args>
    void initialize(Args&&... _args)
    {
        m_temporary.template initialize<Tp...>(std::forward<Args>(_args)...);
    }

    template <typename... Tail>
    void disable()
    {
        m_temporary.template disable<Tail...>();
    }

public:
    friend std::ostream& operator<<(std::ostream& os, const this_type& obj)
    {
        os << obj.m_temporary;
        return os;
    }

private:
    bool            m_enabled        = true;
    bool            m_report_at_exit = false;
    component_type  m_temporary;
    component_type* m_reference_object = nullptr;
};

//======================================================================================//

template <typename... Types>
template <typename Func>
auto_list<Types...>::auto_list(const string_t& key, scope::config _scope,
                               bool report_at_exit, const Func& _func)
: m_enabled(settings::enabled())
, m_report_at_exit(report_at_exit)
, m_temporary(key, m_enabled, _scope)
, m_reference_object(nullptr)
{
    if(m_enabled)
    {
        _func(*this);
        m_temporary.start();
    }
}

//--------------------------------------------------------------------------------------//

template <typename... Types>
template <typename Func>
auto_list<Types...>::auto_list(const captured_location_t& loc, scope::config _scope,
                               bool report_at_exit, const Func& _func)
: m_enabled(settings::enabled())
, m_report_at_exit(report_at_exit)
, m_temporary(loc, m_enabled, _scope)
, m_reference_object(nullptr)
{
    if(m_enabled)
    {
        _func(*this);
        m_temporary.start();
    }
}

//--------------------------------------------------------------------------------------//

template <typename... Types>
template <typename Func>
auto_list<Types...>::auto_list(size_t _hash, scope::config _scope, bool report_at_exit,
                               const Func& _func)
: m_enabled(settings::enabled())
, m_report_at_exit(report_at_exit)
, m_temporary(_hash, m_enabled, _scope)
, m_reference_object(nullptr)
{
    if(m_enabled)
    {
        _func(*this);
        m_temporary.start();
    }
}

//--------------------------------------------------------------------------------------//

template <typename... Types>
auto_list<Types...>::auto_list(component_type& tmp, scope::config _scope,
                               bool report_at_exit)
: m_report_at_exit(report_at_exit)
, m_temporary(tmp.clone(true, _scope))
, m_reference_object(&tmp)
{
    if(m_enabled)
    {
        m_temporary.start();
    }
}

//--------------------------------------------------------------------------------------//

template <typename... Types>
auto_list<Types...>::~auto_list()
{
    if(m_enabled)
    {
        // stop the timer
        m_temporary.stop();

        // report timer at exit
        if(m_report_at_exit)
        {
            std::stringstream ss;
            ss << m_temporary;
            if(ss.str().length() > 0)
                std::cout << ss.str() << std::endl;
        }

        if(m_reference_object)
        {
            *m_reference_object += m_temporary;
        }
    }
}

//======================================================================================//

template <typename... Types>
auto
get(const auto_list<Types...>& _obj)
{
    return get(_obj.get_component());
}

//--------------------------------------------------------------------------------------//

template <typename... Types>
auto
get_labeled(const auto_list<Types...>& _obj)
{
    return get_labeled(_obj.get_component());
}

//======================================================================================//

}  // namespace tim

//======================================================================================//

//--------------------------------------------------------------------------------------//
// variadic versions
//
#if !defined(TIMEMORY_VARIADIC_BLANK_AUTO_LIST)
#    define TIMEMORY_VARIADIC_BLANK_AUTO_LIST(tag, ...)                                  \
        using _TIM_TYPEDEF(__LINE__) = ::tim::auto_list<__VA_ARGS__>;                    \
        TIMEMORY_BLANK_MARKER(_TIM_TYPEDEF(__LINE__), tag);
#endif

#if !defined(TIMEMORY_VARIADIC_BASIC_AUTO_LIST)
#    define TIMEMORY_VARIADIC_BASIC_AUTO_LIST(tag, ...)                                  \
        using _TIM_TYPEDEF(__LINE__) = ::tim::auto_list<__VA_ARGS__>;                    \
        TIMEMORY_BASIC_MARKER(_TIM_TYPEDEF(__LINE__), tag);
#endif

#if !defined(TIMEMORY_VARIADIC_AUTO_LIST)
#    define TIMEMORY_VARIADIC_AUTO_LIST(tag, ...)                                        \
        using _TIM_TYPEDEF(__LINE__) = ::tim::auto_list<__VA_ARGS__>;                    \
        TIMEMORY_MARKER(_TIM_TYPEDEF(__LINE__), tag);
#endif

//======================================================================================//
//
//      std::get operator
//
namespace std
{
//--------------------------------------------------------------------------------------//

template <std::size_t N, typename... Types>
auto
get(tim::auto_list<Types...>& obj) -> decltype(get<N>(obj.data()))
{
    return get<N>(obj.data());
}

//--------------------------------------------------------------------------------------//

template <std::size_t N, typename... Types>
auto
get(const tim::auto_list<Types...>& obj) -> decltype(get<N>(obj.data()))
{
    return get<N>(obj.data());
}

//--------------------------------------------------------------------------------------//

template <std::size_t N, typename... Types>
auto
get(tim::auto_list<Types...>&& obj)
    -> decltype(get<N>(std::forward<tim::auto_list<Types...>>(obj).data()))
{
    using obj_type = tim::auto_list<Types...>;
    return get<N>(std::forward<obj_type>(obj).data());
}

//======================================================================================//

}  // namespace std

//======================================================================================//
