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

/** \file "timemory/variadic/auto_bundle.hpp"
 */

#pragma once

#include "timemory/general/source_location.hpp"
#include "timemory/mpl/filters.hpp"
#include "timemory/settings/declaration.hpp"
#include "timemory/utility/macros.hpp"
#include "timemory/utility/utility.hpp"
#include "timemory/variadic/component_bundle.hpp"
#include "timemory/variadic/macros.hpp"
#include "timemory/variadic/types.hpp"

#include <cstdint>
#include <string>

namespace tim
{
//--------------------------------------------------------------------------------------//
/// \class tim::auto_bundle
/// \tparam Tag unique identifying type for the bundle which when \ref
/// tim::trait::is_available<Tag> is false at compile-time or \ref
/// tim::trait::runtime_enabled<Tag>() is false at runtime, then none of the components
/// will be collected
/// \tparam Types Specification of the component types to bundle together
///
/// \brief This is a variadic component wrapper which combines the features of \ref
/// tim::auto_tuple<T...> and \ref tim::auto_list<U..>. The "T" types (compile-time fixed,
/// allocated on stack) should be specified as usual, the "U" types (runtime-time
/// optional, allocated on the heap) should be specified as a pointer. Initialization
/// of the optional types is similar to \ref tim::auto_list<U...> but no environment
/// variable is built-in since, ideally, this environment variable should be customized
/// based on the \ref Tag template parameter.
///
/// \code{.cpp}
///
/// // dummy type identifying the context
/// struct FooApi {};
///
/// using bundle_t = tim::auto_bundle<FooApi, wall_clock, cpu_clock*>;
///
/// void foo_init() // user initialization routine
/// {
///     bundle_t::get_initializer() = [](bundle_t& b)
///     {
///         static auto env_enum = tim::enumerate_components(
///             tim::delimit(tim::get_env<string_t>("FOO_COMPONENTS", "wall_clock")));
///
///         :im::initialize(b, env_enum);
///};
///     };
/// }
/// void bar()
/// {
///     // will record whichever components are specified by "FOO_COMPONENT" in
///     // environment, which "wall_clock" as the default
///
///     auto bar = bundle_t("foo");
///     // ...
/// }
///
/// int main(int argc, char** argv)
/// {
///     tim::timemory_init(argc, argv);
///
///     foo_init();
///
///     bar();
///
///     tim::timemory_finalize();
/// }
/// \endcode
///
/// The above code will record wall-clock, cpu-clock, and peak-rss. The intermediate
/// storage will happen on the stack and when the destructor is called, it will add itself
/// to the call-graph
///

template <typename Tag, typename... Types>
class auto_bundle<Tag, Types...>
: public concepts::wrapper
, public concepts::variadic
, public concepts::auto_wrapper
, public concepts::mixed_wrapper
, public concepts::tagged
{
    static_assert(concepts::is_api<Tag>::value,
                  "Error! The first template parameter of an 'auto_bundle' must "
                  "statisfy the 'is_api' concept");

public:
    using this_type           = auto_bundle<Tag, Types...>;
    using base_type           = component_bundle<Tag, Types...>;
    using auto_type           = this_type;
    using component_type      = typename base_type::component_type;
    using tuple_type          = typename component_type::tuple_type;
    using data_type           = typename component_type::data_type;
    using sample_type         = typename component_type::sample_type;
    using bundle_type         = typename component_type::bundle_type;
    using string_t            = std::string;
    using initializer_type    = std::function<void(this_type&)>;
    using captured_location_t = typename component_type::captured_location_t;
    using type       = convert_t<typename component_type::type, auto_bundle<Tag>>;
    using value_type = component_type;

    static constexpr bool has_gotcha_v      = component_type::has_gotcha_v;
    static constexpr bool has_user_bundle_v = component_type::has_user_bundle_v;

public:
    template <typename T, typename... U>
    using quirk_config = mpl::impl::quirk_config<T, type_list<Types...>, U...>;

public:
    //
    //----------------------------------------------------------------------------------//
    //
    static void init_storage() { component_type::init_storage(); }
    //
    //----------------------------------------------------------------------------------//
    //
    static initializer_type& get_initializer()
    {
        static initializer_type _instance = [](this_type&) {};
        return _instance;
    }
    //
    //----------------------------------------------------------------------------------//
    //
    static initializer_type& get_finalizer()
    {
        static initializer_type _instance = [](this_type&) {};
        return _instance;
    }

public:
    template <typename... T, typename Init = initializer_type>
    explicit auto_bundle(const string_t&, quirk::config<T...>,
                         const Init& = this_type::get_initializer());

    template <typename... T, typename Init = initializer_type>
    explicit auto_bundle(const captured_location_t&, quirk::config<T...>,
                         const Init& = this_type::get_initializer());

    template <typename Init = initializer_type>
    explicit auto_bundle(const string_t&, scope::config = scope::get_default(),
                         bool report_at_exit = settings::destructor_report(),
                         const Init&         = this_type::get_initializer());

    template <typename Init = initializer_type>
    explicit auto_bundle(const captured_location_t&, scope::config = scope::get_default(),
                         bool report_at_exit = settings::destructor_report(),
                         const Init&         = this_type::get_initializer());

    template <typename Init = initializer_type>
    explicit auto_bundle(size_t, scope::config = scope::get_default(),
                         bool report_at_exit = settings::destructor_report(),
                         const Init&         = this_type::get_initializer());

    explicit auto_bundle(component_type& tmp, scope::config = scope::get_default(),
                         bool            report_at_exit = settings::destructor_report());

    template <typename Init, typename Arg, typename... Args>
    auto_bundle(const string_t&, bool store, scope::config _scope, const Init&, Arg&&,
                Args&&...);

    template <typename Init, typename Arg, typename... Args>
    auto_bundle(const captured_location_t&, bool store, scope::config _scope, const Init&,
                Arg&&, Args&&...);

    template <typename Init, typename Arg, typename... Args>
    auto_bundle(size_t, bool store, scope::config _scope, const Init&, Arg&&, Args&&...);

    ~auto_bundle();

    // copy and move
    auto_bundle(const auto_bundle&)     = default;
    auto_bundle(auto_bundle&&) noexcept = default;
    auto_bundle& operator=(const auto_bundle&) = default;
    auto_bundle& operator=(auto_bundle&&) noexcept = default;

    static constexpr std::size_t size() { return component_type::size(); }

public:
    // public member functions
    component_type&       get_component() { return m_temporary; }
    const component_type& get_component() const { return m_temporary; }

    /// implicit conversion to underlying component_type
    operator component_type&() { return m_temporary; }
    /// implicit conversion to const ref of underlying component_type
    operator const component_type&() const { return m_temporary; }

    /// query the number of (compile-time) fixed components
    static constexpr auto fixed_count() { return component_type::fixed_count(); }
    /// query the number of (run-time) optional components
    static constexpr auto optional_count() { return component_type::optional_count(); }
    /// count number of active components in an instance
    auto count() { return (m_enabled) ? m_temporary.count() : 0; }
    /// push components into call-stack storage
    void push()
    {
        if(m_enabled)
            m_temporary.push();
    }
    /// pop components off call-stack storage
    void pop()
    {
        if(m_enabled)
            m_temporary.pop();
    }
    /// execute a measurement
    template <typename... Args>
    void measure(Args&&... args)
    {
        if(m_enabled)
            m_temporary.measure(std::forward<Args>(args)...);
    }
    /// execute a sample
    template <typename... Args>
    void sample(Args&&... args)
    {
        if(m_enabled)
            m_temporary.sample(std::forward<Args>(args)...);
    }
    /// invoke start member function on all components
    template <typename... Args>
    void start(Args&&... args)
    {
        if(m_enabled)
            m_temporary.start(std::forward<Args>(args)...);
    }
    /// invoke stop member function on all components
    template <typename... Args>
    void stop(Args&&... args)
    {
        if(m_enabled)
            m_temporary.stop(std::forward<Args>(args)...);
    }
    /// invoke assemble member function on all components to determine if measurements can
    /// be derived from other components in a bundle
    template <typename... Args>
    void assemble(Args&&... args)
    {
        if(m_enabled)
            m_temporary.assemble(std::forward<Args>(args)...);
    }
    /// invoke derive member function on all components to extract measurements from other
    /// components in the bundle
    template <typename... Args>
    void derive(Args&&... args)
    {
        if(m_enabled)
            m_temporary.derive(std::forward<Args>(args)...);
    }
    /// invoke mark member function on all components
    template <typename... Args>
    void mark(Args&&... args)
    {
        if(m_enabled)
            m_temporary.mark(std::forward<Args>(args)...);
    }
    /// invoke mark_begin member function on all components
    template <typename... Args>
    void mark_begin(Args&&... args)
    {
        if(m_enabled)
            m_temporary.mark_begin(std::forward<Args>(args)...);
    }
    /// invoke mark_begin member function on all components
    template <typename... Args>
    void mark_end(Args&&... args)
    {
        if(m_enabled)
            m_temporary.mark_end(std::forward<Args>(args)...);
    }
    /// invoke store member function on all components
    template <typename... Args>
    void store(Args&&... args)
    {
        if(m_enabled)
            m_temporary.store(std::forward<Args>(args)...);
    }
    /// invoke audit member function on all components
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
    /// invoke the provided operation on all components
    template <template <typename> class OpT, typename... Args>
    void invoke(Args&&... _args)
    {
        if(m_enabled)
            m_temporary.template invoke<OpT>(std::forward<Args>(_args)...);
    }
    /// invoke get member function on all components to get their data
    template <typename... Args>
    auto get(Args&&... args) const
    {
        return m_temporary.get(std::forward<Args>(args)...);
    }
    /// invoke get member function on all components to get data labeled with component
    /// name
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
    template <typename Tp, typename... Args>
    auto init(Args&&... args)
    {
        m_temporary.template init<Tp>(std::forward<Args>(args)...);
    }

    template <typename... Tp, typename... Args>
    auto initialize(Args&&... args)
    {
        m_temporary.template initialize<Tp...>(std::forward<Args>(args)...);
    }

    template <typename... Tail>
    void disable()
    {
        m_temporary.template disable<Tail...>();
    }

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

    void get(void*& ptr, size_t hash) { m_temporary.get(ptr, hash); }

    template <typename T>
    auto get_component()
        -> decltype(std::declval<component_type>().template get_component<T>())
    {
        return m_temporary.template get_component<T>();
    }

protected:
    template <typename Func>
    void internal_init(Func&& _init)
    {
        if(m_enabled)
            _init(*this);
    }

    template <typename Func, typename Arg, typename... Args>
    void internal_init(Func&& _init, Arg&& _arg, Args&&... _args)
    {
        if(m_enabled)
        {
            _init(*this);
            m_temporary.construct(std::forward<Arg>(_arg), std::forward<Args>(_args)...);
        }
    }

public:
    friend std::ostream& operator<<(std::ostream& os, const this_type& obj)
    {
        os << obj.m_temporary;
        return os;
    }

protected:
    bool            m_enabled          = true;
    bool            m_report_at_exit   = false;
    component_type* m_reference_object = nullptr;
    component_type  m_temporary;
};

//--------------------------------------------------------------------------------------//

template <typename Tag, typename... Types>
template <typename... T, typename Init>
auto_bundle<Tag, Types...>::auto_bundle(const string_t& key, quirk::config<T...> _config,
                                        const Init& init_func)
: m_enabled(settings::enabled())
, m_report_at_exit(quirk_config<quirk::exit_report, T...>::value)
, m_reference_object(nullptr)
, m_temporary(key, m_enabled, _config)
{
    if(m_enabled)
    {
        internal_init(init_func);
        IF_CONSTEXPR(!quirk_config<quirk::explicit_start, T...>::value)
        {
            m_temporary.start();
        }
    }
}

//--------------------------------------------------------------------------------------//

template <typename Tag, typename... Types>
template <typename... T, typename Init>
auto_bundle<Tag, Types...>::auto_bundle(const captured_location_t& loc,
                                        quirk::config<T...>        _config,
                                        const Init&                init_func)
: m_enabled(settings::enabled())
, m_report_at_exit(quirk_config<quirk::exit_report, T...>::value)
, m_reference_object(nullptr)
, m_temporary(loc, m_enabled, _config)
{
    if(m_enabled)
    {
        internal_init(init_func);
        IF_CONSTEXPR(!quirk_config<quirk::explicit_start, T...>::value)
        {
            m_temporary.start();
        }
    }
}

//--------------------------------------------------------------------------------------//

template <typename Tag, typename... Types>
template <typename Init>
auto_bundle<Tag, Types...>::auto_bundle(const string_t& key, scope::config _scope,
                                        bool report_at_exit, const Init& init_func)
: m_enabled(settings::enabled())
, m_report_at_exit(report_at_exit || quirk_config<quirk::exit_report>::value)
, m_reference_object(nullptr)
, m_temporary(key, m_enabled, _scope)
{
    if(m_enabled)
    {
        internal_init(init_func);
        IF_CONSTEXPR(!quirk_config<quirk::explicit_start>::value) { m_temporary.start(); }
    }
}

//--------------------------------------------------------------------------------------//

template <typename Tag, typename... Types>
template <typename Init>
auto_bundle<Tag, Types...>::auto_bundle(const captured_location_t& loc,
                                        scope::config _scope, bool report_at_exit,
                                        const Init& init_func)
: m_enabled(settings::enabled())
, m_report_at_exit(report_at_exit || quirk_config<quirk::exit_report>::value)
, m_reference_object(nullptr)
, m_temporary(loc, m_enabled, _scope)
{
    if(m_enabled)
    {
        internal_init(init_func);
        IF_CONSTEXPR(!quirk_config<quirk::explicit_start>::value) { m_temporary.start(); }
    }
}

//--------------------------------------------------------------------------------------//

template <typename Tag, typename... Types>
template <typename Init>
auto_bundle<Tag, Types...>::auto_bundle(size_t hash, scope::config _scope,
                                        bool report_at_exit, const Init& init_func)
: m_enabled(settings::enabled())
, m_report_at_exit(report_at_exit || quirk_config<quirk::exit_report>::value)
, m_reference_object(nullptr)
, m_temporary(hash, m_enabled, _scope)
{
    if(m_enabled)
    {
        internal_init(init_func);
        IF_CONSTEXPR(!quirk_config<quirk::explicit_start>::value) { m_temporary.start(); }
    }
}

//--------------------------------------------------------------------------------------//

template <typename Tag, typename... Types>
auto_bundle<Tag, Types...>::auto_bundle(component_type& tmp, scope::config _scope,
                                        bool report_at_exit)
: m_report_at_exit(report_at_exit || quirk_config<quirk::exit_report>::value)
, m_reference_object(&tmp)
, m_temporary(component_type(tmp.clone(true, _scope)))
{
    if(m_enabled)
    {
        IF_CONSTEXPR(!quirk_config<quirk::explicit_start>::value) { m_temporary.start(); }
    }
}

//--------------------------------------------------------------------------------------//

template <typename Tag, typename... Types>
template <typename Init, typename Arg, typename... Args>
auto_bundle<Tag, Types...>::auto_bundle(const string_t& key, bool store,
                                        scope::config _scope, const Init& init_func,
                                        Arg&& arg, Args&&... args)
: m_enabled(store && settings::enabled())
, m_report_at_exit(settings::destructor_report() ||
                   quirk_config<quirk::exit_report>::value)
, m_reference_object(nullptr)
, m_temporary(key, m_enabled, _scope)
{
    if(m_enabled)
    {
        internal_init(init_func, std::forward<Arg>(arg), std::forward<Args>(args)...);
        IF_CONSTEXPR(!quirk_config<quirk::explicit_start>::value) { m_temporary.start(); }
    }
}

//--------------------------------------------------------------------------------------//

template <typename Tag, typename... Types>
template <typename Init, typename Arg, typename... Args>
auto_bundle<Tag, Types...>::auto_bundle(const captured_location_t& loc, bool store,
                                        scope::config _scope, const Init& init_func,
                                        Arg&& arg, Args&&... args)
: m_enabled(store && settings::enabled())
, m_report_at_exit(settings::destructor_report() ||
                   quirk_config<quirk::exit_report>::value)
, m_reference_object(nullptr)
, m_temporary(loc, m_enabled, _scope)
{
    if(m_enabled)
    {
        internal_init(init_func, std::forward<Arg>(arg), std::forward<Args>(args)...);
        IF_CONSTEXPR(!quirk_config<quirk::explicit_start>::value) { m_temporary.start(); }
    }
}

//--------------------------------------------------------------------------------------//

template <typename Tag, typename... Types>
template <typename Init, typename Arg, typename... Args>
auto_bundle<Tag, Types...>::auto_bundle(size_t hash, bool store, scope::config _scope,
                                        const Init& init_func, Arg&& arg, Args&&... args)
: m_enabled(store && settings::enabled())
, m_report_at_exit(settings::destructor_report() ||
                   quirk_config<quirk::exit_report>::value)
, m_reference_object(nullptr)
, m_temporary(hash, m_enabled, _scope)
{
    if(m_enabled)
    {
        internal_init(init_func, std::forward<Arg>(arg), std::forward<Args>(args)...);
        IF_CONSTEXPR(!quirk_config<quirk::explicit_start>::value) { m_temporary.start(); }
    }
}

//--------------------------------------------------------------------------------------//

template <typename Tag, typename... Types>
auto_bundle<Tag, Types...>::~auto_bundle()
{
    IF_CONSTEXPR(!quirk_config<quirk::explicit_stop>::value)
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
}
//
//======================================================================================//
//
template <typename Tag, typename... Types>
auto
get(const auto_bundle<Tag, Types...>& _obj)
{
    return get(_obj.get_component());
}

//--------------------------------------------------------------------------------------//

template <typename Tag, typename... Types>
auto
get_labeled(const auto_bundle<Tag, Types...>& _obj)
{
    return get_labeled(_obj.get_component());
}

//======================================================================================//

}  // namespace tim

