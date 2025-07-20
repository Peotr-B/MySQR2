# Scriptor42
 
 20июл25
 @Scriptor42
 Scriptor42@list.ru

Проект "16_Сборка простого генератора сигналов"

Дисклеймер
Данный проект является частью статьи для компании "Суперайс"

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