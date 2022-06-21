#include <Servo.h>
#define LED 0
#define Push_Button 4


bool Button_Pressed_Flag;
unsigned long Press_Duration;
unsigned long Begin_Time;
unsigned long Last_Press_Time;
bool Charcter_Flag;
bool New_Word_Flag;
String Morse_Code = "";
Servo servo;
int angle[20];
int j = 0;
float previousangle = 0; 
String words;
void setup() 
{
  pinMode(Push_Button, INPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  servo.attach(2,430,2400); //D3
  servo.write(0);
  delay(10);
}

void loop()
{   
    char Flag_from_unity = Serial.read(); 

    if (Flag_from_unity == '2')
    digitalWrite(LED, HIGH);

    if (Flag_from_unity == '3')
    digitalWrite(LED, LOW);
    
    Check_Button_State();  
    
    if( millis() - Last_Press_Time > 2000 && New_Word_Flag == true )
    {
        //servoangle(angle);
        //Serial.println();
        //Serial.println("New word");
        //delay(100);
        Serial.println(words);
        for (int t=0; t<20;t++) angle[t] = 0;
        j=0; 
        New_Word_Flag = false;
        Morse_Code = "";
        words ="";
    }
    
    else if( millis() - Last_Press_Time > 750 && Charcter_Flag == true) 
    {
        DecodeMorse_Code(Morse_Code);
        Charcter_Flag = false;
        Morse_Code ="";
        //Serial.println();
    }
    
}



void Check_Button_State() 
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



void Button_Released() 
{
     
    Press_Duration = millis() - Begin_Time;
    digitalWrite(LED, LOW);
    if( Press_Duration > 25 ) 
    {
        
        if( Press_Duration >= 40 && Press_Duration <= 180 )
        { 
              Morse_Code += '.';
             // Serial.print(". ");
        }
        else  
        {
              Morse_Code += '-';
              //Serial.print("- ");
        }
   }

   
}




void DecodeMorse_Code(const String& Morse_Code)
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
  //Serial.print(" ");            
  for (int i=0; i<26;i++)
  {
  
     if (String(letters[i]) == Morse_Code)
     {
         //Serial.print( "ASCII: ");
         //Serial.print(letter[i]);
         angle[j]= i;  
         words += letter[i];
         j++;    
         Error_Flag =1;
         char buffer[6];
         unsigned char character = letter[i];
         sprintf(buffer, "%02X", character);
         //Serial.print( " Hex: ");
         //Serial.print(buffer);
         break; 
     }
    
  }
  
  if (Error_Flag==0) 
  Serial.println("Invalid Morse Code");
  
}


void servoangle(int angle[])
{
  static int hex[27][2] = {4,1,4,2,4,3,4,4,4,5,4,6,4,7,4,8,4,9,4,10,4,11,4,12,4,13,4,14,4,15,5,0,5,1,5,2,5,3,5,4,5,5,5,6,5,7,5,8,5,9,5,10,2,0};  //Hex code from A to Z 
  
  int k=0; 

  angle [j]= 26;
  j = j+1;
  while(k < j)
  {
      int z = angle[k];
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
       k++;
     
   }
 } 
