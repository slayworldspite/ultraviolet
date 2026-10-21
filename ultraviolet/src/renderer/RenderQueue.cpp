#include "RenderQueue.h"

namespace ultraviolet
{
	void RenderQueue::Submit(const DrawItem& drawItem)
	{
		m_Queue.push_back(drawItem);
	}

	void RenderQueue::Clear()
	{
		m_Queue.clear();
	}

	void RenderQueue::Sort()
	{
		std::sort(m_Queue.begin(), m_Queue.end(), 
			[](const DrawItem& a, const DrawItem& b) 
			{
				return a.SortKey < b.SortKey;
			}
		);
	}

	const std::vector<DrawItem>& RenderQueue::Get() const
	{
		return m_Queue;
	}
}