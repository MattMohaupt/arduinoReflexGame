/*-------------------------------------------
Group Project: Reflex Game 
GAME ARDUINO CODE
Date: 4-22-24
Course: CS 362, Spring 2024, UIC
System: Visual Studio Code
Author of code: Matthew Mohaupt; mmohau3; 651525023
Group members: Matthew Mohaupt, Ember Garfinkle
------------------------------------------- */

//Global variables defined here
int wincon = 2000;      //how long the win condition must be met for
int buttonTO = 50;      //how long the button must wait before checking its status for debounce purposes

int bit8 = 7;           //the led corresponding to binary 8 bit
int bit4 = 6;           //the led corresponding to binary 4 bit
int bit2 = 5;           //the led corresponding to binary 2 bit
int bit1 = 4;           //the led corresponding to binary 1 bit

int minibutton = 8;     //the button that is used for the minigame

int potentio = A0;      //the analogue potentionmeter
int photo = A5;         //the analogue photoresistor

char miniID = '\0';     //the ID of the command the 

int lightRead = 0;      //the value currently read from the photoresistor

int potentioRead = 0;   //the value currently read from the potentiometer

int match = -1;         //the numerical value of what the user must reach to complete the game that is sent by the control arduino
int lightLevel = 0;     //the light value that must be obtained by the user in the photoresistor minigame
int buttonmatch = 0;    //the button vaue that must be obtained by the user in the button minigame
int binarymatch = 0;    //the binary led value that must be obtained by the user in the binary led minigame
int buttoncount = 0;    //a counter for what value the user is on during the button minigame
int potentiocount = 0;  //what value the user is on during the binary minigame[0-15]

char receive[8];        //what is recieved in serial io
char send[8];           //what is sent to serial io

bool activated = false; //whether or not a requesnt was sent by the control arduino
bool isabort = false;   //whether or not the current status of the code should be aborted(if the user presses the end game button on control arduino)

unsigned long curr = 0; //the current system time in milliseconds
unsigned long prev = 0; //the previous recorded system time in milliseconds

unsigned long bcurr;    //the current system time that the button has been pressed on in milliseconds
unsigned long bprev = 0;//the previous recorded system time that the button has been pressed on in milliseconds

byte currminibuttonpress = LOW; //the current status on whether the button is being pressed or not
byte prevminibuttonpress = LOW; //the previous recorded status on whether the button has been pressed or not
//------------------------------------------------------

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

void loop(){
    if(Serial.available()){ //the serial must be available in order to run the code
        activated = interpret(); //check to see if a command has been requested
        if(activated){ //find out what has been called
            isabort = shouldabort();
            if(isabort){
                killgame();
            }
            else{
                //This is the photo Resistor Mini game where the user must match the random light level range and hold it to that for 5 seconds 
                //  in order to gain a point.
                if(miniID ==  'x' || miniID == 'y' || miniID == 'z'){
                    if(miniID == 'x'){ //the photoresistor should be darkened
                        match = 50;
                    }
                    else if(miniID = 'y'){//the photoresistor should be left at room light level
                        match = 125;
                    }
                    else if(miniID = 'z'){//the photo resistor shoud be brightened
                        match = 250;
                    }
                    else{
                        killgame();
                    }
                    lightgame();
                }
                //button minigame must match the number with the same amount of button presses
                else if(miniID == 'b'){
                    buttongame();
                }
                //this is the potentiometer minigame where a user must use the potentiometer to match the number shown to them, by twisting the 
                //  potentiometer to represent that number in binary with the leds
                else if(miniID == 'l'){
                    binarygame();
                }
                //not a valid game ID
                else{
                    killgame();
                }
            }
        } //otherwise do nothing
    }
}

bool shouldabort(){
    if(miniID == 'k'){
        return true;
    }
    else{
        return false;
    }
}

bool interpret(){
    //interpret the io message that control arduino sent
    memset(receive, 0x00, 8);
    Serial.readBytesUntil('.', receive, 8);
    //Serial.print("recieved = ");
    //Serial.println(receive);
    if(receive[0] == '?'){
        miniID = tolower(receive[1]);
        char* beg = receive;
        beg++; //go past the ?
        beg++; //go past the minigame identifier and no beg should point to the beginning of the number
        char* tok = strtok("!", beg); //find the end of the serial string
        int numlength = (int)((long)&beg - (long)&tok);
        if(numlength > 1){//there is atleast one value between minigame identifier and !
            char matches[numlength +1];
            memset(matches, 0x00, numlength+1);
            strncpy(matches, beg, numlength);
            matches[numlength] = '\0';
            match = atoi(matches);
        }
        return true;
    }
    else{
        //not in proper format or something wacky is being sent over
        return false;
    }
}

void killgame(){
    //abort call has been triggered
    isabort = false;
    activated = false;
    miniID = '\0';
    match = -1;
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(bit1, LOW);
    digitalWrite(bit2, LOW);
    digitalWrite(bit4, LOW);
    digitalWrite(bit8, LOW);
}

void buttongame(){
    //button pressing minigame
    curr = millis();
    bcurr = millis();
    buttoncount = 0;
    prev = curr;
    bprev = bcurr;
    buttonmatch = match;
    while(curr - prev < wincon){ //in order to win the minigame the user must match number based on what their count is
        if(Serial.available()){
            activated = interpret();//check to see if any request came in
            isabort = shouldabort();//check to see if the game should be aborted
            if(activated && isabort){
                killgame();
                return;
            }
        }
        curr = millis();
        bcurr = millis();
        currminibuttonpress = digitalRead(minibutton);
        if(currminibuttonpress != prevminibuttonpress){ 
            if(bcurr - bprev >= buttonTO){
                prevminibuttonpress = currminibuttonpress;
                if(currminibuttonpress == LOW){ //button confirmed to be pressed and then released
                    buttoncount++;
                }
                bprev = bcurr;
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
        }
    }
    digitalWrite(LED_BUILTIN, LOW);
    wongame();
}

void lightgame(){
    //photoresistor minigame
    curr = millis();
    prev = curr;
    lightLevel = match;
    while(curr - prev < wincon){ //in order to win the minigame the user must match the light level for the desired time
        if(Serial.available()){
            activated = interpret();//check to see if any request came in
            isabort = shouldabort();//check to see if the game should be aborted
            if(activated && isabort){
                killgame();
                return;
            }
        }
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
    digitalWrite(LED_BUILTIN, LOW);
    wongame();
}

void binarygame(){
    //led+potentiometer binary counting minigame
    curr = millis();
    prev = curr;
    binarymatch = match;
    while(curr - prev < wincon){
        if(Serial.available()){
            activated = interpret();//check to see if any request came in
            isabort = shouldabort();//check to see if the game should be aborted
            if(activated && isabort){
                killgame();
                return;
            }
        }
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
            //something went wrong
            killgame();
            return;
        }

        if(potentiocount != binarymatch){
            digitalWrite(LED_BUILTIN, LOW);
            prev = curr;
        }
        else{
            digitalWrite(LED_BUILTIN, HIGH);
        }
    }
    wongame();
}

void wongame(){
    //sends message to control arduino that the game has been won
    memset(send, 0x00, 8);
    potentioRead = analogRead(potentio);
    potentioRead = potentioRead%10;
    snprintf(send, 8, "?W%d!", potentioRead);
    //Serial.print("sending = ");
    Serial.write(send);
    killgame();
}