//======================================================================================//
//
// variadic versions
//
#if !defined(TIMEMORY_VARIADIC_BLANK_AUTO_BUNDLE)
#    define TIMEMORY_VARIADIC_BLANK_AUTO_BUNDLE(tag, ...)                                \
        using _TIM_TYPEDEF(__LINE__) = ::tim::auto_bundle<__VA_ARGS__>;                  \
        TIMEMORY_BLANK_MARKER(_TIM_TYPEDEF(__LINE__), tag);
#endif

#if !defined(TIMEMORY_VARIADIC_BASIC_AUTO_BUNDLE)
#    define TIMEMORY_VARIADIC_BASIC_AUTO_BUNDLE(tag, ...)                                \
        using _TIM_TYPEDEF(__LINE__) = ::tim::auto_bundle<__VA_ARGS__>;                  \
        TIMEMORY_BASIC_MARKER(_TIM_TYPEDEF(__LINE__), tag);
#endif

#if !defined(TIMEMORY_VARIADIC_AUTO_BUNDLE)
#    define TIMEMORY_VARIADIC_AUTO_BUNDLE(tag, ...)                                      \
        using _TIM_TYPEDEF(__LINE__) = ::tim::auto_bundle<__VA_ARGS__>;                  \
        TIMEMORY_MARKER(_TIM_TYPEDEF(__LINE__), tag);
#endif

//======================================================================================//
//
//      std::get operator
//
//======================================================================================//
//
namespace std
{
//
//--------------------------------------------------------------------------------------//
//
template <std::size_t N, typename Tag, typename... Types>
typename std::tuple_element<N, std::tuple<Types...>>::type&
get(tim::auto_bundle<Tag, Types...>& obj)
{
    return get<N>(obj.data());
}
//
//--------------------------------------------------------------------------------------//
//
template <std::size_t N, typename Tag, typename... Types>
const typename std::tuple_element<N, std::tuple<Types...>>::type&
get(const tim::auto_bundle<Tag, Types...>& obj)
{
    return get<N>(obj.data());
}
//
//--------------------------------------------------------------------------------------//
//
template <std::size_t N, typename Tag, typename... Types>
auto
get(tim::auto_bundle<Tag, Types...>&& obj)
    -> decltype(get<N>(std::forward<tim::auto_bundle<Tag, Types...>>(obj).data()))
{
    using obj_type = tim::auto_bundle<Tag, Types...>;
    return get<N>(std::forward<obj_type>(obj).data());
}
//
//--------------------------------------------------------------------------------------//
//
}  // namespace std
//
//======================================================================================//
