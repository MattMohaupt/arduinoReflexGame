
/*-------------------------------------------
Group Project: Reflex Game
Date: 
Course: CS 362, Spring 2024, UIC
System: Visual Studio Code
Author: Matthew Mohaupt; mmohau3; 651525023
TA presented to: 
Description: 
Refrences: 
------------------------------------------- */

int wincon = 2000;
int bit8 = 7;
int bit4 = 6;
int bit2 = 5;
int bit1 = 4;
int potentio = A0;
int photo = A5;
unsigned long curr;
unsigned long prev = 0;
int interval = 1000;
char mininum = 0;
int lightLevel = 0;
int lightRead = 0;
int potentioRead = 0;
int match = -1;
int buttoncount = 0;
int potentiocount = -1;
char minibutton = '\0';
byte currminibuttonpress = LOW;
byte prevminibuttonpress = LOW;
int buttontimeout = 50;
unsigned long bcurr;
unsigned long bprev = 0;
char receive[8];
char send[8];

void setup(){
    memset(receive, 0x00, 8); //clear the recieve value
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(bit8, OUTPUT);
    pinMode(bit4, OUTPUT);
    pinMode(bit2, OUTPUT);
    pinMode(bit1, OUTPUT);
    pinMode(minibutton, INPUT);
    Serial.begin(9600);
}

