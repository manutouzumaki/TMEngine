#include "message.h" 
#include <utils/tm_darray.h>
#include <memory.h>

struct RegisteredMessage {
    void *listener;
    PFN_OnMessage onMessage;

};

struct Listener {
    RegisteredMessage *messages;
};

#define MAX_MESSAGE_CODES 16384 

struct MessageSystem {
    Listener registered[MAX_MESSAGE_CODES];
};

static MessageSystem gMessageSystem;

void MessageSystemInitialize() {
    memset(&gMessageSystem, 0, sizeof(MessageSystem));
}

void MessageSystemShoutdown() {
    
    for(int i = 0; i < MAX_MESSAGE_CODES; ++i) {
        if(gMessageSystem.registered[i].messages) { 
            TMDarrayDestroy(gMessageSystem.registered[i].messages);
        }
    }

}

bool MessageRegister(MessageType type, void *listener, PFN_OnMessage onMessage) {
    
    if(gMessageSystem.registered[type].messages) {
        for(int i = 0; i < TMDarraySize(gMessageSystem.registered[type].messages); ++i) {
            RegisteredMessage *current = gMessageSystem.registered[type].messages + i;
            if(listener == current->listener) {
                // listener already registered
                return false;
            }
        }
    }

    RegisteredMessage message{};
    message.listener = listener;
    message.onMessage = onMessage;
    TMDarrayPush(gMessageSystem.registered[type].messages, message, RegisteredMessage);
    return true;


}

void MessageFireSpread(MessageType type, void *sender, Message message) {

    if(gMessageSystem.registered[type].messages) {
        for(int i = 0; i < TMDarraySize(gMessageSystem.registered[type].messages); ++i) {
            gMessageSystem.registered[type].messages[i].onMessage(type, sender, gMessageSystem.registered[type].messages[i].listener, message);
        }
    }

}

void MessageFireFirstHit(MessageType type, void *sender, Message message) {

    if(gMessageSystem.registered[type].messages) {
        for(int i = 0; i < TMDarraySize(gMessageSystem.registered[type].messages); ++i) {
            gMessageSystem.registered[type].messages[i].onMessage(type, sender, gMessageSystem.registered[type].messages[i].listener, message);
            return;
        }
    }

}
