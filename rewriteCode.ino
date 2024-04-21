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
                        match = 75;
                    }
                    else if(miniID = 'y'){//the photoresistor should be left at room light level
                        match = 150;
                    }
                    else if(miniID = 'z'){//the photo resistor shoud be brightened
                        match = 225;
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
    Serial.readBytesUntil('!', receive, 8);
    if(receive[0] == '?'){
        miniID = tolower(receive[1]);
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
}

void lightgame(){
    //photoresistor minigame
    curr = millis();
    prev = curr;
    lightLevel = match;
    while(curr - prev < wincon){ //in order to win the minigame the user must match the light level for the desired time
        activated = interpret();//check to see if any request came in
        isabort = shouldabort();//check to see if the game should be aborted
        if(activated && isabort){
            killgame();
            return;
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
}

void wongame(){
    //sends message to control arduino that the game has been won
    memset(send, 0x00, 8);
    potentioRead = analogRead(potentio);
    potentioRead = potentioRead%10;
    snprintf(send, 8, "?W%d!", potentioRead);
    Serial.write(send);
    killgame();
}