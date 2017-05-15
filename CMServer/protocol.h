#ifndef PROTOCOL_H
#define PROTOCOL_H

enum MessageType {
  Undefined = -1,
  CallFrame = 0,
  StartCall,
  SuccessCall,
  CanselCall,
  EndCall,
  TextMessage,
  Auth,
  GetUserList
};

#endif // PROTOCOL_H
