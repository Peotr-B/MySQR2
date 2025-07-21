/*
21июл25
Подключил ЦАП MCP4725
Файл <MySQR2-2.ino>

20июл25
Подключил инкрементный энкодер WAVGAT:
нажатие увеличивает кратность шага регулировки частоты: 1-10-100-1000-1
(см. файл My2_Gen_MCP4725.ino)

29июн25
https://musbench.com/all/arduino-sqr-gen/
Точный генератор 1гц — 8 МГц на Arduino с управлением от компьютера
Управляется программой ArdSQrGen_CSharp.exe (см. в папке со скетчем)

см. также:
https://community.alexgyver.ru/threads/ochen-prostoj-generator-iz-arduiny.4268/
Очень простой генератор из ардуины
Генератор частоты от 1 Гц до 8 000 000 Гц. Вырабатывает однополярный меандр со 
скважность 2. 
<generator1-8.ino>

3июл25
В ИТОГЕ
Соединил два проекта: 
https://microkontroller.ru/arduino-projects/generator-signalov-pryamougolnoj-i-sinusoidalnoj-formy-na-arduino/
https://circuitdigest.com/microcontroller-projects/arduino-waveform-generator
DIY Waveform Генератор с использованием Arduino
<Waveform_Generator.ino>
и
https://musbench.com/all/arduino-sqr-gen/
Точный генератор 1гц — 8 МГц на Arduino с управлением от компьютера
<generator1-8.ino>

Есть много идей, но НЕКОГДА.
В соответствии с этим обратить внимание на:
https://psseven.ru/2025/02/03/%D1%81%D0%BE%D0%B7%D0%B4%D0%B0%D0%BD%D0%B8%D0%B5-%D0%BF%D1%80%D0%BE%D1%81%D1%82%D0%BE%D0%B3%D0%BE-%D0%B3%D0%B5%D0%BD%D0%B5%D1%80%D0%B0%D1%82%D0%BE%D1%80%D0%B0-%D1%81%D0%B8%D0%B3%D0%BD%D0%B0%D0%BB/?ysclid=mciul1ek5i767162742
Создание простого генератора сигналов своими руками 
!!!!!Создание простого генератора сигналов своими руками
<Gen_MCP4725.ino>

https://cxem.net/arduino/arduino62.php?ysclid=mchg3l1c4875939445
Генератор сигналов на Arduino
function_generator.ino>

 Итак
 <MySQR2.ino>
 
Схему подключения см. на рисунке
Использован Arduino Leonardo
Меандр и одновремённо стробирующий сигнал снимается с пина 9
Внешнее прерывание производится с пина 7, на который заведён стробирующий сигнал
С пина 5 снимается синусоидальный сигнал в виде ШИМ
Для выделения синусоиды из ШИМ к пину 5 подключается RC-фильтр:
C = 22 мкФ
R = подстроечный / переменный резистор 1 кОм (установил 70 Ом)

20июл25
Подключил:
ЖК-модуль IIC/I2C 1602 для arduino 1602 LCD UNO r3 mega2560 LCD 1602
*/
//#define Sine_pin 5

#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Подключение библиотеки
LiquidCrystal_I2C lcd(0x27,16,2); // Указываем I2C адрес (наиболее распространенное
// значение), а также параметры экрана (в случае LCD 1602 - 2 строки по 16 символов
// в каждой
#include <Adafruit_MCP4725.h> // MCP4725 library from adafruit
Adafruit_MCP4725 dac; 

static uint32_t UartFreq = 0; //Переменная для задания нужной частоты
uint32_t compare = OCR1A;  //OCR1A - Регистры сравнения A выхода таймера/счетчика1
uint16_t divider = 1;  //переменная коэфф. деления прескалера
byte Lshift[] = {3,3,2,2};
double angle = 0;
//const int signal_pin = 9;  
const int Sine_pin = 5;
//const int Strob_pin = 7; //Контакт для подключения стробирующего сигнала (здесь 
//от pin = 9)
double increment = 0.2;
float freqency = 0;
float freq_old = 0;		//Для регистрации изменения частоты

