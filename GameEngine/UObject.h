#ifndef _UOBJECT_H
#define _UOBJECT_H

//Additional Includes
#ifndef _CONTEXT_H
	#include "context.h"
#endif

#ifndef _STRING_H
	#include "string.h"
#endif

class UObject
{
public:
	UObject();
	UObject(const std::tstring& name);
	virtual ~UObject();

	//void* operator new(size_t size) {};
	//void operator delete(size_t size) {};

	virtual bool Initialize() {
		m_bIsInitialized = true;
		return true;
	}
	virtual bool PostInitialize() {
		m_bIsPostInitialized = true;
		return true;
	}
	virtual bool LoadContent() {
		m_bIsContentLoaded = true;
		return true;
	}
	virtual bool PostLoadContent() {
		m_bIsPostContentLoaded = true;
		return true;
	}

	virtual bool Update(Context& context) {
		return true;
	}
	//virtual bool LateUpdate(Context& context) {
	//	return true
	//}
	virtual bool Draw(Context& context) {
		return true;
	}
	virtual bool DrawUI(Context& context) {
		return true;
	}
	virtual bool Shutdown() {
		return true;	
	}

	virtual void Reset();

	void SetName(const std::tstring& newName) { m_Name = newName; };
	const std::tstring& GetName() { return m_Name; };


	bool Initialized() { return m_bIsInitialized; }
	bool isPostInitialized() { return m_bIsPostInitialized; }
	bool isContentLoaded() { return m_bIsContentLoaded; }
	bool isPostContentLoaded() {return m_bIsPostContentLoaded; }

	void setCanTick(bool ct) { m_bCanTick = ct; }
	bool CanTick() { return m_bCanTick; }

	void setCanDraw(bool cd) { m_bCanDraw = cd; }
	bool canDraw() { return m_bCanDraw; }

	void Destroy() { m_bIsDestroyed = true; }
	bool isDestroyed() {return m_bIsDestroyed; }

	void Activate() { m_bIsActivated = true; }
	void Deactivate() { m_bIsActivated = false; }
	bool isActivated() { return m_bIsActivated; }

protected:
	static int m_objectAmount;
	int m_id;

	bool m_bIsInitialized;
	bool m_bIsPostInitialized;
	
	bool m_bIsContentLoaded;
	bool m_bIsPostContentLoaded;
	
	std::tstring m_Name;

private:
	bool m_bIsActivated;
	bool m_bIsDestroyed;
	bool m_bCanTick;
	bool m_bCanDraw;
};

#endif