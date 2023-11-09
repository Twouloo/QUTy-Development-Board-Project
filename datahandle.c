#include <string.h>
#include <avr/io.h>
#include <stdio.h>

extern uint32_t key1;
extern uint32_t key2;
extern uint8_t shiftCount;

char base64Alphabet[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', 
    '4', '5', '6', '7', '8', '9', '+', '/'
};

uint8_t charToBase64Index(char input){
    for(int i = 0; i < 64; i++)
        if(base64Alphabet[i] == input)
            return i;
}

void base64Decode(char *input, uint8_t *decoded) {
    uint8_t char3 = charToBase64Index(*(input + 2));
    uint8_t char4 = charToBase64Index(*(input + 3));
    uint8_t char1 = charToBase64Index(*input);
    uint8_t char2 = charToBase64Index(*(input + 1));

    decoded[0] = ((char1) << 2) | ((char2) >> 4);   //byte 1
    decoded[1] = ((char2) << 4) | ((char3) >> 2);   //byte 2 
    decoded[2] = ((char3) << 6) | char4;            //byte 3
}

uint32_t leastSignificantByte(uint32_t input){
    return input & 0xFF;  // extract the least significant byte
}

uint8_t leastSignificantBit(uint32_t input){
    return input & 1;  // extract the least significant bit
}

void descrambler(uint8_t *input, uint8_t *output) {
    for(int i = 0; i < 3; i++) {
        output[i] = leastSignificantByte(key2) ^ input[i];
        shiftCount = leastSignificantBit(key2);
        key2 = key2 >> 1;
        if(shiftCount == 1)
            key2 = key2 ^ key1;
    }   
}
