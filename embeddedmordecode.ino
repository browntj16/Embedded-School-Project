const int buttonPin = 2; // the button pin that inputs dots and dashes
const int recordButtonPin = 3; // starts and stops recording

const int led1Pin = 4;// the red LEDs are pins 4-11. 
const int led2Pin = 5;// the red LEDs show the binary of whatever
const int led3Pin = 6; // the user just entered
const int led4Pin = 7;
const int led5Pin = 8;
const int led6Pin = 9;
const int led7Pin = 10;
const int led8Pin = 11;
const int recordLedPin = 12; //The yellow pin. On when recording, off when not
const int buzzerPin = 13; //buzzer
 

unsigned long startTime = 0;
unsigned long endTime = 0;
unsigned long elapsedTime = 0;


int characters = 0;
int lastLetterPosition = 0;
char code[100]; 

bool buttonState = HIGH; // Assuming button is active LOW (connects to ground when pressed)
bool previousButtonState = HIGH;

bool recordButtonState;
bool recording = LOW;
bool repeating = LOW;
unsigned long timeSinceLastPress = 0;
//timeSinceLastPress is important because, in morse code, 
// there is a space between letters, and a larger space between words
// meaning that we need to know how long it has been since the user
// last input.


void setup() {

  pinMode(buttonPin, INPUT_PULLUP); 
  pinMode(recordButtonPin, INPUT_PULLUP);
  Serial.begin(9600); // init serial communication
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);
  pinMode(led4Pin, OUTPUT);
  pinMode(led5Pin, OUTPUT);
  pinMode(led6Pin, OUTPUT);
  pinMode(led7Pin, OUTPUT);
  pinMode(led8Pin, OUTPUT);
  pinMode(recordLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  recordButtonState = digitalRead(recordButtonPin);
 
  
  /**
  *This if else block pretty much just controls whether recording 
  *is happening or not happening
  */
  if(recordButtonState==LOW && repeating==HIGH){
    repeating=LOW;
    for(int i = 0; i<8; i++){
       digitalWrite(led1Pin + i, HIGH);
     } 
    
	startTime = 0;
	endTime = 0;
	elapsedTime = 0;
    delay(1000);
    turnOffPins();
  }
  else if (recordButtonState == LOW && recording==LOW) {
	recording=HIGH;
    repeating=LOW;
    digitalWrite(recordLedPin, HIGH);
    resetArray(code);
    characters=0;
    Serial.println("high");
    delay(100);
  }
  else if(recordButtonState==LOW && recording==HIGH){
  	recording=LOW;
    repeating=HIGH;
    digitalWrite(recordLedPin, LOW);
    Serial.println("low");
    delay(100);
    lightUpPins();
    delay(1000);
    turnOffPins();
  }
  if(repeating==HIGH){
    playMorseCode();
  }
  /**
  * if we are currently recording, then we are free to record the other
  * button clicks
  */
  if(recording==HIGH){
    // Check if button is pressed
    if (buttonState == LOW && previousButtonState == HIGH) {
      startTime = millis(); // Record the start time
      if((startTime-endTime)>3000 && (startTime-endTime)<7000){
        code[characters] = ',';
        
        //if we hit a comma, we should try to translate the prior
        //dots and dashes into a letter, then translate the letter
        // into binary
        //afterwards, we light each of our red leds to show what
        // letter they typed in
        Serial.println("Comma");
       	lightUpPins();
      }
      else if((startTime-endTime>7000)){
        code[characters] = ' ';
        Serial.println("Space");
        lightUpPins();
      }
    } //end of internal if

    // Check if button is released
    if (buttonState == HIGH && previousButtonState == LOW) {
      endTime = millis(); // Record the end time
      elapsedTime = endTime - startTime; // Calculate elapsed time

      // Print the elapsed time in milliseconds
      Serial.print("Button pressed for: ");
      Serial.print(elapsedTime);
      Serial.println(" milliseconds");

      if(elapsedTime<3000){
          code[characters]='.';
          characters = characters+1;
      }
      else if(elapsedTime>3000 && elapsedTime<5000){
        code[characters]='-';
        characters = characters+1;
      }
      else{
          for(int i =0; i < characters; i++){
          Serial.print(code[i]);       
        }
      }

    }
    previousButtonState = buttonState; // Update previous button state
  }
  

}