void loop() {
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(bit8, LOW);
    digitalWrite(bit4, LOW);
    digitalWrite(bit2, LOW);
    digitalWrite(bit1, LOW);

    if(Serial.available()){
        Serial.readBytesUntil('!', receive, 8);
        if(receive[0] == '?'){
            mininum = tolower(receive[1]);
            char* beg = receive;
            beg++; //go past the ?
            beg++; //go past the minigame identifier and no beg should point to the beginning of the number
            char* tok = strstr("!", beg); //find the end of the serial string
            int numlength = (int)(tok - beg);
            if(numlength > 1){//there is atleast one value between minigame identifier and !
                char matches[numlength +1];
                strncpy(matches, beg, numlength);
                matches[numlength + 1] = '\0';
                match = atoi(matches);
            }
        }

        
    }
    else{
        mininum = '\0';
    }
  


    if(mininum == 'k'){
        //abort game so set everything to default values
        mininum = '\0';
        curr = 0;
        prev = 0;
        bcurr = 0;
        bprev = 0;
        match = -1;
        memset(receive, 0x00, 8);
        memset(send, 0x00, 8);
        potentiocount = -1;
        buttoncount = 0;
        currminibuttonpress = LOW;
        prevminibuttonpress = LOW;
        digitalWrite(LED_BUILTIN, LOW);
        digitalWrite(bit1, LOW);
        digitalWrite(bit2, LOW);
        digitalWrite(bit4, LOW);
        digitalWrite(bit8, LOW);
    }
    //This is the photo Resistor Mini game where the user must match the random light level range and hold it to that for 5 seconds 
    //  in order to gain a point.
    else if(mininum ==  'x' || mininum == 'y' || mininum == 'z'){
        curr = millis();
        prev = curr;
        if(mininum == 'x'){
            lightLevel = 100;
        }
        else if(mininum = 'y'){
            lightLevel = 150;
        }
        else if(mininum = 'z'){
            lightLevel = 200;
        }
        else{
            lightLevel = -1;
            return;
        }
        if(curr - prev < wincon){ //in order to win the minigame the user must match the light level for 5 seconds
            curr = millis();
            lightRead = analogRead(photo);
            if(lightRead > (lightLevel+50) || lightRead < (lightLevel - 50)){
                digitalWrite(LED_BUILTIN, LOW);
                prev = curr;
            }
            else{
                digitalWrite(LED_BUILTIN, HIGH);
            }
        }
        else{//user wins photoresitor minigame
            memset(receive, 0x00, 8);
            mininum = '\0';
            match = 0;
            potentioRead = analogRead(potentio);
            potentioRead = potentioRead%10;
            snprintf(send, 8, "?W%d!", potentioRead);
            Serial.write(send);
        }
    }

    //button minigame must match the number with the same amount of button presses
    else if(mininum == 'b'){

        curr = millis();
        buttoncount = 0;
        prev = curr;
        if(curr - prev < wincon){ //in order to win the minigame the user must match number based on what their count is
            curr = millis();
            bcurr = millis();
            currminibuttonpress = digitalRead(minibutton);
            if(currminibuttonpress != prevminibuttonpress){ 
                if(bcurr - bprev >= buttontimeout){
                    prevminibuttonpress = currminibuttonpress;
                    if(currminibuttonpress == LOW){ //button confirmed to be pressed and then released
                        buttoncount++;
                    }
                }
            }
            if(buttoncount != match){
                digitalWrite(LED_BUILTIN, LOW);
                prev = curr;
            }
            else{
                digitalWrite(LED_BUILTIN, HIGH);
            }

            if(buttoncount > match){
                buttoncount = 0;
                //Serial.println("Went too high, reset back to zero");
            }
        }
        else{//user wins button minigame
            memset(receive, 0x00, 8);
            mininum = '\0';
            match = 0;
            potentioRead = analogRead(potentio);
            potentioRead = potentioRead%10;
            snprintf(send, 8, "?W%d!", potentioRead);
            Serial.write(send);
        }
    }



    //this is the potentiometer minigame where a user must use the potentiometer to match the number shown to them, by twisting the 
    //  potentiometer to represent that number in binary with the leds
    else if(mininum == 'l'){
        curr = millis();
        prev = curr;
        if(curr - prev < wincon){
            curr = millis();
            potentioRead = analogRead(potentio);
            //the range of values from a potentiometer is [0, 1024] so to split that up between 4 LEDs represinting binary bit, then that 
            //  range must be seperated into 16 states so each ranch value 64
            if(potentioRead < 64){ //potentio is between 0 to 63 so represent that as 0 in binary
                digitalWrite(bit1, LOW);
                digitalWrite(bit2, LOW);
                digitalWrite(bit4, LOW);
                digitalWrite(bit8, LOW);
                potentiocount = 0;
            }
            else if(potentioRead >= 64 && potentioRead < 128){//potentio is between 64 to 128 so represent that as 1 in binary
                digitalWrite(bit1, HIGH);
                digitalWrite(bit2, LOW);
                digitalWrite(bit4, LOW);
                digitalWrite(bit8, LOW);
                potentiocount = 1;
            }
            else if(potentioRead >= 128 && potentioRead < 192){//potentio is between 128 to 192 so represent that as 2 in binary
                digitalWrite(bit1, LOW);
                digitalWrite(bit2, HIGH);
                digitalWrite(bit4, LOW);
                digitalWrite(bit8, LOW);
                potentiocount = 2;
            }
            else if(potentioRead >= 192 && potentioRead < 256){//potentio is between 192 to 256 so represent that as 3 in binary
                digitalWrite(bit1, HIGH);
                digitalWrite(bit2, HIGH);
                digitalWrite(bit4, LOW);
                digitalWrite(bit8, LOW);
                potentiocount = 3;
            }
            else if(potentioRead >= 256 && potentioRead < 320){//potentio is between 256 to 320 so represent that as 4 in binary
                digitalWrite(bit1, LOW);
                digitalWrite(bit2, LOW);
                digitalWrite(bit4, HIGH);
                digitalWrite(bit8, LOW);
                potentiocount = 4;
            }
            else if(potentioRead >= 320 && potentioRead < 384){//potentio is between 320 to 384 so represent that as 5 in binary
                digitalWrite(bit1, HIGH);
                digitalWrite(bit2, LOW);
                digitalWrite(bit4, HIGH);
                digitalWrite(bit8, LOW);
                potentiocount = 5;
            }
            else if(potentioRead >= 384 && potentioRead < 448){//potentio is between 384 to 448 so represent that as 6 in binary
                digitalWrite(bit1, LOW);
                digitalWrite(bit2, HIGH);
                digitalWrite(bit4, HIGH);
                digitalWrite(bit8, LOW);
                potentiocount = 6;
            }
            else if(potentioRead >= 448 && potentioRead < 512){//potentio is between 448 to 512 so represent that as 7 in binary
                digitalWrite(bit1, HIGH);
                digitalWrite(bit2, HIGH);
                digitalWrite(bit4, HIGH);
                digitalWrite(bit8, LOW);
                potentiocount = 7;
            }
            else if(potentioRead >= 512 && potentioRead < 576){//potentio is between 512 to 576 so represent that as 8 in binary
                digitalWrite(bit1, LOW);
                digitalWrite(bit2, LOW);
                digitalWrite(bit4, LOW);
                digitalWrite(bit8, HIGH);
                potentiocount = 8;
            }
            else if(potentioRead >= 576 && potentioRead < 640){//potentio is between 576 to 640 so represent that as 9 in binary
                digitalWrite(bit1, HIGH);
                digitalWrite(bit2, LOW);
                digitalWrite(bit4, LOW);
                digitalWrite(bit8, HIGH);
                potentiocount = 9;
            }
            else if(potentioRead >= 640 && potentioRead < 704){//potentio is between 640 to 704 so represent that as 10 in binary
                digitalWrite(bit1, LOW);
                digitalWrite(bit2, HIGH);
                digitalWrite(bit4, LOW);
                digitalWrite(bit8, HIGH);
                potentiocount = 10;
            }
            else if(potentioRead >= 704 && potentioRead < 768){//potentio is between 704 to 768 so represent that as 11 in binary
                digitalWrite(bit1, HIGH);
                digitalWrite(bit2, HIGH);
                digitalWrite(bit4, LOW);
                digitalWrite(bit8, HIGH);
                potentiocount = 11;
            }
            else if(potentioRead >= 768 && potentioRead < 832){//potentio is between 768 to 834 so represent that as 12 in binary
                digitalWrite(bit1, LOW);
                digitalWrite(bit2, LOW);
                digitalWrite(bit4, HIGH);
                digitalWrite(bit8, HIGH);
                potentiocount = 12;
            }
            else if(potentioRead >= 832 && potentioRead < 896){//potentio is between 832 to 896 so represent that as 13 in binary
                digitalWrite(bit1, HIGH);
                digitalWrite(bit2, LOW);
                digitalWrite(bit4, HIGH);
                digitalWrite(bit8, HIGH);
                potentiocount = 13;
            }
            else if(potentioRead >= 896 && potentioRead < 960){//potentio is between 896 to 960 so represent that as 14 in binary
                digitalWrite(bit1, LOW);
                digitalWrite(bit2, HIGH);
                digitalWrite(bit4, HIGH);
                digitalWrite(bit8, HIGH);
                potentiocount = 14;
            }
            else if(potentioRead >= 960){//potentio is between 960 to 1024 so represent that as 15 in binary
                digitalWrite(bit1, HIGH);
                digitalWrite(bit2, HIGH);
                digitalWrite(bit4, HIGH);
                digitalWrite(bit8, HIGH);
                potentiocount = 15;
            }
            else{
                exit(0);
            }

            if(potentiocount != match){
                digitalWrite(LED_BUILTIN, LOW);
                prev = curr;
            }
            else{
                digitalWrite(LED_BUILTIN, HIGH);
            }
        }
        else{//user potentiometer minigame
            memset(receive, 0x00, 8);
            mininum = '\0';
            match = 0;
            potentioRead = analogRead(potentio);
            potentioRead = potentioRead%10;
            snprintf(send, 8, "?W%d!", potentioRead);
            Serial.write(send);
        }
    }
    else{
        mininum = '\0';
    }

}