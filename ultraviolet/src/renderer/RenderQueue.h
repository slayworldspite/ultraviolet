#pragma once

#include "DrawItem.h"

namespace ultraviolet
{
	class RenderQueue
	{
	public:
		void Submit(const DrawItem& drawItem);
		void Clear();
		void Sort();
		const std::vector<DrawItem>& Get() const;

	private:
		std::vector<DrawItem> m_Queue;
	};
}