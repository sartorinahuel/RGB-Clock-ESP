#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_NeoPixel.h>

#define PIN 8
#define NUMPIXELS 60

RTC_DS3231 rtc;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

int brillo=100;
int dia;
int mes;
int horas;
int minutos;
int segundos;
int hArrow=5;
int i;
bool tarde = false;
bool step = false;
int hm;
DateTime date;

int buzzer = 5;

void setup()
{
  pinMode(buzzer, OUTPUT);
  
  //Buzzer test
  digitalWrite(buzzer, 100);
  delay(20);
  digitalWrite(buzzer, 0);
  
  Serial.begin(9600);
  delay(1000); 
 
   if (!rtc.begin()) {
      Serial.println(F("Couldn't find RTC"));
      while (1);
   }
 
   // Si se ha perdido la corriente, fijar fecha y hora
   if (rtc.lostPower()) {
      // Fijar a fecha y hora de compilacion
      rtc.adjust(DateTime(2020, 1, 1, 12, 0, 0));
      
      // Fijar a fecha y hora específica. En el ejemplo, 21 de Enero de 2016 a las 03:00:00
      // rtc.adjust(DateTime(2016, 1, 21, 3, 0, 0));
   }
//  inicializamos el STRIP
  strip.begin();
  strip.setBrightness(brillo);
 colorTest();
}

void loop()
{
  strip.setBrightness(brillo);
  strip.show();
  date = rtc.now();
  ledClock();
  
}

void colorTest(){
  Serial.print("Color test");
  for(i=0;i<60;i++){
  strip.setPixelColor(i, 0, 0, 255);
  strip.show();
  delay(5);
  }
  
  delay(500);
  for(i=0;i<60;i++){
  strip.setPixelColor(i, 0, 255, 0);
  strip.show();
  delay(5);
  }
  delay(500);
  for(i=0;i<60;i++){
  strip.setPixelColor(i, 255, 0, 0);
  strip.show();
  delay(5);
  }
  delay(500);
  for(i=0;i<60;i++){
  strip.setPixelColor(i, 0, 0, 0);
  strip.show();
  delay(20);
  }
}

void ledClock(){

  Serial.print(date.day());
  dia = date.day();
  Serial.print("/");
  Serial.print(date.month());
  mes = date.month();
  Serial.print("/");
  Serial.print(date.year());
  Serial.println();
  Serial.print(date.hour());
  horas = date.hour();
  Serial.print(":");
  Serial.print(date.minute());
  minutos = date.minute();
  Serial.print(":");
  segundos = date.second();
  Serial.println(date.second());

  if(dia == 31){
    if(mes == 12){
      if(horas == 23){
        if(minutos == 59){
          if(segundos == 50){
            newYear();
          }
        }
      }
    }
  }
  
  step = false;
    
  hArrow = 5;

  if (horas > 12){
    horas = horas - 12;
    tarde = true;
  }
   if (tarde = false){
  strip.setPixelColor(0, 255, 255, 255); //prende LED como PM
    strip.show();
  }else{
  strip.setPixelColor(0, 0, 0, 0); //prende LED como AM
  strip.show();
  }
  
  hm = horas * 5;
  
  for (i=1;i<60;i++){ //probar con i<59
    strip.setPixelColor(i, 0, 0, 0);
    strip.show();
    if (hArrow == i){
      if (hArrow <= hm){
        if (hArrow <= minutos){
          strip.setPixelColor(i, 255, 0, 255);
          strip.show();
          hArrow = hArrow + 5;
          step = true;
        }else{
          strip.setPixelColor(i, 255, 0, 0);
          strip.show();
          hArrow = hArrow + 5;
          step = true;
        }
      }
    }
    if (step == false){
      if (minutos >= i){
        strip.setPixelColor(i, 0, 0, 255);
        strip.show();
      }
    }
   step = false;
   if (segundos==i){
      strip.setPixelColor(i, 0, 255, 0);
      strip.show();
    }
  }
}

void newYear() {
  strip.setBrightness(100);
  strip.show();
  for(int ny=0;ny<10;ny++){
    for(i=0;i<60;i++){
      strip.setPixelColor(i, 255, 255, 255);
      strip.show();
      delay(10);
      }
  digitalWrite(buzzer, 100);
  delay(20);
  digitalWrite(buzzer, 0);
  delay(380);
  for(i=0;i<60;i++){
      strip.setPixelColor(i, 0, 0, 0);
      strip.show();
      }
   }
  digitalWrite(buzzer, 100);
  for(int j=0;j<5;j++){
     for(i=0;i<60;i++){
         strip.setPixelColor(i, 255, 255, 255);
         strip.show();
         }
     delay(300);
     for(i=0;i<60;i++){
         strip.setPixelColor(i, 0, 0, 0);
         strip.show();
         }
     delay(300);
  }
  digitalWrite(buzzer, 0);
  setup();
}
