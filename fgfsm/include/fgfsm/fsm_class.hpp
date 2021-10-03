//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_FSM_CLASS_HPP
#define FGFSM_FSM_CLASS_HPP

//Expand to an fgfsm internal name to avoid name conflicts with user stuff
#define _FGFSM(NAME) _fgfsm_private_##NAME

//Forward declare FSM class and related functions to break circular dependency
#define FGFSM_DECLARE_FSM_CLASS(CLASS_NAME) \
    class CLASS_NAME; \
    \
    template<class _FGFSM(Event)> \
    void process_event \
    ( \
        CLASS_NAME* _FGFSM(pfsm), \
        const _FGFSM(Event)& _FGFSM(event) \
    );

//Define FSM class and related functions to break circular dependency
#define FGFSM_DEFINE_FSM_CLASS(CLASS_NAME, IMPL_TYPE_NAME) \
    class CLASS_NAME \
    { \
        public: \
            template<class _FGFSM(Context)> \
            CLASS_NAME(_FGFSM(Context)& _FGFSM(context)): \
                _FGFSM(impl_)(_FGFSM(context)) \
            { \
            } \
 \
            template<class _FGFSM(Event)> \
            void process_event(const _FGFSM(Event)& _FGFSM(event)) \
            { \
                _FGFSM(impl_).process_event(_FGFSM(event)); \
            } \
 \
        private: \
            IMPL_TYPE_NAME _FGFSM(impl_); \
    }; \
 \
    template<class _FGFSM(Event)> \
    void process_event \
    ( \
        CLASS_NAME* const _FGFSM(pfsm), \
        const _FGFSM(Event)& _FGFSM(event) \
    ) \
    { \
        _FGFSM(pfsm)->process_event(_FGFSM(event)); \
    }

#endif