const int Encoder_OuputA  = 11;		//S1
const int Encoder_OuputB  = 12;		//S2
const int Encoder_Switch = 10;		//KEY
int Previous_Output;
int multiplier = 1;
unsigned long lastButtonPress = 0;    // Создаем переменную lastBut для устранения дребезга
unsigned long Pause = 0; 

volatile bool intFlag = false;   // флаг: https://alexgyver.ru/lessons/interrupts/?ysclid=mdbe515w8a409736619

//=========================================================================
void setup() 
{
  Serial.begin(115200);
//=======================================================================
//Вместо приветствия :)

  lcd.init();                      // Инициализация дисплея
  lcd.clear();
  lcd.backlight();                 // Подключение подсветки
  lcd.setCursor(0,0);              // Установка курсора в начало первой строки
  lcd.print("Generator");       // Набор текста на первой строке
  lcd.setCursor(0,1);              // Установка курсора в начало второй строки
  lcd.print("1Hz - 8MHz");       // Набор текста на второй строке

byte tries = 20;
delay(5000);
//Serial.println("Вставка для индикации названия скетча при запуске: <My_Scetch.ino>");
  Serial.println("Демонстрационный генератор на Arduino с управлением от ПК: <16_MySQR2-2.ino>");
  Serial.println("Состав: Arduino Leonardo, ЦАП MCP4725, ЖК-модуль IIC/I2C 1602, энкодер WAVGAT");
  while (--tries)
	{
		delay(500);               // Пауза 500 мс
        Serial.print(".");        // Печать "."
		Serial.print(tries);	  // Печать ожидания пуска программы
	}
 
 Serial.println();
 Serial.println("Введите значение частоты меандра числом в герцах");
 Serial.println("Значение синуса будет меньше в 16 раз");
 Serial.println("Или поворачивай ручку энкодера");
 Serial.println("Кратность шага поворота энкодера по кнопке: х1-х10-х100-х1000-х1");
 Serial.println(" =========================");
 Serial.println();
//=======================================================================
  
  dac.begin(0x60); // Default I2C Address of MCP4725 breakout board (sparkfun) 
  
  Previous_Output = digitalRead(Encoder_OuputA);
  pinMode (9, OUTPUT); // выход генератора
  pinMode (5, OUTPUT); // выход ШИМ
  //Регистр управления А таймера/счетчика1
  TCCR1A=0;
  //Регистр управления B таймера/счетчика1
  TCCR1B=0;
  attachInterrupt(4,generateSignal,CHANGE); //Внешнее прерывание от сигнала, поступающего на pin = 7)
 //см. https://alexgyver.ru/lessons/interrupts/?ysclid=mcne1qxpy3966705587
 }

