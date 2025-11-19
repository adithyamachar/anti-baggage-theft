#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
int flag=0;
int alert=0;
int led=4;
// Create a radio object
RF24 radio(7,8); // CE, CSN pins

// Address through which two modules communicate
const byte address[6] = "00001";

const int buttonPin = 6; // Pin where the button is connected

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP); // Set the button pin as input with internal pull-up
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  digitalWrite(buttonPin,HIGH);
}

void loop() {
    radio.openWritingPipe(address);

  if(digitalRead(buttonPin)==HIGH)
  {
    flag++;
  }
 // delay(50);
   
  if (flag==1) { // Check if the button is pressed
    const char text[] = "activate";
      radio.stopListening(); // Stop listening to be able to transmit

    radio.write(&text, sizeof(text));
    Serial.println("Button pressed");
    delay(500); // Debounce delay
    flag++;

  }
 else if(flag==3) { // Check if the button is pressed
    const char text[] = "deactivate";
      radio.stopListening(); // Stop listening to be able to transmit

    radio.write(&text, sizeof(text));
    delay(500); // Debounce delay
    flag=0;

  }



  radio.startListening(); 
    if (alert==1)
{
  blinkimpact();
}// Start listening again
  receive();



}

  
void receive() {
    radio.openReadingPipe(0, address);

  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);

    if (strcmp(text, "activate") == 0) {
      Serial.println("Received activate command");
      blinkactivate();
    } else if (strcmp(text, "deactivate") == 0) {
      Serial.println("Received deactivate command");
      alert=0;
      blinkdeactivate();
    }
    else if (strcmp(text, "impact") == 0) {
      alert=1;
    }
  }
}

void blinkactivate()
{
  digitalWrite(led,HIGH);
  delay(300);
  digitalWrite(led,LOW);
    delay(300);
   digitalWrite(led,HIGH);
  delay(300);
  digitalWrite(led,LOW);
    delay(300);
   digitalWrite(led,HIGH);
  delay(300);
  digitalWrite(led,LOW);
    delay(300);

}

void blinkimpact()
{
  digitalWrite(led,HIGH);
  delay(100);
  digitalWrite(led,LOW);
  delay(100);
}

void blinkdeactivate()
{
  digitalWrite(led,HIGH);
  delay(3000);
  digitalWrite(led,LOW);
  
}
