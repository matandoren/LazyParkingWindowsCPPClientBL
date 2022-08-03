#pragma once

namespace LazyParkingWindowsCPPClientBL
{
	template <typename... Params>
	class Delegate
	{
		using CallbackType = void(*)(void*, Params...);
	private:
		struct Node
		{
			CallbackType m_Callback;
			void* m_OptionalData;
			Node* m_Next;

			Node(CallbackType i_Callback, void* i_OptionalData, Node* i_Next = nullptr)
			{
				m_Callback = i_Callback;
				m_OptionalData = i_OptionalData;
				m_Next = i_Next;
			}

			void Invoke(Params... i_Params)
			{
				m_Callback(m_OptionalData, i_Params...);
			}
		};

		Node* m_Head;

	public:
		Delegate()
		{
			m_Head = nullptr;
		}

		~Delegate()
		{
			Node* current = m_Head;
			Node* next;

			while (current != nullptr)
			{
				next = current->m_Next;
				delete current;
				current = next;
			}
		}

		void* AddCallback(CallbackType i_Callback, void* i_OptionalData)
		{
			m_Head = new Node(i_Callback, i_OptionalData, m_Head);
			return m_Head;
		}

		bool RemoveCallback(void* i_Descriptor)
		{
			Node* current = m_Head;
			Node* prev = nullptr;

			while (current != nullptr && current != i_Descriptor)
			{
				prev = current;
				current = current->m_Next;
			}

			if (current == nullptr)
			{
				return false;
			}
			else if (current == m_Head)
			{
				m_Head = m_Head->m_Next;
			}
			else
			{
				prev->m_Next = current->m_Next;
			}

			delete current;
			return true;
		}

		void Invoke(Params... i_Params)
		{
			Node* current = m_Head;

			while (current != nullptr)
			{
				current->Invoke(i_Params...);
				current = current->m_Next;
			}
		}
	};
}