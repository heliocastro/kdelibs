// -*- c-basic-offset: 2 -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2001 Harri Porten (porten@kde.org)
 *  Copyright (C) 2001 Peter Kelly (pmk@post.com)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 *
 *  $Id$
 */

#include "value.h"
#include "object.h"
#include "types.h"
#include "interpreter.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "internal.h"
#include "collector.h"
#include "operations.h"
#include "error_object.h"
#include "nodes.h"

using namespace KJS;

// ------------------------------ Object ---------------------------------------

Object::Object() : Value()
{
}

Object::Object(ObjectImp *v) : Value(v)
{
}

Object::Object(const Object &v) : Value(v)
{
}

Object::~Object()
{
}

Object& Object::operator=(const Object &v)
{
  Value::operator=(v);
  return *this;
}

const ClassInfo *Object::classInfo() const
{
  return static_cast<ObjectImp*>(rep)->classInfo();
}

bool Object::inherits(const ClassInfo *cinfo) const
{
  return static_cast<ObjectImp*>(rep)->inherits(cinfo);
}

Object Object::dynamicCast(const Value &v)
{
  if (v.isNull() || v.type() != ObjectType)
    return 0;

  return static_cast<ObjectImp*>(v.imp());
}

Value Object::prototype() const
{
  return static_cast<ObjectImp*>(rep)->prototype();
}

UString Object::getClass() const
{
  return static_cast<ObjectImp*>(rep)->getClass();
}

Value Object::get(ExecState *exec, const UString &propertyName) const
{
  return static_cast<ObjectImp*>(rep)->get(exec,propertyName);
}

void Object::put(ExecState *exec, const UString &propertyName, const Value &value, int attr)
{
  static_cast<ObjectImp*>(rep)->put(exec,propertyName,value,attr);
}

bool Object::canPut(ExecState *exec, const UString &propertyName) const
{
  return static_cast<ObjectImp*>(rep)->canPut(exec,propertyName);
}

bool Object::hasProperty(ExecState *exec, const UString &propertyName, bool recursive) const
{
  return static_cast<ObjectImp*>(rep)->hasProperty(exec,propertyName,recursive);
}

bool Object::deleteProperty(ExecState *exec, const UString &propertyName)
{
  return static_cast<ObjectImp*>(rep)->deleteProperty(exec,propertyName);
}

Value Object::defaultValue(ExecState *exec, Type hint) const
{
  return static_cast<ObjectImp*>(rep)->defaultValue(exec,hint);
}

bool Object::implementsConstruct() const
{
  return static_cast<ObjectImp*>(rep)->implementsConstruct();
}

Object Object::construct(ExecState *exec, const List &args)
{
  return static_cast<ObjectImp*>(rep)->construct(exec,args);
}

bool Object::implementsCall() const
{
  return static_cast<ObjectImp*>(rep)->implementsCall();
}

Value Object::call(ExecState *exec, Object &thisObj, const List &args)
{
  return static_cast<ObjectImp*>(rep)->call(exec,thisObj,args);
}

bool Object::implementsHasInstance() const
{
  return static_cast<ObjectImp*>(rep)->implementsHasInstance();
}

Boolean Object::hasInstance(ExecState *exec, const Value &value)
{
  return static_cast<ObjectImp*>(rep)->hasInstance(exec,value);
}

const List Object::scope() const
{
  return static_cast<ObjectImp*>(rep)->scope();
}

void Object::setScope(const List &s)
{
  static_cast<ObjectImp*>(rep)->setScope(s);
}

List Object::propList(ExecState *exec, bool recursive)
{
  return static_cast<ObjectImp*>(rep)->propList(exec,recursive);
}

Value Object::internalValue() const
{
  return static_cast<ObjectImp*>(rep)->internalValue();
}

void Object::setInternalValue(const Value &v)
{
  static_cast<ObjectImp*>(rep)->setInternalValue(v);
}

// ------------------------------ ObjectImp ------------------------------------

