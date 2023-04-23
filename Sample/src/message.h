#ifndef _MESSAGE_MANAGER_H_
#define _MESSAGE_MANAGER_H_

#include <utils/tm_math.h>


enum MessageType {
    MESSAGE_TYPE_COLLISION_DETECTION,
    MESSAGE_TYPE_COLLISION_GROUNDED,
    MESSAGE_TYPE_COLLISION_RESOLUTION,
    MESSAGE_TYPE_GRAPHICS_UPDATE_POSITIONS,
    MESSAGE_TYPE_PHYSICS_ADD_FORCE,
    MESSAGE_TYPE_PHYSICS_ADD_IMPULSE,
    MESSAGE_TYPE_PHYSICS_CLEAR_FORCES,
    MESSAGE_TYPE_GRAPHICS_UPDATE_ANIMATION_INDEX,
    MESSAGE_TYPE_ANIMATION_SET_STATE
};

// TODO: adjust the size of this struct to be better siuted for the game
// NOTE: this must be zero initialized
struct Message {
    union {
        TMVec2             v2[6]; 
        double             f64[6];
        unsigned long long u64[6];
        long long          i64[6];
        void *             ptr[6];
        
        float              f32[12];
        int                i32[12];
        unsigned int       u32[12];
        
        short              i16[24];
        unsigned short     u16[24];

        char               i8[48];
        unsigned char      u8[48];
    };
};


typedef void (*PFN_OnMessage) (MessageType type, void *sender, void *listener, Message message);


void MessageSystemInitialize();
void MessageSystemShoutdown();
bool MessageRegister(MessageType type, void *listener, PFN_OnMessage onMessage);
void MessageFireSpread(MessageType type, void *sender, Message message);
void MessageFireFirstHit(MessageType type, void *sender, Message message);

#endif
