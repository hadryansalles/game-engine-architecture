#pragma once

#include <atomic>

template <typename T>
class LockFreeList {
public:
	void PushFront(T data) {
		// prepare the transaction locally
		auto pNode = new Node();
		pNode->m_Data = data;
		pNode->m_pNext = m_Head;

		// retry the transaction until it succeeds
		while (!m_Head.compare_exchange_weak(pNode->m_pNext, pNode)) {}
	}

	int CountNodes() {
		int count = 0;
		Node* node = m_Head;

		while (node) {
			node = node->m_pNext;
			count++;
		}

		return count;
	}
private:
	struct Node {
		T m_Data;
		Node* m_pNext;
	};

	std::atomic<Node*> m_Head{ nullptr };
};