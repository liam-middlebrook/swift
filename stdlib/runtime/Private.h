//===--- Private.h - Private runtime declarations --------------*- C++ -*--===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2015 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//
//
// Private declarations of the Swift runtime.
//
//===----------------------------------------------------------------------===//

#ifndef SWIFT_RUNTIME_PRIVATE_H
#define SWIFT_RUNTIME_PRIVATE_H

#include "swift/Runtime/Config.h"
#include "swift/Runtime/FastEntryPoints.h"
#include "swift/Runtime/Metadata.h"
#include "llvm/Support/Compiler.h"

namespace swift {
  struct ProtocolDescriptor;

  LLVM_LIBRARY_VISIBILITY
  extern uintptr_t ISAMask;

  extern "C" LLVM_LIBRARY_VISIBILITY
  bool _swift_classConformsToObjCProtocol(const void *theClass,
                                    const ProtocolDescriptor *theProtocol);

  extern "C" LLVM_LIBRARY_VISIBILITY LLVM_ATTRIBUTE_NORETURN
  void _swift_abortRetainUnowned(const void *object);

  extern "C" LLVM_LIBRARY_VISIBILITY
  void _swift_deallocClassInstance(HeapObject *object);

  /// Is the given value a valid alignment mask?
  static inline bool isAlignmentMask(size_t mask) {
    // mask          == xyz01111...
    // mask+1        == xyz10000...
    // mask&(mask+1) == xyz00000...
    // So this is nonzero if and only if there any bits set
    // other than an arbitrarily long sequence of low bits.
    return (mask & (mask + 1)) == 0;
  }

  /// Return the class of an object which is known to be an allocated
  /// heap object.
  static inline const ClassMetadata *_swift_getClassOfAllocated(const void *object) {
    // Load the isa field.
    uintptr_t bits = *reinterpret_cast<const uintptr_t*>(object);
    // Apply the mask.
    bits &= ISAMask;
    // The result is a class pointer.
    return reinterpret_cast<const ClassMetadata *>(bits);
  }

#if SWIFT_OBJC_INTEROP
  /// Is the given value an Objective-C tagged pointer?
  static inline bool isObjCTaggedPointer(const void *object) {
    return (((uintptr_t) object) & heap_object_abi::ObjCReservedBitsMask);
  }

  static inline bool isObjCTaggedPointerOrNull(const void *object) {
    return object == nullptr || isObjCTaggedPointer(object);
  }

  LLVM_LIBRARY_VISIBILITY
  const ClassMetadata *_swift_getClass(const void *object);
#else
  static inline const ClassMetadata *_swift_getClass(const void *object) {
    return _swift_getClassOfAllocated(object);
  }  
#endif

  static inline
  const ClassMetadata *_swift_getSuperclass(const ClassMetadata *theClass) {
    // This is kosher under the ABI.
    return theClass->SuperClass;
  }

  LLVM_LIBRARY_VISIBILITY
  bool usesNativeSwiftReferenceCounting(const ClassMetadata *theClass);

  extern "C" bool _swift_usesNativeSwiftReferenceCounting_nonNull(
    const void *object);

  extern "C" bool _swift_isUniquelyReferencedNative_nonNull(const void* object);
} // end namespace swift

#endif /* SWIFT_RUNTIME_PRIVATE_H */
