/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: empty.proto */

#ifndef PROTOBUF_C_empty_2eproto__INCLUDED
#define PROTOBUF_C_empty_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

#include <grpc/grpc.h>
#include <grpc/byte_buffer_reader.h>
#include <grpc/support/alloc.h>
#include <grpc/support/log.h>
#include <grpc-c/grpc-c.h>

#if PROTOBUF_C_VERSION_NUMBER < 1000000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1003001 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif


typedef struct _Google__Protobuf__Empty Google__Protobuf__Empty;
typedef struct _Google__Protobuf__Empty google__protobuf__Empty;
#define google__protobuf__Empty__init google__protobuf__empty__init
#define google__protobuf__Empty__free_unpacked google__protobuf__empty__free_unpacked


/* --- enums --- */


/* --- messages --- */

/*
 * A generic empty message that you can re-use to avoid defining duplicated
 * empty messages in your APIs. A typical example is to use it as the request
 * or the response type of an API method. For instance:
 *     service Foo {
 *       rpc Bar(google.protobuf.Empty) returns (google.protobuf.Empty);
 *     }
 * The JSON representation for `Empty` is empty JSON object `{}`.
 */
struct  _Google__Protobuf__Empty
{
  ProtobufCMessage base;
};
#define GOOGLE__PROTOBUF__EMPTY__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&google__protobuf__empty__descriptor) \
     }


/* Google__Protobuf__Empty methods */
void   google__protobuf__empty__init
                     (Google__Protobuf__Empty         *message);
size_t google__protobuf__empty__get_packed_size
                     (const Google__Protobuf__Empty   *message);
size_t google__protobuf__empty__pack
                     (const Google__Protobuf__Empty   *message,
                      uint8_t             *out);
size_t google__protobuf__empty__pack_to_buffer
                     (const Google__Protobuf__Empty   *message,
                      ProtobufCBuffer     *buffer);
Google__Protobuf__Empty *
       google__protobuf__empty__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   google__protobuf__empty__free_unpacked
                     (Google__Protobuf__Empty *message,
                      ProtobufCAllocator *allocator);

/* --- data packers and unpackers -- */

/* google__protobuf__empty packer and unpacker methods */
size_t google__protobuf__empty_packer (void *input, grpc_byte_buffer **buffer);
void *google__protobuf__empty_unpacker (grpc_c_context_t *context, grpc_byte_buffer *buffer);
void google__protobuf__empty_free (Google__Protobuf__Empty * buf);


/* --- services --- */

/* --- descriptors --- */

extern const ProtobufCMessageDescriptor google__protobuf__empty__descriptor;



#endif  /* PROTOBUF_C_empty_2eproto__INCLUDED */