namespace KJS {
  // ### temp - will be replaced by PropertyMap2
  class PropertyMap {
  public:
    UString name;
    ValueImp *val;
    int attribute;
    PropertyMap *next;
  };
}

ObjectImp::ObjectImp(const Object &proto)
  : _prop(0), _proto(static_cast<ObjectImp*>(proto.imp())), _internalValue(0L), _scope(0)
{
  //fprintf(stderr,"ObjectImp::ObjectImp %p %s\n",(void*)this);
  _scope = ListImp::empty();
}

ObjectImp::ObjectImp()
{
  //fprintf(stderr,"ObjectImp::ObjectImp %p %s\n",(void*)this);
  _prop = 0;
  _proto = NullImp::staticNull;
  _internalValue = 0L;
  _scope = ListImp::empty();
}

ObjectImp::~ObjectImp()
{
  //fprintf(stderr,"ObjectImp::~ObjectImp %p\n",(void*)this);
  if (_proto)
    _proto->setGcAllowed();
  if (_internalValue)
    _internalValue->setGcAllowed();
  if (_scope)
    _scope->setGcAllowed();
}

void ObjectImp::mark()
{
  //fprintf(stderr,"ObjectImp::mark() %p\n",(void*)this);
  ValueImp::mark();

  if (_proto && !_proto->marked())
    _proto->mark();

  struct PropertyMap *p = _prop;
  while (p) {
    if (p->val && !p->val->marked())
      p->val->mark();
    p = p->next;
  }

  if (_internalValue && !_internalValue->marked())
    _internalValue->mark();
  if (_scope && !_scope->marked())
    _scope->mark();
}

const ClassInfo *ObjectImp::classInfo() const
{
  return 0;
}

bool ObjectImp::inherits(const ClassInfo *info) const
{
  if (!info)
    return false;

  const ClassInfo *ci = classInfo();
  if (!ci || !info)
    return false;

  while (ci && ci != info)
    ci = ci->parentClass;

  return (ci == info);
}

Type ObjectImp::type() const
{
  return ObjectType;
}

Value ObjectImp::prototype() const
{
  return _proto;
}

void ObjectImp::setPrototype(const Value &proto)
{
  _proto = proto.imp();
}

UString ObjectImp::getClass() const
{
  const ClassInfo *ci = classInfo();
  if ( ci )
    return ci->className;
  return "Object";
}

Value ObjectImp::get(ExecState *exec, const UString &propertyName) const
{
  Object proto = Object::dynamicCast(prototype());

  if (propertyName == "__proto__") {
    // non-standard netscape extension
    if (proto.isNull())
      return Null();
    else
      return proto;
  }

  PropertyMap *pr = _prop;
  while (pr) {
    if (pr->name == propertyName) {
      return pr->val;
    }
    pr = pr->next;
  }

  if (proto.isNull())
    return Undefined();

  return proto.get(exec,propertyName);
}

// ECMA 8.6.2.2
void ObjectImp::put(ExecState *exec, const UString &propertyName,
                     const Value &value, int attr)
{
  assert(!value.isNull());
  assert(value.type() != ReferenceType);
  assert(value.type() != CompletionType);
  assert(value.type() != ListType);

  /* TODO: check for write permissions directly w/o this call */
  // putValue() is used for JS assignemnts. It passes no attribute.
  // Assume that a C++ implementation knows what it is doing
  // and let it override the canPut() check.
  if (attr == None && !canPut(exec,propertyName))
    return;

  if (propertyName == "__proto__") {
    // non-standard netscape extension
    setPrototype(value);
    return;
  }

  PropertyMap *pr;
  PropertyMap *last = 0;

  if (_prop) {
    pr = _prop;
    while (pr) {
      if (pr->name == propertyName) {
	// replace old value
	pr->val = value.imp();
	pr->attribute = attr;
	return;
      }
      last = pr;
      pr = pr->next;
    }
  }

  // add new property
  pr = new PropertyMap;
  pr->name = propertyName;
  pr->val = value.imp();
  pr->attribute = attr;
  pr->next = 0;
  if (last)
    last->next = pr;
  else
    _prop = pr;
}

