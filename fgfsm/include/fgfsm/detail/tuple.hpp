//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_DETAIL_TUPLE_HPP
#define FGFSM_DETAIL_TUPLE_HPP

namespace fgfsm::detail
{

/*
A minimal std::tuple-like container that fits our needs.
Using this instead of std::tuple improves build time.
*/

template<class... Ts>
class tuple;

template<class T, class... Ts>
class tuple<T, Ts...>: public tuple<Ts...>
{
    public:
        template<class Arg>
        tuple(Arg& arg):
            tuple<Ts...>(arg),
            obj_{arg}
        {
        }

        using tuple<Ts...>::get;

        T& get(T*)
        {
            return obj_;
        }

        const T& get(T*) const
        {
            return obj_;
        }

    private:
        T obj_;
};

template<>
class tuple<>
{
    public:
        template<class Arg>
        tuple(Arg&)
        {
        }

        void get() const
        {
        }
};

} //namespace

#endif
