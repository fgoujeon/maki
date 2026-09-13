//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

namespace maki::detail::tlu
{

template<class AosVoid, class TList, class F>
struct MAKI_AOS_NAME(for_each_helper);

template<class AosVoid, template<class...> class TList, class... Ts, class F>
struct MAKI_AOS_NAME(for_each_helper)<AosVoid, TList<Ts...>, F>
{
    template<class... Args>
    static AosVoid call([[maybe_unused]] Args&... args)
    {
        (MAKI_AOS_CALL F::template call<Ts>(args...), ...);
    }
};

/*
Calls:
    F::call<T0>(args...);
    F::call<T1>(args...);
    ...
    F::call<TN>(args...);
*/
template<class AosVoid, class TList, class F, class... Args>
AosVoid MAKI_AOS_NAME(for_each)(Args&... args)
{
    MAKI_AOS_CALL MAKI_AOS_NAME(for_each_helper)<AosVoid, TList, F>::call(args...);
}

} //namespace