/**
* Pretty much, this function takes the last few inputs, converts
* them from dots and dashes to a character, converts that character
* to binary, then lights up the pins with that binary
*/
void lightUpPins(){
  		String temp = String("");
        for(int i = lastLetterPosition; i < characters; i++){
        	 temp = temp + String(code[i]);
        }
        char c = convert(temp);
        if(c!='?'){
          int bin[8];
    		for(int i = 7; i >= 0; i--) {
    			bin[i] = (bitRead(c, i));
  			}
        
        	for(int i = 0; i < 8; i++){
          	Serial.print(bin[i]);
          	if(bin[i]==0){
           		digitalWrite(led1Pin + i, LOW);
          	}
          	else if(bin[i]==1){
            	digitalWrite(led1Pin + i, HIGH);
          }
        }
          
        }
        else{
          turnOffPins();
        }
        
        characters = characters + 1;
        lastLetterPosition = characters;
}
/**
*Turns off all 8 of the red led pins. We use this either when we dont
* find a match, or one other time
*/
void turnOffPins(){
 for(int i = 0; i<8; i++){
       digitalWrite(led1Pin + i, LOW);
     } 
}

void playMorseCode(){
  for(int i = 0; i < characters; i++){
      
      char c = code[i];
      if(c=='.'){
        tone(buzzerPin, 262, 500);
        Serial.println("Dot");
        delay(1000);
      }
      else if(c=='-'){
        tone(buzzerPin, 263, 1000);
        Serial.println("Dash");
        delay(1500);
      }
      else if(c==','){
        Serial.println("Letter space");
        delay(1000);
      }
      else if(c==' '){
        Serial.println("Word space");
        delay(3000);
      }
    }
    delay(1000);
}

void resetArray(char array[]){
  for(int i =0; i < 100; i++){
  	array[i]=0; 
  }
}

//TODO: make this less shitty
char convert(String morseCode){
  if(morseCode==".-"){ //yes, a switch statement would be better here, but shut up
  	return 'a';  
  }
  else if(morseCode=="-..."){
  	return 'b';  
  }
  else if(morseCode=="-.-."){
  	return 'c';  
  }
  else if(morseCode=="-.."){
  	return 'd';  
  }
  else if(morseCode=="."){
  	return 'e';  
  }
  else if(morseCode=="..-."){
  	return 'f';  
  }
  else if(morseCode=="--."){
  	return 'g';  
  }
  else if(morseCode=="...."){
  	return 'h';  
  }
  else if(morseCode==".."){
  	return 'i';  
  }
  else if(morseCode==".---"){
  	return 'j';  
  }
  else if(morseCode=="-.-"){
  	return 'k';  
  }
  else if(morseCode==".-.."){
  	return 'l';  
  }
  else if(morseCode=="--"){
  	return 'm';  
  }
  else if(morseCode=="-."){
  	return 'n';  
  }
  else if(morseCode=="---"){
  	return 'o';  
  }
  else if(morseCode==".--."){
  	return 'p';  
  }
  else if(morseCode=="--.-"){
  	return 'q';  
  }
  else if(morseCode==".-."){
  	return 'r';  
  }
  else if(morseCode=="..."){
  	return 's';  
  }
  else if(morseCode=="-"){
  	return 't';  
  }
  else if(morseCode=="..-"){
  	return 'u';  
  }
  else if(morseCode=="...-"){
  	return 'v';  
  }
  else if(morseCode==".--"){
  	return 'w';  
  }
  else if(morseCode=="-..-"){
  	return 'x';  
  }
  else if(morseCode=="-.--"){
  	return 'y';  
  }
  else if(morseCode=="--.."){
  	return 'z';  
  }
  else{
    return '?';
  }
  
}
