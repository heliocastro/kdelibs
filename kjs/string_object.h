/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _STRING_OBJECT_H_
#define _STRING_OBJECT_H_

#include "object.h"
#include "function.h"

namespace KJS {

  class StringObject : public ConstructorImp {
  public:
    StringObject(const Object& proto) : ConstructorImp(proto, 1) { }
    KJSO get(const UString &p) const;
    Completion execute(const List &);
    Object construct(const List &);
  };

  class StringObjectFunc : public InternalFunctionImp {
  public:
    Completion execute(const List &);
  };

  class StringPrototype : public ObjectImp {
  public:
    StringPrototype(const Object& proto);
    KJSO get(const UString &p) const;
  };

  class StringProtoFunc : public InternalFunctionImp {
  public:
    StringProtoFunc(int i);
    Completion execute(const List &);

    enum { ToString, ValueOf, CharAt, CharCodeAt, IndexOf, LastIndexOf,
	   Substr, Substring, FromCharCode };
  private:
    int id;
  };

}; // namespace

#endif
