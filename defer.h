#pragma once
#include <functional>

#define SCOPEGUARD_LINENAME_CAT(name, line) name##line
#define SCOPEGUARD_LINENAME(name, line) SCOPEGUARD_LINENAME_CAT(name, line)
#define defer(callback) ScopeGuard SCOPEGUARD_LINENAME(EXIT, __LINE__)(callback)

class ScopeGuard
{
public:
	explicit ScopeGuard(std::tr1::function<void()> onExitScope) 
		: onExitScope_(onExitScope), dismissed_(false)
	{ }

	~ScopeGuard()
	{
		if(!dismissed_)
		{
			onExitScope_();
		}
	}

	void Dismiss()
	{
		dismissed_ = true;
	}

private:
	std::tr1::function<void()> onExitScope_;
	bool dismissed_;

private: // noncopyable
	ScopeGuard(ScopeGuard const&);
	ScopeGuard& operator=(ScopeGuard const&);
};