// ECMA 8.6.2.3
bool ObjectImp::canPut(ExecState *exec, const UString &propertyName) const
{
  if (_prop) {
    const PropertyMap *pr = _prop;
    while (pr) {
      if (pr->name == propertyName)
	return !(pr->attribute & ReadOnly);
      pr = pr->next;
    }
  }

  Object proto = Object::dynamicCast(prototype());
  if (proto.isNull())
    return true;

  return proto.canPut(exec,propertyName);
}

// ECMA 8.6.2.4
bool ObjectImp::hasProperty(ExecState *exec, const UString &propertyName, bool recursive) const
{
  const PropertyMap *pr = _prop;
  while (pr) {
    if (pr->name == propertyName)
      return true;
    pr = pr->next;
  }

  Object proto = Object::dynamicCast(prototype());
  if (proto.isNull() || !recursive)
    return false;

  return proto.hasProperty(exec,propertyName);
}

// ECMA 8.6.2.5
bool ObjectImp::deleteProperty(ExecState */*exec*/, const UString &propertyName)
{
  PropertyMap *pr = _prop;
  PropertyMap **prev = &_prop;
  while (pr) {
    if (pr->name == propertyName) {
      if ((pr->attribute & DontDelete))
	return false;
      *prev = pr->next;
      delete pr;
      return true;
    }
    prev = &(pr->next);
    pr = pr->next;
  }
  return true;
}

// ECMA 8.6.2.6
Value ObjectImp::defaultValue(ExecState *exec, Type hint) const
{
  /* TODO String on Date object */
  if (hint != StringType && hint != NumberType)
    hint = NumberType;

  Value v;
  if (hint == StringType)
    v = get(exec,"toString");
  else
    v = get(exec,"valueOf");

  if (v.type() == ObjectType) {
    Object o = static_cast<ObjectImp*>(v.imp());
    if (o.implementsCall()) { // spec says "not primitive type" but ...
      Object thisObj = Object(const_cast<ObjectImp*>(this));
      Value def = o.call(exec,thisObj,List::empty());
      Type defType = def.type();
      if (defType == UnspecifiedType || defType == UndefinedType ||
          defType == NullType || defType == BooleanType ||
          defType == StringType || defType == NumberType) {
        return def;
      }
    }
  }

  if (hint == StringType)
    v = get(exec,"valueOf");
  else
    v = get(exec,"toString");

  if (v.type() == ObjectType) {
    Object o = static_cast<ObjectImp*>(v.imp());
    if (o.implementsCall()) { // spec says "not primitive type" but ...
      Object thisObj = Object(const_cast<ObjectImp*>(this));
      Value def = o.call(exec,thisObj,List::empty());
      Type defType = def.type();
      if (defType == UnspecifiedType || defType == UndefinedType ||
          defType == NullType || defType == BooleanType ||
          defType == StringType || defType == NumberType) {
        return def;
      }
    }
  }

  Object err = Error::create(exec, TypeError, I18N_NOOP("No default value"));
  exec->setException(err);
  return err;
}

bool ObjectImp::implementsConstruct() const
{
  return false;
}

Object ObjectImp::construct(ExecState */*exec*/, const List &/*args*/)
{
  assert(false);
  return 0;
}

bool ObjectImp::implementsCall() const
{
  return false;
}

Value ObjectImp::call(ExecState */*exec*/, Object &/*thisObj*/, const List &/*args*/)
{
  assert(false);
  return 0;
}

bool ObjectImp::implementsHasInstance() const
{
  return false;
}

Boolean ObjectImp::hasInstance(ExecState */*exec*/, const Value &/*value*/)
{
  assert(false);
  return Boolean(false);
}

const List ObjectImp::scope() const
{
  return _scope;
}

void ObjectImp::setScope(const List &s)
{
  _scope = static_cast<ListImp*>(s.imp());
}

