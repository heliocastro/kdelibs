    /*

    Copyright (C) 1999 Stefan Westerfeld
                       stefan@space.twc.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Permission is also granted to link this program with the Qt
    library, treating Qt like a library that normally accompanies the
    operating system kernel, whether or not that is in fact the case.

    Permission is also granted to link this program with the Qt
    library, treating Qt like a library that normally accompanies the
    operating system kernel, whether or not that is in fact the case.

    */

#ifndef COMMON_H
#define COMMON_H

#include "buffer.h"
#include "type.h"
#include "dispatcher.h"
#include "object.h"
#include "connection.h"
#include "objectmanager.h"
#include "idlfilereg.h"

/* define this to see what mcop transfers around */
#undef DEBUG_IO

/* define this to see what mcop messages are processed */
#undef DEBUG_MESSAGES

/* reference counting helper */

template<class T>
class ReferenceHelper
{
private:
	T *referencedObject;
public:
	ReferenceHelper() :referencedObject(0)
	{
	}
	inline ReferenceHelper(T* assignedObject) :referencedObject(assignedObject)
	{
	}
	inline ReferenceHelper<T>& operator=(T* assignedObject)
	{
		if(referencedObject)
			referencedObject->_release();

		referencedObject = assignedObject ;
		return *this;
	}
	inline T* operator->() const
	{
		assert(referencedObject);
		return referencedObject;
	}
	inline operator T*() const
	{
		return referencedObject;
	}
	~ReferenceHelper()
	{
		if(referencedObject)
		{
			referencedObject->_release();
			referencedObject = 0;
		}
	}
};

#include "core.h"

/* some marshalling helpers */

template<class T>
void freeTypeSeq(vector<T *>& sequence) {
	for(unsigned long l=0;l<sequence.size();l++)
		delete sequence[l];

	sequence.clear();
};

template<class T>
void readTypeSeq(Buffer& stream, vector<T *>& sequence) {
	freeTypeSeq(sequence);

	unsigned long l = stream.readLong();
	while(l--) sequence.push_back(new T(stream));
};

template<class T>
void writeTypeSeq(Buffer& stream, const vector<T *>& sequence) {
	stream.writeLong(sequence.size());
	for(unsigned long l=0;l<sequence.size();l++)
		sequence[l]->writeType(stream);
};

template<class T>
void writeObject(Buffer& stream, T* object) {
	if(object)
	{
		/*
	 	 * perhaps reimplement directly (without conversion to/from string)
	 	 * for more speed
	 	 */

		string s = object->_toString();

		Buffer buffer;
		buffer.fromString(s,"MCOP-Object");
		ObjectReference reference(buffer);

		object->_copyRemote();	// prevents that the object is freed for a while
		reference.writeType(stream);
	}
	else
	{
		ObjectReference null_reference;

		null_reference.serverID = "null";
		null_reference.objectID = 0;
		null_reference.writeType(stream);
	}
};

template<class T>
T *readObject(Buffer& stream) {
	ObjectReference reference(stream);

	if(reference.serverID == "null") return 0;		// null reference?
	return T::_fromReference(reference,false);
};

#endif
