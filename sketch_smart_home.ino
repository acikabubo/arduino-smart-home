using namespace std;

#include <math.h>
#include <ArduinoSTL.h>
#include <IRremote.h>

#define touch1 8
#define touch2 9
#define touch4 10
#define led 12

IRsend irsend;

const int IR_RECEIVE_PIN = 7;
IRrecv irrecv(IR_RECEIVE_PIN);
decode_results results;

String rawCode = "";
long code = 0;

// Define handlers constants
const char TURN_ON_OFF = '1';
const char MUTE_VOLUME = '2';
const char INCREASE_VOLUME = '3';
const char DECREASE_VOLUME = '4';
const char GO_TO_CHANNEL = '5';
const char GO_TO_PREV_CHANNEL = '6';

// Define remote codes
const long BTN_ON_OFF = 0x1000C;
const long BTN_OK = 0x1005C;
const long BTN_MUTE = 0x1000D;
const long BTN_VOLUME_UP = 0x10;
const long BTN_VOLUME_DOWN = 0x10011;
const long BTN_GO_TO_PREV_CHANNEL = 0x1000A;

// Define code for numbers from 0 to 9
const long CODES[] = {  
    0x10000, 0x10001, 0x10002, 
    0x10003, 0x10004, 0x10005, 
    0x10006, 0x10007, 0x10008, 
    0x10009
};

// Function that send code for turning on or off tv
void turnOnOff() {
    Serial.println("TURN ON/OFF");
    irsend.sendRC6(BTN_ON_OFF, 20);
    delay(300);
}

// Function that send code for mute or unmute volume
void muteVolume() {
    Serial.println("UN/MUTE VOLUME");
    irsend.sendRC6(BTN_MUTE, 20);
    delay(300);
}

// Function that send number of how many times to increase volume
void increaseVolume(long volume) {
    Serial.println("INCREASE VOLUME");
    for(int i=0; i<=volume; i++) {
        digitalWrite(led, HIGH);
        irsend.sendRC6(BTN_VOLUME_UP, 20);
        delay(200);
        digitalWrite(led, LOW);
        delay(100);
    }
}

// Function that send number of how many times to decrease volume
void decreaseVolume(long volume) {
    Serial.println("DECREASE VOLUME");
    for(int i=0; i<=volume; i++) {
        digitalWrite(led, HIGH);
        irsend.sendRC6(BTN_VOLUME_DOWN, 20);
        delay(200);
        digitalWrite(led, LOW);
        delay(100);
    }
}


// Function that send channel
void goToChannel(long channel) {
    Serial.println("GO TO CHANNEL");
    vector<int> digits;

    // Split channel into digits
    while(channel)
    {
        digits.push_back(channel % 10);
        channel /= 10;
    }

    // Change order
    reverse(digits.begin(),digits.end());

    // Iterate over digits and send codes
    for(auto i: digits){
        Serial.print("DIGIT: ");
        Serial.println(i);
        irsend.sendRC6(CODES[int(i)], 20);
        delay(300);
    };
    
    // Send code for OK button
    irsend.sendRC6(BTN_OK, 20);
}

// Function that send code to go to previous channel
void goToPrevChannel() {
    Serial.println("GO TO PREVIOUS CHANNEL");
    irsend.sendRC6(BTN_GO_TO_PREV_CHANNEL, 20);
    delay(300);
}

void setup(){
    Serial.begin(9600);
    
    pinMode(touch1, INPUT);
    pinMode(touch2, INPUT);
    pinMode(touch4, INPUT);
    
    irrecv.enableIRIn();
    irrecv.blink13(true);
  
    pinMode(led, OUTPUT);
    digitalWrite (led, LOW);
}

void loop(){
    // Read code
    while(Serial.available()) {
        int num = Serial.read();
        if(isDigit(num)) {
            rawCode += (char)num;
        }
        
    };

    // Convert value to integer and get last 4 numbers
    char func = rawCode[0];
    code = rawCode.toInt() % 10000;

    if(func == TURN_ON_OFF) {
        turnOnOff();
    }
    else if(func == MUTE_VOLUME) {
        muteVolume();
    }
    else if(func == INCREASE_VOLUME) {
        increaseVolume(code);
    }
    else if(func == DECREASE_VOLUME) {
        decreaseVolume(code);
    }
    else if(func == GO_TO_CHANNEL) {
        goToChannel(code);
    }
    else if(func == GO_TO_PREV_CHANNEL) {
        goToPrevChannel();
    }
    else {
        Serial.println("UNKNOWN CODE");
    }

    // Set default values
    func = '0';
    rawCode = "";
    code = 0;
    
    if(irrecv.decode(&results)){
        Serial.println("REMOTE CODE");
        Serial.println(results.value);
        Serial.println(results.value, HEX);
        delay(1000);
    }
    
    int touched1 = digitalRead(touch1);
    if (touched1 == HIGH){
        Serial.println("TOUCHED 1");
        for(int i=0; i<=5; i++) {
            Serial.println(i);
            irsend.sendRC6(0x10, 20);  // Increase volume
            delay(300);
        }
       
    }

    int touched2 = digitalRead(touch2);
    if (touched2 == HIGH){
        Serial.println("TOUCHED 2");
        for(int i=0; i<=5; i++) {
            Serial.println(i);
            irsend.sendRC6(0x10011, 20);  // Descrease volume
            delay(300);
        }
    }
  
    int touched4 = digitalRead(touch4);
    if (touched4 == HIGH){
        Serial.println("TOUCHED 4");
        irsend.sendRC6(0x1000C, 20);
        delay(300);
    }
}
