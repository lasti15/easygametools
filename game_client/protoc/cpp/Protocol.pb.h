// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Protocol.proto

#ifndef PROTOBUF_Protocol_2eproto__INCLUDED
#define PROTOBUF_Protocol_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2004000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
// @@protoc_insertion_point(includes)

namespace ROG {
namespace protocol {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_Protocol_2eproto();
void protobuf_AssignDesc_Protocol_2eproto();
void protobuf_ShutdownFile_Protocol_2eproto();

class ProtocolMessage;

enum MessageId {
  CHALLENGE = 1,
  CHALLENGERESPONSE = 2,
  CHALLENGEACCEPTED = 3,
  NEWHOST = 4,
  USER = 5
};
bool MessageId_IsValid(int value);
const MessageId MessageId_MIN = CHALLENGE;
const MessageId MessageId_MAX = USER;
const int MessageId_ARRAYSIZE = MessageId_MAX + 1;

const ::google::protobuf::EnumDescriptor* MessageId_descriptor();
inline const ::std::string& MessageId_Name(MessageId value) {
  return ::google::protobuf::internal::NameOfEnum(
    MessageId_descriptor(), value);
}
inline bool MessageId_Parse(
    const ::std::string& name, MessageId* value) {
  return ::google::protobuf::internal::ParseNamedEnum<MessageId>(
    MessageId_descriptor(), name, value);
}
// ===================================================================

class ProtocolMessage : public ::google::protobuf::Message {
 public:
  ProtocolMessage();
  virtual ~ProtocolMessage();
  
  ProtocolMessage(const ProtocolMessage& from);
  
  inline ProtocolMessage& operator=(const ProtocolMessage& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const ProtocolMessage& default_instance();
  
  void Swap(ProtocolMessage* other);
  
  // implements Message ----------------------------------------------
  
  ProtocolMessage* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ProtocolMessage& from);
  void MergeFrom(const ProtocolMessage& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required .ROG.protocol.MessageId messageId = 1;
  inline bool has_messageid() const;
  inline void clear_messageid();
  static const int kMessageIdFieldNumber = 1;
  inline ROG::protocol::MessageId messageid() const;
  inline void set_messageid(ROG::protocol::MessageId value);
  
  GOOGLE_PROTOBUF_EXTENSION_ACCESSORS(ProtocolMessage)
  // @@protoc_insertion_point(class_scope:ROG.protocol.ProtocolMessage)
 private:
  inline void set_has_messageid();
  inline void clear_has_messageid();
  
  ::google::protobuf::internal::ExtensionSet _extensions_;
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  int messageid_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_Protocol_2eproto();
  friend void protobuf_AssignDesc_Protocol_2eproto();
  friend void protobuf_ShutdownFile_Protocol_2eproto();
  
  void InitAsDefaultInstance();
  static ProtocolMessage* default_instance_;
};
// ===================================================================


// ===================================================================

// ProtocolMessage

// required .ROG.protocol.MessageId messageId = 1;
inline bool ProtocolMessage::has_messageid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ProtocolMessage::set_has_messageid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ProtocolMessage::clear_has_messageid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ProtocolMessage::clear_messageid() {
  messageid_ = 1;
  clear_has_messageid();
}
inline ROG::protocol::MessageId ProtocolMessage::messageid() const {
  return static_cast< ROG::protocol::MessageId >(messageid_);
}
inline void ProtocolMessage::set_messageid(ROG::protocol::MessageId value) {
  GOOGLE_DCHECK(ROG::protocol::MessageId_IsValid(value));
  set_has_messageid();
  messageid_ = value;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace protocol
}  // namespace ROG

#ifndef SWIG
namespace google {
namespace protobuf {

template <>
inline const EnumDescriptor* GetEnumDescriptor< ROG::protocol::MessageId>() {
  return ROG::protocol::MessageId_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_Protocol_2eproto__INCLUDED