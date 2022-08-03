#pragma once
#include "Delegate.h"

namespace LazyParkingWindowsCPPClientBL
{
	template <typename Invoker, typename... Params>
	class Event
	{
	private:
		friend Invoker;
		Delegate<Params...> m_Delegate;

		void Invoke(Params... i_Params)
		{
			m_Delegate.Invoke(i_Params...);
		}

	public:
		void* AddEventHandler(void(*i_Callback)(void*, Params...), void* i_OptionalData)
		{
			return m_Delegate.AddCallback(i_Callback, i_OptionalData);
		}

		bool RemoveEventHandler(void* i_Descriptor)
		{
			return m_Delegate.RemoveCallback(i_Descriptor);
		}
	};
}