void loop() 
{
  static uint32_t UartF = 0; //Переменная для задания нужной частоты
  
  if (intFlag) 
  {
    intFlag = false;    // сбрасываем
    double sineValue = sin(angle);
    sineValue *= 255;
    int plot = map(sineValue, -255, +255, 0, 255);
   // Serial.println(plot);   //Для контроля
   
    dac.setVoltage(plot, false);
   
    analogWrite(Sine_pin,plot);	// Выход ШИМ
    angle += increment; 
    if (angle > (2*PI))
       angle =0;
  }
  
  if (Serial.available() > 0)
  { 
    UartF = Serial.parseInt(); //получаем значение частоты из сообщения UART
	
     if (UartF <= 0 || UartF > F_CPU/2) //Проверяем допустимый диапазон значений частоты
      return;
	 UartFreq = UartF;
	 F_Gen();
	 }
 
 if (freqency != freq_old)
	 LCD();
 
  /* Энкодер */
 
  if (digitalRead(Encoder_OuputA) != Previous_Output)
   {
     if (digitalRead(Encoder_OuputB) != Previous_Output) 
     { 
      // freqency = freqency + multiplier;
	   UartFreq = UartFreq + multiplier;
       Serial.print("UartFreq = ");
	   Serial.println(UartFreq);	  
     } 
     else 
     {
     //  freqency = freqency - multiplier;
	   UartFreq = UartFreq - multiplier;
       Serial.print("UartFreq = ");
	   Serial.println(UartFreq);	
     }
	 
	 F_Gen();
   }
  
      if (digitalRead(Encoder_Switch) == 0)	//ВНИМАНИЕ! Ввести задержку
	  // как в https://robotchip.ru/podklyuchenie-enkodera-ky-040-k-arduino/
   {
	     if (millis() - lastButtonPress > 50) // Если состояние LOW в течении 50 мкс, кнопка нажата 
	{ 
      multiplier = multiplier * 10;
	  if (multiplier > 1000)
		  multiplier = 1;
	  Serial.print("multiplier = ");
	  Serial.println(multiplier);	
    }
    lastButtonPress = millis();
  //  delay(1);                               // Пауза   
  
  // Pause
  if (millis() - Pause > 500)
  {
	  if(digitalRead(Encoder_Switch) != 0)
	  {
		  Previous_Output = digitalRead(Encoder_OuputA);
		  Pause = millis();
	  }	  
  }
   }
   Previous_Output = digitalRead(Encoder_OuputA);  
}

//====== ПОДПРОГРАММЫ ==============================================>>>
void generateSignal()
{
   intFlag = true;   // подняли флаг прерывания
}

void LCD()
{
	freq_old = freqency;
	lcd.clear();
	
	lcd.setCursor(0,0);       // Установка курсора в начало первой строки

  if(freqency < 1000)
 {
    lcd.print("Freqency, Hz =");  // Набор текста на первой строке
    lcd.setCursor(0,1);       // Установка курсора в начало второй строки
    lcd.print((uint32_t) freqency);
  }

  else
  {
    lcd.print("Freqency, kHz =");  // Набор текста на первой строке
    lcd.setCursor(0,1);       // Установка курсора в начало второй строки
    lcd.print( freqency / 1000);
  }
}

void F_Gen()
{
  //  if (UartFreq <= 0 || UartFreq > F_CPU/2) //Проверяем допустимый диапазон значений частоты
  //    return;
  
  Serial.print("UartFreq= ");
  Serial.println(UartFreq);
  
/*
F_CPU — макроопределение в программировании микроконтроллеров AVR, которое задаёт 
тактовую частоту микроконтроллера в герцах. Например, в файле Makefile может быть 
задано значение: F_CPU = 16000000UL — частота задана 16 МГц.
*/
    compare = (F_CPU / UartFreq /2 /divider); //вычисляем нужное значение OCR
    for (byte i = 0; i < 4; i++) 
	{
      if (compare > 65536) 
	  { 
        divider <<= Lshift[i];
        compare = F_CPU / UartFreq /2 /divider; 
	  }
      else 
	  {
        TCCR1B = (i + 1) | ( 1 << WGM12 ); 
        break; }  // CTC режим работы таймера
      } 
    OCR1A = compare-1; 
    TCCR1A = 1 << COM1A0;
	
    freqency = F_CPU / 2 / (OCR1A + 1) / divider;
	Serial.print("Частота меандра (строба) = ");
    if (freqency < 1000) 
	{
		Serial.print(freqency, 0); 
		Serial.println(" Hz ");
	}
    if (freqency >= 1000)
		{
			Serial.print(freqency / 1000); 
			Serial.println(" kHz");
		}
	Serial.println(" --------------------------------");
	
	Serial.print("Частота синуса = ");
		 if (freqency < 1000) 
	{
		Serial.print(freqency/16, 0); 
		Serial.println(" Hz ");
	}
    if (freqency >= 1000)
		{
			Serial.print(freqency/16000); 
			Serial.println(" kHz");
		}
}
// <<<================================================ ПОДПРОГРАММЫ ===
