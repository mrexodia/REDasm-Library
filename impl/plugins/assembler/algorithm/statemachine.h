#pragma once

#include <unordered_map>
#include <unordered_set>
#include <forward_list>
#include <functional>
#include <redasm/plugins/assembler/algorithm/state.h>

#define REGISTER_STATE(id, cb)                               m_states[id] = std::bind(cb, this, std::placeholders::_1)

namespace REDasm {

class StateMachine
{
    protected:
        typedef std::function<void(const State*)> StateCallback;

    public:
        StateMachine();
        virtual ~StateMachine() = default;
        size_t pending() const;
        bool hasNext();
        void next();

    protected:
        void enqueueState(const State& state);
        void executeState(const State& state);
        void executeState(const State* state);
        virtual bool validateState(const State& state) const;
        virtual void onNewState(const State *state) const;

    private:
        bool getNext(State* state);

    protected:
        std::unordered_map<state_t, StateCallback> m_states;

    private:
        std::forward_list<State> m_pending;
        size_t m_count;
};

} // namespace REDasm
