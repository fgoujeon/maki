//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_TRANSITION_TABLE_HPP
#define FGFSM_TRANSITION_TABLE_HPP

namespace fgfsm
{

template<class... Rows>
class transition_table
{
    public:
        template<class... RowArgs>
        transition_table(RowArgs&&... rows):
            rows_(rows...)
        {
        }

    private:
        template<class Configuration>
        friend class fsm;

        std::tuple<Rows...> rows_;
};

template<class... RowArgs>
transition_table(RowArgs&&...) -> transition_table<std::decay_t<RowArgs>...>;

} //namespace

#endif
