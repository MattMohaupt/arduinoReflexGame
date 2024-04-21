
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
int bit8 = 7;
int bit4 = 6;
int bit2 = 5;
int bit1 = 4;
int potentio = A0;
int photo = A5;
unsigned long curr;
unsigned long prev = 0;
int interval = 1000;
int mininum = 0;
int lightLevel = 0;
int lightRead = 0;
int potentioRead = 0;
int rando = 0;
int score = 0;
int match = 0;
int buttoncount = 0;
int potentiocount = -1;
int minibutton = 8;
byte currminibuttonpress = LOW;
byte prevminibuttonpress = LOW;
int buttontimeout = 50;
unsigned long bcurr;
unsigned long bprev = 0;

void setup(){
    randomSeed(analogRead(potentio));
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(bit8, OUTPUT);
    pinMode(bit4, OUTPUT);
    pinMode(bit2, OUTPUT);
    pinMode(bit1, OUTPUT);
    pinMode(minibutton, INPUT);
    Serial.begin(9600);
    Serial.println(score);
}

void loop() {
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(bit8, LOW);
    digitalWrite(bit4, LOW);
    digitalWrite(bit2, LOW);
    digitalWrite(bit1, LOW);
    mininum = random(3); //will generate a random number from 0 to 2



    //This is the photo Resistor Mini game where the user must match the random light level range and hold it to that for 5 seconds 
    //  in order to gain a point.
    if(mininum == 0){
        curr = millis();
        prev = curr;
        lightLevel = random(4);
        lightLevel = (lightLevel * 100) + 50;
        Serial.print("Light level minigame!!!! Match the brightness value of ");
        Serial.println(lightLevel);
        while(curr - prev < 5000){ //in order to win the minigame the user must match the light level for 5 seconds
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
        score++;
        digitalWrite(LED_BUILTIN, LOW);
    }

    //button minigame must match the number with the same amount of button presses
    else if(mininum == 1){

        curr = millis();
        buttoncount = 0;
        prev = curr;
        match = random(10) + 1; //will generate a random number from 0 to 9 and then add 1 so the range is 1 to 10
        Serial.print("Button press minigame!!!! Hit the button ");
        Serial.print(match);
        Serial.println(" times!!!");
        while(curr - prev < 5000){ //in order to win the minigame the user must match number based on what their count is
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
                Serial.println("Went too high, reset back to zero");
            }
        }
        score++;
        digitalWrite(LED_BUILTIN, LOW);
    }



    //this is the potentiometer minigame where a user must use the potentiometer to match the number shown to them, by twisting the 
    //  potentiometer to represent that number in binary with the leds
    else if(mininum == 2){
        curr = millis();
        prev = curr;
        match = random(16); //random number from 0 to 15
        Serial.print("Potentiometer Binary minigame!!!! Make the binary number ");
        Serial.println(match);
        while(curr - prev < 5000){
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
                Serial.println("Error");
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
        score++;
        digitalWrite(LED_BUILTIN, LOW);
    }
    else{
        mininum = 0;
    }
    Serial.print("Score: ");
    Serial.println(score);

}