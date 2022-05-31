#include <Servo.h>
#define LED 0
#define Push_Button 4

  #include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(5, 16);                    // connect CE, CSN to 5 and 16
const byte ID[][6] = {"00001", "00002"}; 
String re="";
bool Button_Pressed_Flag;
unsigned long Press_Duration;
unsigned long Begin_Time;
unsigned long Last_Press_Time;
bool Charcter_Flag;
bool New_Word_Flag;
String Morse_Code = "";
Servo servo;
int angle[20];
int transmitting_angle[20];
int j = 0;
float previousangle = 0; 
bool radio_flag;
String hex_word="";
String total_morse;
void setup() 
{
  pinMode(Push_Button, INPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  servo.attach(2,430,2400); //D3
  servo.write(0);
  delay(10);
  for (int t=0; t<20;t++) angle[t] = 999;
  radio.begin();
  radio.openWritingPipe(ID[1]); // 00002
  radio.openReadingPipe(1, ID[0]); // 00001
  radio.setPALevel(RF24_PA_MIN);
}

void loop()
{
    Check_Button_State();                                                     //attach motor to D2
    
    if( millis() - Last_Press_Time > 2000 && New_Word_Flag == true )         // execute the word pressed after 2000 ms
    {
        int kk=0;
        //delay(100);
        radio.stopListening();                                              // stop the recieving communication to start the transmission
        //while(angle[kk] != 999)
        //{ int ll = angle[kk];
        radio.write(&hex_word, sizeof(hex_word));
        
        delay(500);
        //kk++;
        //}
        //delay(100);
        //Serial.println();
        Serial.print("Code sent: ");
        Serial.println(total_morse);
        Serial.println("New word");
        hex_word = "";
        total_morse = "";
        //servoangle(angle);
        for (int t=0; t<20;t++) angle[t] = 999;
        j=0; 
        New_Word_Flag = false;
        Morse_Code = "";
    }
    
    else if( millis() - Last_Press_Time > 750 && Charcter_Flag == true)     // execute the corresponding letter pressed within 750 ms.
    {
        total_morse += Morse_Code ;
        total_morse += " ";
        //hex_word += " "; 
        DecodeMorse_Code(Morse_Code);
        Charcter_Flag = false;
        Morse_Code ="";
        Serial.println();
    }
    
    delay(5);
if (New_Word_Flag== false){
   radio.startListening();                                                      // start the recieving communication and stop the transmission
   if ( radio.available())
   {
    delay(50);
    //Serial.println("0");
    while (radio.available())
    {
      //Serial.println("1");
      Serial.println("Recieved Hexa Decimal Code from Mahima:");
    //int Recieved_Angle[20];
    re="";
    radio.read(&re, sizeof(re));
    Serial.println(re);
    //int kk = 0;
    //while(Recieved_Angle[kk] != 999)
    //{Serial.println(Recieved_Angle[kk]);  
    //kk++;
    //}
     
    delay(500);
    servoangle();
    delay(20);
  
    }
   }
}
}





void Check_Button_State()                                                         // to check the pressed state of button 
{
{
   if( ! Button_Pressed_Flag && digitalRead(Push_Button) == HIGH ) 
   {
        Button_Pressed_Flag = true;
        // start timer
        Begin_Time = millis();
        Last_Press_Time = Begin_Time;
        Press_Duration = 0;
        Charcter_Flag = true;
        New_Word_Flag = true;
       digitalWrite(LED, HIGH);
 
   }
   
   if( Button_Pressed_Flag && digitalRead(Push_Button) == LOW ) 
   { 
      Button_Released();
      Button_Pressed_Flag = false;
      Last_Press_Time = millis();
   }
}



void Button_Released()                                                              //this function returns "." or "-" based on the duration of button pressed
{
     
    Press_Duration = millis() - Begin_Time;
    digitalWrite(LED, LOW);
    if( Press_Duration > 25 ) 
    {
        
        if( Press_Duration >= 40 && Press_Duration <= 180 )
        { 
              Morse_Code += '.';
              Serial.print(". ");
        }
        else  
        {
              Morse_Code += '-';
              Serial.print("- ");
        }
   }

   
}




void DecodeMorse_Code(const String& Morse_Code)                           // this function returns the corresponding ASCII and Hex value of morsecode
{
  static char letters[][5] = {".-", "-...", "-.-.", "-..", ".",
                              "..-.", "--.", "....", "..", ".---",
                              "-.-", ".-..", "--", "-.", "---",
                              ".--.", "--.-", ".-.", "...", "-",
                              "..-", "...-", ".--", "-..-",
                              "-.--", "--.."};
  static char letter[26] ={'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
                            'Q','R','S','T','U','V','W','X','Y','Z'};

  int Error_Flag=0;  
  //Serial.println();                          
  //Serial.print("DECODED Alphabet: ");
  Serial.print(" ");            
  for (int i=0; i<26;i++)
  {
  
     if (String(letters[i]) == Morse_Code)
     {
         //Serial.print( "ASCII: ");
         //Serial.print(letter[i]);
         angle[j]= i;  
             
         Error_Flag =1;
         char buffer[6];
         unsigned char character = letter[i];
         sprintf(buffer, "%02X", character);
         //Serial.print( " Hex: ");
         //Serial.print(buffer);
         hex_word += buffer; 
         j++;
         break; 
     }
    
  }
  
  if (Error_Flag==0) 
  Serial.println("Invalid Morse Code");
  
}


void servoangle()                                   // this function returns the angle which indicates corresponding hexadecimal value
{
  static int hex[27][2] = {4,1,4,2,4,3,4,4,4,5,4,6,4,7,4,8,4,9,4,10,4,11,4,12,4,13,4,14,4,15,5,0,5,1,5,2,5,3,5,4,5,5,5,6,5,7,5,8,5,9,5,10,2,0};  //Hex code from A to Z 
  static char hexcode[27][2] = {'4','1','4','2','4','3','4','4','4','5','4','6','4','7','4','8','4','9','4','A','4','B','4','C','4','D','4','E','4','F','5','0','5','1','5','2','5','3','5','4','5','5','5','6','5','7','5','8','5','9','5','A','2','0'};  int k=0; 
 int kkk=0; int m= 0; int ty;
  while(re[m] != '\0')
  {
     
    //Serial.println(re[kkk]);
     for( ty=0; ty<27;ty++)
        if(hexcode[ty][0] == re[m])
           if(hexcode[ty][1] == re[m+1])
           {break;}

     m=m+2;
     int z = ty;
      if(previousangle == hex[z][0])
      {
            servo.write((hex [z][0] * 11.25)+1.625);
            delay(500);
            servo.write((hex [z][0] * 11.25)+5.625);
            delay(1000);
          
       }
       else
       { 
             servo.write((hex [z][0] * 11.25)+5.625);
             delay(1000);
       }
         
       previousangle = hex[z][0];
       
       if(previousangle == hex[z][1])
       {
          servo.write((hex [z][1] * 11.25)+1.625);
          delay(500);
          servo.write((hex [z][1] * 11.25)+5.625);
          delay(1000);
       }
       
       else
       { 
       
        servo.write((hex [z][1] * 11.25)+5.625);
        delay(1000);
        
       }
       
       previousangle = hex[z][1];
       kkk++;

    }
    
    }

  
