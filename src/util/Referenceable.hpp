/*
 * Copyright 2004-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_UTIL_REFERENCEABLE_HPP
#define HAM_UTIL_REFERENCEABLE_HPP

#include <stddef.h>
#include <stdint.h>

namespace ham
{
namespace util
{

// #pragma mark - support functions

static inline int32_t
increment_reference_count(int32_t& referenceCount)
{
	return __sync_fetch_and_add(&referenceCount, 1);
}

static inline int32_t
decrement_reference_count(int32_t& referenceCount)
{
	return __sync_fetch_and_sub(&referenceCount, 1);
}

// #pragma mark - Referenceable

class Referenceable
{
  public:
	Referenceable();
	virtual ~Referenceable();

	// acquire and release return
	// the previous ref count
	int32_t AcquireReference();
	int32_t ReleaseReference();

	int32_t CountReferences() const { return fReferenceCount; }

  protected:
	virtual void FirstReferenceAcquired();
	virtual void LastReferenceReleased();

  protected:
	int32_t fReferenceCount;
};

// #pragma mark - Reference

template<typename Type = Referenceable>
class Reference
{
  public:
	Reference()
		: fObject(nullptr)
	{
	}

	Reference(Type* object, bool alreadyHasReference = false)
		: fObject(nullptr)
	{
		SetTo(object, alreadyHasReference);
	}

	Reference(const Reference<Type>& other)
		: fObject(nullptr)
	{
		SetTo(other.fObject);
	}

	template<typename OtherType>
	Reference(const Reference<OtherType>& other)
		: fObject(nullptr)
	{
		SetTo(other.Get());
	}

	~Reference() { Unset(); }

	void SetTo(Type* object, bool alreadyHasReference = false)
	{
		if (object != nullptr && !alreadyHasReference)
			object->AcquireReference();

		Unset();

		fObject = object;
	}

	void Unset()
	{
		if (fObject) {
			fObject->ReleaseReference();
			fObject = nullptr;
		}
	}

	Type* Get() const { return fObject; }

	Type* Detach()
	{
		Type* object = fObject;
		fObject = nullptr;
		return object;
	}

	Type& operator*() const { return *fObject; }

	Type* operator->() const { return fObject; }

	operator Type*() const { return fObject; }

	Reference& operator=(const Reference<Type>& other)
	{
		SetTo(other.fObject);
		return *this;
	}

	Reference& operator=(Type* other)
	{
		SetTo(other);
		return *this;
	}

	template<typename OtherType>
	Reference& operator=(const Reference<OtherType>& other)
	{
		SetTo(other.Get());
		return *this;
	}

	bool operator==(const Reference<Type>& other) const
	{
		return fObject == other.fObject;
	}

	bool operator==(const Type* other) const { return fObject == other; }

	bool operator!=(const Reference<Type>& other) const
	{
		return fObject != other.fObject;
	}

	bool operator!=(const Type* other) const { return fObject != other; }

  private:
	Type* fObject;
};

} // namespace util
} // namespace ham

#endif // HAM_UTIL_REFERENCEABLE_HPP
