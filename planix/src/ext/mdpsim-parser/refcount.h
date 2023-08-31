/* -*-C++-*- */
/*
 * Reference counters.
 *
 * Copyright 2003-2005 Carnegie Mellon University and Rutgers University
 * Copyright 2007 Håkan Younes
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef REFCOUNT_H
#define REFCOUNT_H

#include <cassert>



namespace PPDDL {

/* ====================================================================== */
/* RCObject */

/*
 * An object with a reference counter.
 */
struct RCObject {
  /* Increases the reference count for the given object. */
  static void ref(const RCObject* o) {
    if (o != 0) {
      o->ref_count_++;
    }
  }

  /* Decreases the reference count for the given object. */
  static void deref(const RCObject* o) {
    if (o != 0) {
      assert(o->ref_count_ > 0);
      o->ref_count_--;
    }
  }

  /* Decreases the reference count for the given object and deletes it
     if the reference count becomes zero. */
  static void destructive_deref(const RCObject* o) {
    if (o != 0) {
      assert(o->ref_count_ > 0);
      o->ref_count_--;
      if (o->ref_count_ == 0) {
        delete o;
      }
    }
  }

  static unsigned long refCount(RCObject const& o) {
    return o.ref_count_;
  }

  static void deleteIfZeroCount(RCObject const& o) {
    if (o.ref_count_ == 0) delete &o;
  }

  /* Deletes this object. */
  virtual ~RCObject() {}

 protected:
  /* Constructs an object with a reference counter. */
  RCObject() : ref_count_(0) {}

  /* Copy constructor. */
  RCObject(const RCObject& o) : ref_count_(0) {}

 private:
  /* Reference counter. */
  mutable unsigned long ref_count_;
};


}  // namespace PPDDL
#endif /* REFCOUNT_H */
