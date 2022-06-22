//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SM_OPTIONS_RUN_TO_COMPLETION_HPP
#define AWESM_SM_OPTIONS_RUN_TO_COMPLETION_HPP

namespace awesm::sm_options
{

namespace detail::tags
{
    struct run_to_completion{};
}

namespace detail::defaults
{
    struct run_to_completion
    {
        template<class Sm, class Context>
        run_to_completion(Sm& /*m*/, Context& /*ctx*/)
        {
        }

        static constexpr bool call(tags::run_to_completion /*tag*/, long)
        {
            return true;
        }
    };
}

template<bool Enabled>
class run_to_completion
{
    public:
        template<class Sm, class Context>
        run_to_completion(Sm& /*sm*/, Context& /*ctx*/)
        {
        }

        static constexpr bool call(detail::tags::run_to_completion /*tag*/, int)
        {
            return Enabled;
        }
};

} //namespace

#endif