List ObjectImp::propList(ExecState *exec, bool recursive)
{
  List list;
  if (_proto && _proto->type() == ObjectType && recursive)
    list = static_cast<ObjectImp*>(_proto)->propList(exec,recursive);

  PropertyMap *pr = _prop;
  while(pr) {
    if (!(pr->attribute & DontEnum))
      list.append(Reference(this,pr->name));
    pr = pr->next;
  }

  return list;
}

Value ObjectImp::internalValue() const
{
  return _internalValue;
}

void ObjectImp::setInternalValue(const Value &v)
{
  _internalValue = v.imp();
}

// The following functions simply call the corresponding functions in ValueImp
// but are overridden in case of future needs

Value ObjectImp::toPrimitive(ExecState *exec, Type preferredType) const
{
  return defaultValue(exec,preferredType);
}

Boolean ObjectImp::toBoolean(ExecState */*exec*/) const
{
  return true;
}

Number ObjectImp::toNumber(ExecState *exec) const
{
  Value prim = toPrimitive(exec,NumberType);
  if (exec->hadException()) // should be picked up soon in nodes.cpp
    return Number(0);
  return prim.toNumber(exec);
}

Number ObjectImp::toInteger(ExecState *exec) const
{
  return ValueImp::toInteger(exec);
}

int32_t ObjectImp::toInt32(ExecState *exec) const
{
  return ValueImp::toInt32(exec);
}

uint32_t ObjectImp::toUInt32(ExecState *exec) const
{
  return ValueImp::toUInt32(exec);
}

uint16_t ObjectImp::toUInt16(ExecState *exec) const
{
  return ValueImp::toUInt16(exec);
}

String ObjectImp::toString(ExecState *exec) const
{
  Value prim = toPrimitive(exec,StringType);
  if (exec->hadException()) // should be picked up soon in nodes.cpp
    return String("");
  return prim.toString(exec);
}

Object ObjectImp::toObject(ExecState */*exec*/) const
{
  return Object(const_cast<ObjectImp*>(this));
}


// ------------------------------ Error ----------------------------------------

const char *errorNamesArr[] = {
  I18N_NOOP("Error") // GeneralError
  I18N_NOOP("Evaluation error") // EvalError
  I18N_NOOP("Range error") // RangeError
  I18N_NOOP("Reference error") // ReferenceError
  I18N_NOOP("Syntax error") // SyntaxError
  I18N_NOOP("Type error") // TypeError
  I18N_NOOP("URI error") // URIError
};

const char **Error::errorNames = errorNamesArr;

Object Error::create(ExecState *exec, ErrorType errtype, const char *message,
                     int lineno, int sourceId)
{
  Object cons;

  switch (errtype) {
  case EvalError:
    cons = exec->interpreter()->builtinEvalError();
    break;
  case RangeError:
    cons = exec->interpreter()->builtinRangeError();
    break;
  case ReferenceError:
    cons = exec->interpreter()->builtinReferenceError();
    break;
  case SyntaxError:
    cons = exec->interpreter()->builtinSyntaxError();
    break;
  case TypeError:
    cons = exec->interpreter()->builtinTypeError();
    break;
  case URIError:
    cons = exec->interpreter()->builtinURIError();
    break;
  default:
    cons = exec->interpreter()->builtinError();
    break;
  }

  if (!message)
    message = errorNames[errtype];
  List args;
  args.append(String(message));
  Object err = Object::dynamicCast(cons.construct(exec,args));

  if (lineno != -1)
    err.put(exec, "line", Number(lineno));
  if (sourceId != -1)
    err.put(exec, "sourceId", Number(sourceId));

  return err;

/*
#ifndef NDEBUG
  const char *msg = err.get("message").toString().value().ascii();
  if (l >= 0)
      fprintf(stderr, "KJS: %s at line %d. %s\n", estr, l, msg);
  else
      fprintf(stderr, "KJS: %s. %s\n", estr, msg);
#endif

  return err;
*/
}

