#include <avr/io.h>
#include "DHT.h"
#include "lcd.h"

#define HUMIDIFIER_PIN PC1
#define DEHUMIDIFIER_PIN PC2
#define ROTOR_CW_PIN PC3
#define ROTOR_CCW_PIN PC4
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
//START
unsigned long sec_ran =0;
unsigned long min_ran = 0;
unsigned long hours_ran =0;
unsigned long days_ran = 0;
unsigned long weeks_ran = 0;
unsigned long rotor_start_ms =0;



///pid functions begin
const float	K_p = 12;
const float	K_i = 0.01;
const float	K_d = 5;

//float P_v[]= {23.0, 26.5, 29.5, 31.5, 33.5,35.5,37.5, 38, 39, 38.5, 38 ,37.5,37.0};
float S_v =37.2;


float	Error = 0;
float	Prev_error = 0;
float	Integral = 0;
float	Derivative = 0;
unsigned long prev_time = 0;
float PID( float P_v)
{
		unsigned long	curr_time = millis();
      sec_ran =(curr_time/1000);
      min_ran = (sec_ran/60);
      hours_ran =(min_ran/60);
      days_ran = (hours_ran/24);
      //days_ran = 18;
      weeks_ran = (days_ran/7);

      float dt_ms = (curr_time - prev_time);
		float	dt = dt_ms/1000;
		float M_v = 0;

		Error = S_v - P_v;

    float P = K_p * Error;

    Serial.println("prev_time");
    Serial.println(prev_time);
    Serial.println("curr_time");
    Serial.println(curr_time);

    Integral += 0.5 * (Prev_error + Error) * dt;
    float I = K_i * Integral;

    Derivative = (Error - Prev_error) / dt;
    float D = K_d * Derivative;

    M_v = P + I + D;

    //Serial.println("P: ");
    //Serial.println(P);
    //Serial.println("I: ");
    //Serial.println(I);
    //Serial.println("D: ");
    //Serial.println(D);
    //Serial.println("dt: ");
    //Serial.println(dt);
    //Serial.println("dt ms: ");
    //Serial.println(dt_ms);
    //Serial.println("\n");

    if (M_v<0){
        M_v =0;
        Integral -= 0.5 * (Prev_error + Error) * dt;
		}
    else if(M_v>255){
        M_v= 255;
        Integral -= 0.5 * (Prev_error + Error) * dt;
		}
   if (P_v >=37.5)
   {
      M_v = 0;
   }
    Prev_error = Error;
    prev_time = curr_time;
   
		return M_v;
		
    
}

//pid functions end


//END
const int DHTPin = A0;     // what digital pin we're connected to

DHT dht(DHTPin, DHTTYPE);


int humidifying_status = 0;
int dehumidifying_status =0;
int rotor_direction = 0;
int running = 0;

void humidify()
{
 PORTC |= (1<<HUMIDIFIER_PIN);
 humidifying_status =1;

}
void stop_humidify()
{
   PORTC &= ~(1<<HUMIDIFIER_PIN);
   humidifying_status = 0;
}
void dehumidify()
{
   PORTC |= (1<<DEHUMIDIFIER_PIN);
   dehumidifying_status = 1;
}
void stop_dehumidify()
{
   PORTC &= ~(1<<DEHUMIDIFIER_PIN);
   dehumidifying_status = 0;
}
void rotor_start(int ROTOR_PIN)
{
   PORTC |= (1<<ROTOR_PIN);
   running = 1;

}
void rotor_stop(int ROTOR_PIN)
{
   PORTC &= ~(1<<ROTOR_PIN);
   running = 0;
   
}


void Pwm_init(void)
{
   DDRD |= (1<<PD3);
   TCCR2A |= (1<<WGM20) |(1<<WGM21);//FAST PWM
   TCCR2A |= (1<<COM2B1);//NON INVERTING MODE
   TCCR2B |= (1<<CS22);//PRESCAKER 64
}
void Pwm_set(int duty){
   OCR2B = duty;
}


