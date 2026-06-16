#include <Wire.h>
#include<LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define dc_mtr_pin 6
#define clk 3
#define dt 5
#define pulse 2
#define sw 7

unsigned long t1=0;  //time start for calculating rpm
unsigned long tb = 0; // debounce time start
byte pre_enc_val=0;  // previous value of encoder
volatile byte enc_val=0;  //encoder value
volatile int ir_pulse_val=0; //no. of pulses from IR sensor
byte cur_dt,cur_clk; //current status of data and clock pins
unsigned int speed=0; //speed of dc motor measured with Ir sensor
bool backlight_on=false; //is backlight switched on
bool cur_sw=0; //current status of sw
byte tb_flg=0; //debounce time flag
uint16_t num_pulses=1; //number of pulses per rotation in encoder of dc motor, follow README.md file

void ir_pulse(){
  ir_pulse_val++;
}
void enc_pulse(){
    cur_dt=digitalRead(dt);
    cur_clk=digitalRead(clk);
    if(cur_clk==cur_dt){
      if(enc_val>0)
        enc_val--;
    }
    else{
      if(enc_val<255)
        enc_val++;
    }
}

void setup() {
  Serial.begin(9600);
  pinMode(dc_mtr_pin, OUTPUT);
  pinMode(clk,INPUT);
  pinMode(dt,INPUT);
  pinMode(pulse, INPUT);
  pinMode(sw, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  backlight_on=true;
  lcd.setCursor(0, 0);             
  lcd.print("MOTOR DASHBOARD");

  attachInterrupt(digitalPinToInterrupt(pulse),ir_pulse,RISING);
  attachInterrupt(digitalPinToInterrupt(clk),enc_pulse,CHANGE);
}

void loop() {

  if(pre_enc_val!=enc_val){
    analogWrite(dc_mtr_pin,enc_val);
    Serial.println("encoder:");
    Serial.println(enc_val);
    Serial.println("speed:");
    Serial.println(speed);
    lcd.setCursor(0, 1);             
    lcd.print("enc:");
    lcd.setCursor(4, 1);
    lcd.print("    ");
    lcd.setCursor(4, 1);             
    lcd.print(enc_val);
    pre_enc_val=enc_val;
  }
  cur_sw=digitalRead(sw);
  if(cur_sw==1){
    if(tb_flg==0){
      tb=millis();
      tb_flg=1;
    }
    if(tb_flg==1 && millis()-tb>=50){
      if(backlight_on==true){
        lcd.noBacklight();
        backlight_on=false;
      }else{
        lcd.backlight();
        backlight_on=true;
      }
      tb_flg=2;
    }
  }else{
    tb_flg=0;
  }
  if(millis()-t1>=1000){
    noInterrupts();
    speed=ir_pulse_val*60UL/num_pulses;
    ir_pulse_val=0;
    interrupts();
    t1=millis();
    lcd.setCursor(8, 1);             
    lcd.print("rpm:");
    lcd.setCursor(12, 1);             
    lcd.print("    ");
    lcd.setCursor(12, 1);             
    lcd.print(speed);
  }

}
