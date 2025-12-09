#include "interface.h"

void i2cSleepBetweenPulses(){
    sleep_us(1000000/BAUDRATE);
}

void SDA_HIGH(){
    gpio_set_dir(SDA_PIN,GPIO_IN);
}
void SDA_LOW(){
    gpio_set_dir(SDA_PIN, GPIO_OUT);
    gpio_put(SDA_PIN, 0);
}
void SCL_HIGH(){
    gpio_set_dir(SCL_PIN,GPIO_IN);
}
void SCL_LOW(){
    gpio_set_dir(SCL_PIN, GPIO_OUT);
    gpio_put(SCL_PIN, 0);
}

void i2cInit(){
    gpio_init(SDA_PIN);
    gpio_init(SCL_PIN);

    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
    SDA_HIGH();
    SCL_HIGH();
}

void i2cStart(){
    SDA_HIGH();
    SCL_HIGH();
    i2cSleepBetweenPulses();

    SDA_LOW();
    i2cSleepBetweenPulses();

    SCL_LOW();
    i2cSleepBetweenPulses();
}

void i2cFinish(){
    SDA_LOW();
    SCL_LOW();
    i2cSleepBetweenPulses();

    SCL_HIGH();
    i2cSleepBetweenPulses();
    SDA_HIGH();
}

// Send a bit by pulsing by setting data to the bit then pulsign the clock
void i2cSendBit(bool bit){
    if(bit){
        SDA_HIGH();
    }else{
        SDA_LOW();
    }
    SCL_HIGH();
    i2cSleepBetweenPulses();
    SCL_LOW();
    i2cSleepBetweenPulses();
}

// loop through all the bits sending them all then finishing, while cheaking the ACK
// !!!! If using function raw, user must initilise the Start and Stop commands
int i2cSendByte(uint8_t byte){
    // Send the Byte
    int i = 7; // 8 bits of data
    while (i>=0){
        i2cSendBit(((uint8_t)(byte>>i))&((uint8_t)0x1));
        i--;
    }
    // Return the ACK
    SDA_HIGH();
    SCL_HIGH(); 
    i2cSleepBetweenPulses();
    int ack = gpio_get(SDA_PIN);
    SCL_LOW();
    SDA_LOW();
    i2cSleepBetweenPulses();
    if (ack){
        gpio_set_dir(25,GPIO_OUT);
        gpio_put(25,1);
    }
    return ack;
}
// Sends several data bits, requires a list of the instructions and amount of them, 
// Strict is wheather to ignore failing packets, if True it will return when any packet fails with its index
// If false it will return the amount of failed packets
int i2cSendPacket(uint8_t address,uint8_t *instructions, size_t num, bool Strict){
    i2cStart();
    if (i2cSendByte(address<<1)){
        i2cFinish();
        return 1;
    }
    uint32_t i = 0;
    unsigned int failed = 0;
    while (i<num){
        int code = i2cSendByte(instructions[i]);
        // Failed
        if(code){
            if(Strict){  
                i2cFinish();
                return i+1;
            }
            else{
                failed++;
            }
        }
        i++;
    }
    if (!Strict){
        i2cFinish();
        return failed;
    }
    else{
        i2cFinish();
        return 0;
    }
}

int SetPixel(int x, int y, frame *Screen, bool bit){
    // find row and see if its valid
    int rowadr = y/8;
    if (rowadr>=HEIGHT){
        return 0;
    }
    if (x>=WIDTH){
        return 0;
    }
    if (bit){
        Screen->rows[rowadr].pixels[x] |= (1 << y%8);
    }else{
        Screen->rows[rowadr].pixels[x] &= ~(1 << y%8);
    }
    return 1;
}

int displayFrame(uint8_t address,frame screen){
    for (uint8_t page = 0; page < HEIGHT; page++) {
        uint8_t cmd[] = {0x00, //Following is a command
            0xB0 | page, // 0xB0 is the base page command and then follow it with the page
            0x00, // Set lower nibble to 0 of collom addresses
            0x10  // Set higher nibble to 0 of collom addresses
        };
        i2cSendPacket(OLED_ADDR,cmd, sizeof(cmd), false);

        uint8_t data[WIDTH+1]; // Set to be one large to allow for the command bit
        data[0]=0x40; // Set first value to indicate write command
        for(int index = 1; index <=WIDTH;index++){
            data[index]=screen.rows[page].pixels[index];
        }
        
        i2cSendPacket(OLED_ADDR,data, sizeof(data), false);
    }

}
