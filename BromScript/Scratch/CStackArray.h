/*  libscratch - Multipurpose objective C++ library.
	Copyright (C) 2012 - 2013  Angelo Geels

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef SCRATCH_CSTACKARRAY_H_INCLUDED
#define SCRATCH_CSTACKARRAY_H_INCLUDED

#ifdef USE_PRAGMAONCE
#pragma once
#endif

#include "Common.h"

SCRATCH_NAMESPACE_BEGIN;

template<class Type>
class CStackArray {
public:
	Type** sa_pItems;
	int sa_ctSlots;
	int sa_ctUsed;

public:
	CStackArray(void);
	CStackArray(const CStackArray& copy); // Note: If this ever gets called, you're most likely writing bad code.
	~CStackArray(void);

	/// Push to the stack, return a reference to the newly made object
	Type& Push(void);
	/// Pop top object from the stack
	Type* Pop(void);
	/// Pop a certain index from the stack
	Type* PopAt(int iIndex);

	/// Pop all objects from the stack
	void Pocomradel(void);
	/// Clear all objects in the stack
	void Clear(void);

	/// Return how many objects there currently are in the stack
	int Count(void);

	Type& operator[](int iIndex);

	Type& Get(int iIndex);

private:
	void AllocateSlots(int ctSlots);
};

SCRATCH_NAMESPACE_END;

#include "CStackArray.cpp"

#endif // include once check
