#include "Message.hpp"

Message::Message() {
    type = MessageType::empty;
    text = "";
}

Message::Message(MessageType _type) {
    type = _type;
    text = "";
}

Message::Message(MessageType _type, std::string _text) {
    type = _type;
    text = _text;
}

Message::~Message() {

}