void setup()
{
   //Serial.begin(9600);
   //Serial.println("DHTxx test!");
   Error = 0;
	Prev_error = 0;
	Integral = 0;
	Derivative = 0;
   DDRC |= ((1<<HUMIDIFIER_PIN)|(1<<DEHUMIDIFIER_PIN)|(1<<ROTOR_CW_PIN)|(1<<ROTOR_CCW_PIN));
   LCD_Init();
   LCD_String("Initializing ...");
   delay( 100 );
   
   dht.begin();
   prev_time = millis();
   Pwm_init();
}

char buffer[10];
void loop() 

{

// Reading temperature or humidity takes about 250 milliseconds!
   float h = dht.readHumidity();
   float t = dht.readTemperature();

   float M_v = PID(t);
   Pwm_set(M_v);

   if (days_ran < 18)
   {
      if (h<=51)
         {
            //if (!humidifying_status)
            //{
               humidify();
               //stop_dehumidify();
            //}
         }
      else if (h>=54)
      {
         //if (!dehumidifying_status)
         //{
            stop_humidify();
            //dehumidify();
         //}
      }
   
      if (!(hours_ran%3))
      {
         if ((hours_ran *60) == min_ran)
         {
            if (sec_ran <= 60 *min_ran +30)
            {
               if (!running)
               {
                  if (rotor_direction)
                  {
                     rotor_start(ROTOR_CW_PIN);
                  
                  }

                  else
                  {
                     rotor_start(ROTOR_CCW_PIN);
                  }
               }
            }
            else{
               if (running)
               {
                  rotor_stop(ROTOR_CW_PIN);
                  rotor_stop(ROTOR_CCW_PIN);
                  if (!(hours_ran%2))
                  {
                     rotor_direction = !rotor_direction;
                  }
                  
               }
               
            }
         }
      }
   }
   else 
   {
      rotor_stop(ROTOR_CW_PIN);
      rotor_stop(ROTOR_CCW_PIN);

       if (h<=66)
         {
            //if (!humidifying_status)
            //{
               humidify();
               //stop_dehumidify();
            //}
         }
      else if (h>=69)
      {
         //if (!dehumidifying_status)
         //{
            stop_humidify();
            //dehumidify();
         //}
      }
   }
//   else{rotor_stop();}

   if (isnan(h) || isnan(t)) {
      //Serial.println("Failed to read from DHT sensor!");
      LCD_String("Failed to read from DHT sensor!");
      return;
   }
    //Serial.println(prev_time);

   // Serial.println(x);

   LCD_Clear();
   //Serial.print("Humidity: ");
   //LCD_String("Humidity: ");
   //Serial.print(h);
   //itoa(h,buffer,10);
   //LCD_String(buffer);
   //LCD_String(".");
   //int x = (h*10);
   //int n = x%10;
   //itoa(n,buffer,10);
   //LCD_String(buffer);
   //Serial.print(" %\t");
   //LCD_Cmd(0xC0);
   //Serial.print("Temperature: ");
   LCD_String("Temp: ");
   //Serial.print(t);
   itoa(t,buffer,10);
   LCD_String(buffer);
   LCD_String(".");
   int x = (t*10);
   int n = x%10;
   itoa(n,buffer,10);
   LCD_String(buffer);
   //Serial.println(" *C ");
   
   LCD_Char(223);
   LCD_String("C ");

   LCD_Cmd(0xC0);
   //Serial.print("M_V: ");
   LCD_String("M_V: ");
   //Serial.print(h);
   itoa(M_v,buffer,10);
   LCD_String(buffer);
   LCD_String(".");
   x = (M_v*10);
   n = x%10;
   itoa(n,buffer,10);
   LCD_String(buffer);

  _delay_ms(52);
   //LCD_Clear();
   //delay( 500 );
   




}