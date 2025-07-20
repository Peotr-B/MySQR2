/*
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
*/
//#define Sine_pin 5

byte Lshift[] = {3,3,2,2};


double angle = 0;
const int signal_pin = 9;  
const int Sine_pin = 5;
//const int POT_pin = A2;
const int Strob_pin = 7; //Контакт для подключения стробирующего сигнала (здесь от pin = 9)
double increment = 0.2;

void setup() 
{
  Serial.begin(9600);
  
  //=======================================================================
//Моя стандартная вставка для индикации названия скетча при запуске
byte tries = 20;
delay(5000);
//Serial.println("Вставка для индикации названия скетча при запуске: <My_Scetch.ino>");
  Serial.println("Точный генератор 1гц — 8 МГц на Arduino с управлением от ПК + Sin: <MySQR2.ino>");
  while (--tries)
	{
		delay(500);               // Пауза 500 мс
        Serial.print(".");        // Печать "."
		Serial.print(tries);	  // Печать ожидания пуска программы
	}
  Serial.println();
  
  	
	Serial.println("Введите значение частоты меандра числом в герцах");
	Serial.println("Значение синуса будет меньше в 16 раз");
    Serial.println(" =========================");
	Serial.println();
//=======================================================================
  
  pinMode (9, OUTPUT); // выход генератора
  
  pinMode (5, OUTPUT); // 
  
  //pinMode (10, INPUT); // 

  //Регистр управления А таймера/счетчика1
  TCCR1A=0;
  //Регистр управления B таймера/счетчика1
  TCCR1B=0;
  
  attachInterrupt(4,generateSignal,CHANGE); //от сигнала, поступающего на pin = 7)
  //см. https://alexgyver.ru/lessons/interrupts/?ysclid=mcne1qxpy3966705587
}

//====== ПОДПРОГРАММЫ ==============================================
void generateSignal()
{
   double sineValue = sin(angle);
   sineValue *= 255;
   int plot = map(sineValue, -255, +255, 0, 255);
  // Serial.println(plot);   //Для контроля
   analogWrite(Sine_pin,plot);	// Выход ШИМ
  // Write(Sine_pin,plot);		// Выход "цифра" для ЦАП
   angle += increment; 
  // if (angle > 180)
   if (angle > (2*PI))
   angle =0;
}

// ================================================ ПОДПРОГРАММЫ ===

void loop() 
{

  static uint32_t UartFreq = 0; //Переменная для задания нужной частоты
  uint32_t compare = OCR1A;  //OCR1A - Регистры сравнения A выхода таймера/счетчика1
  uint16_t divider = 1;  //переменная коэфф. деления прескалера

  float freqency;

  if (Serial.available() > 0) 
  { 
    UartFreq = Serial.parseInt(); //получаем значение частоты из сообщения UART

    if (UartFreq <= 0 || UartFreq > F_CPU/2) //Проверяем допустимый диапазон значений частоты
	{ 
      return; 
	}
/*
F_CPU — макроопределение в программировании микроконтроллеров AVR, которое задаёт 
тактовую частоту микроконтроллера в герцах. Например, в файле Makefile может быть 
задано значение: F_CPU = 16000000UL — частота задана 16 МГц.
*/
    compare = (F_CPU / UartFreq /2 /divider); //вычисляем нужное значение OCR


    for (byte i = 0; i < 4; i++) 
	{

      if (compare > 65536) { 
        divider <<= Lshift[i];
        compare = F_CPU / UartFreq /2 /divider; }
        
      else {
        TCCR1B = (i + 1) | ( 1 << WGM12 ); 
        break; }  // CTC режим работы таймера
    } 

    OCR1A = compare-1; 
    TCCR1A = 1 << COM1A0;

// Вывод значения частоты обратно в UART в подтверждение 
//Следующие три строки можно закомментировать, эта информация
//не используется в программе управления на Delphi
    freqency = F_CPU / 2 / (OCR1A + 1) / divider;
	
	Serial.print("Частота меандра (строба) = ");
    if (freqency <10000) 
	{
		Serial.print(freqency, 0); 
		Serial.println(" Hz ");
	}
    if (freqency >= 10000)
		{
			Serial.print(freqency / 1000, 3); 
			Serial.println(" kHz");
		}
	Serial.println(" --------------------------------");
	
	Serial.print("Частота синуса = ");
		 if (freqency <10000) 
	{
		Serial.print(freqency/16, 0); 
		Serial.println(" Hz ");
	}
    if (freqency >= 10000)
		{
			Serial.print(freqency / 16000, 3); 
			Serial.println(" kHz");
		}

 }
}
