#include "../lib/interface.h"
#include "frames.h"

//void printByteBits(uint8_t b) {
//    for (int i = 7; i >= 0; i--) {
//        printf("%d", (b >> i) & 1);
//    }
//}
//
//void printFrameBits(frame *f) {
//    for (int y = 0; y < HEIGHT; y++) {
//        for (int x = 0; x < WIDTH; x++) {
//            printByteBits(f->rows[y].pixels[x]);
//            printf(" ");
//        }
//        printf("\n");
//    }
//}

int main() {
    stdio_init_all();
    i2cInit();
    uint8_t init2[] = {
        0x00,0xAE,0xA8,0x3F,0xD3,0x00,0x40,0xA1,0xC8,
        0xDA,0x12,0x81,0x7F,0xA6,0xD5,0x80,0x8D,0x14,0xAF
    };
    i2cSendPacket((OLED_ADDR),init2, sizeof(init2),false);


    frame Screen;
    while (1)
    {
        for(int i=0;i<VIDEO_FRAMES;i++){
            Screen=video[i];
            displayFrame(OLED_ADDR,Screen);
        }
    }
    
}
