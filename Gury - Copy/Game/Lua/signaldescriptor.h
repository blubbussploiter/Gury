#pragma once

#include "../Gury/Game/Reflection/signal.h"
#include "G3D/Table.h"

#include "signalInstance.h"

namespace RBX
{
	template<typename signal_func>
	class SignalDesc : public SignalInstance
	{
	private:

		boost::signal<signal_func>* ourSignal;

	public:

		bool _connectionEstablished;

		void connectGeneric(lua_State* L);

		SignalDesc(boost::signal<signal_func>* ourSignal, std::string name) {
			this->name = name;
			this->ourSignal = ourSignal;
		}
		SignalDesc() {
			this->ourSignal = nullptr;
		}

		RTTR_ENABLE(SignalInstance);
	};
}