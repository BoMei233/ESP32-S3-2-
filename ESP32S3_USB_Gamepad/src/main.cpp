/*
 * GPIO 0, 2, 4, 5, 12, 15, 46使用时要注意上电时引脚电平
 * GPIO 19, 20为USB引脚
 */

/*   雨刷开关引脚定义
 *   LO  NC INT GND  HI
 *   |   |   |   |   |
 *
 *   |   |       |   |
 *   NC  NC    玻璃水GND
 */

/*
 *       ┌────────────────────────┐
         │3V3                  GND│
         │3V3             X  U0TXD│
         │RST             X  U0RXD│
         │GPIO4  X           GPIO1│13
         │GPIO5  X        X  GPIO2│
       14│GPIO6             GPIO42│12
       15│GPIO7             GPIO41│11
         │GPIO15 X          GPIO40│10
       16│GPIO16            GPIO39│9
       17│GPIO17            GPIO38│8
       18│GPIO18            GPIO37│7
       19│GPIO8             GPIO36│6
      *20│GPIO3             GPIO35│5
         │GPIO46 X        X  GPIO0│
      *21│GPIO9             GPIO45│4
      *22│GPIO10            GPIO48│3
      *  │GPIO11            GPIO47│2
         │GPIO12 X          GPIO21│1
      *23│GPIO13          X USB D+│
      *23│GPIO14          X USB D-│
         │5Vin                 GND│
         │GND                  GND│
         │                        │
         │  ┌─────┐      ┌─────┐  │
         │  │     │      │     │  │
         └──┴─────┴──────┴─────┴──┘

 *  按钮引脚定义
 *  GPIO14 ->> 23 //示廓灯
 *  GPIO13 ->> 23 //大灯
 *  GPIO11 ->>    //雨刮器HI
 *  GPIO10 ->>    //雨刮器LO
 *  GPIO9  ->>    //雨刮器INT
 *         ->> 22 //雨刮器降档
 *         ->> 21 //雨刮器升档
 *  GPIO3  ->> 20 //超车灯
 *  注释中的按钮全部为实际按钮，是键值+1
 */

#include <Arduino.h>
#include "USB.h"
#include "USBHIDGamepad.h"

USBHIDGamepad Gamepad;

#define POSITION_LAMP_PIN 14  // GPIO14 ->> btn23 示廓灯
#define HEADLIGHT_PIN 13      // GPIO13 ->> btn23 大灯
#define WIPER_HI_PIN 11       // GPIO11 ->>       雨刮器HI
#define WIPER_LO_PIN 10       // GPIO10 ->>       雨刮器LO
#define WIPER_INT_PIN 9       // GPIO9  ->>       雨刮器INT
#define OVERTAKING_LAMP_PIN 3 // GPIO3  ->> btn20 超车灯

enum wiperState
{
  WIPER_OFF,
  WIPER_INT,
  WIPER_LO,
  WIPER_HI,
};

enum lightState
{
  LIGHT_OFF,
  LIHGT_POSITION,
  LIGHT_HEAD,
};

enum overtakingLampState
{
  OVERTAKING_LAMP_OFF,
  OVERTAKING_LAMP_ON,
};

wiperState currentWiperState = WIPER_OFF;
lightState currentLightState = LIGHT_OFF;
bool currentOvertakingLampState = OVERTAKING_LAMP_OFF;


const uint8_t INPUT_PINS[] = {21, 47, 48, 45, 35, 36, 37, 38, 39, 40, 41, 42, 1, 6, 7, 16, 17, 18, 8, 3, 9, 10, 11, 13, 14};
const uint8_t NUMBER_OF_INPUT_PINS = sizeof(INPUT_PINS) / sizeof(INPUT_PINS[0]);
const uint8_t NUMBER_OF_GENERAL_BUTTONS = NUMBER_OF_INPUT_PINS - 6;
bool lastButtonState[NUMBER_OF_GENERAL_BUTTONS] = {HIGH};

uint8_t convertToActualButton(uint8_t ActualButton)
{
  return ActualButton++; // 实际按钮 = 键值 +1
}

wiperState getWiperState(void)
{
  if (digitalRead(WIPER_INT_PIN) == HIGH && digitalRead(WIPER_LO_PIN) == HIGH && digitalRead(WIPER_HI_PIN) == HIGH)
  // 雨刮器为OFF挡位
  {
    return WIPER_OFF;
  }
  else if (digitalRead(WIPER_INT_PIN) == LOW && digitalRead(WIPER_LO_PIN) == HIGH && digitalRead(WIPER_HI_PIN) == HIGH)
  // 雨刮器为INT挡位
  {
    return WIPER_INT;
  }
  else if (digitalRead(WIPER_INT_PIN) == HIGH && digitalRead(WIPER_LO_PIN) == LOW && digitalRead(WIPER_HI_PIN) == HIGH)
  // 雨刮器为LO挡位
  {
    return WIPER_LO;
  }
  else if (digitalRead(WIPER_INT_PIN) == HIGH && digitalRead(WIPER_LO_PIN) == HIGH && digitalRead(WIPER_HI_PIN) == LOW)
  // 雨刮器为HI挡位
  {
    return WIPER_HI;
  }
  else
  {
    return WIPER_OFF;
  }
}

void wiperGearUp(void)
{
  Gamepad.pressButton(convertToActualButton(21)); // btn21 雨刮器升档
  vTaskDelay(pdMS_TO_TICKS(20));
  Gamepad.releaseButton(convertToActualButton(21));
}

void wiperGearDown(void)
{
  Gamepad.pressButton(convertToActualButton(22)); // btn22 雨刮器降档
  vTaskDelay(pdMS_TO_TICKS(20));
  Gamepad.releaseButton(convertToActualButton(22));
}

void syncWiperState(wiperState targetState)
{
  while (targetState > currentWiperState) // 目标档位大于当前档位，应升档
  {
    wiperGearUp();
    currentWiperState = static_cast<wiperState>((currentWiperState + 1) % 4); // 当前档位+1，取余4，防止溢出
    Serial.println("wiperGearUp");
    Serial.printf("currentWiperState: %d, targetState: %d\n", currentWiperState, targetState);
  }
  while (targetState < currentWiperState) // 目标档位小于当前档位，应降档
  {
    wiperGearDown();
    currentWiperState = static_cast<wiperState>((currentWiperState - 1) % 4); // 当前档位-1，取余4，防止溢出
    Serial.println("wiperGearDown");
    Serial.printf("currentWiperState: %d, targetState: %d\n", currentWiperState, targetState);
  }
}

lightState getLightState(void)
{
  if (digitalRead(POSITION_LAMP_PIN) == HIGH && digitalRead(HEADLIGHT_PIN) == HIGH)
  // 大灯和示廓灯都关闭
  {
    return LIGHT_OFF;
  }
  else if (digitalRead(POSITION_LAMP_PIN) == LOW && digitalRead(HEADLIGHT_PIN) == HIGH)
  // 示廓灯开启
  {
    return LIHGT_POSITION;
  }
  else if (digitalRead(POSITION_LAMP_PIN) == LOW && digitalRead(HEADLIGHT_PIN) == LOW)
  // 示廓灯和大灯均开启
  {
    return LIGHT_HEAD;
  }
  else
  {
    return LIGHT_OFF;
  }
}

void lightGearChange(void)
{
  Gamepad.pressButton(convertToActualButton(23)); // btn23 示廓灯
  vTaskDelay(pdMS_TO_TICKS(20));
  Gamepad.releaseButton(convertToActualButton(23));
}

void syncLightState(lightState targetState) // 同步大灯档位
{
  while (targetState != currentLightState)
  {
    lightGearChange();
    currentLightState = static_cast<lightState>((currentLightState + 1) % 3); // 当前档位+1，取余3，防止溢出, 只有一个按钮，循环换档
  }
}

overtakingLampState getOvertakingLampState(void) // 获取超车灯状态
{
  if (digitalRead(OVERTAKING_LAMP_PIN) == LOW) // 如果超车灯被按下
  {
    return OVERTAKING_LAMP_ON;
  }
  else
  {
    return OVERTAKING_LAMP_OFF;
  }
}

void syncOvertakingLampState(overtakingLampState targetState) // 同步超车灯状态
{
  if (targetState != currentOvertakingLampState) // 这行代码用于防止反复同步超车灯状态，只有开关状态改变才进行同步
  {
    if (targetState == OVERTAKING_LAMP_ON)
    {
      Gamepad.pressButton(convertToActualButton(20)); // btn20 超车灯
      Serial.println("overtaking lamp ON");
    }
    else
    {
      Gamepad.releaseButton(convertToActualButton(20));
      Serial.println("overtaking lamp off");
    }
    currentOvertakingLampState = targetState;
  }
}

void overtakingLamp(void *pvParameters) // 超车灯控制进程
{
  while (1)
  {
    overtakingLampState targetState = getOvertakingLampState();
    if (targetState != currentOvertakingLampState)
    {
      syncOvertakingLampState(targetState); 
    }
    
    // if (digitalRead(OVERTAKING_LAMP_PIN) == LOW)
    // {
    //   syncOvertakingLampState(OVERTAKING_LAMP_ON);
    // }
    // else
    // {
    //   syncOvertakingLampState(OVERTAKING_LAMP_OFF);
    // }
    vTaskDelay(1); // 防止进程卡死
  }
}

void generalButton(void *pvParameters) // 通用按钮控制进程
{
  while (1)
  {
    for (int i = 0; i < NUMBER_OF_GENERAL_BUTTONS; i++)
    {
      bool buttonState = digitalRead(INPUT_PINS[i]); // 读取按钮状态
      if (buttonState != lastButtonState[i])         // 如果当前按钮状态与上一次不一致
      {
        if (buttonState == LOW) // 若按钮被按下
        {
          Gamepad.pressButton(i);
          vTaskDelay(pdMS_TO_TICKS(20));
          Gamepad.releaseButton(i); // 触发一次控制器按钮
          Serial.println("Button " + String(i) + " pressed");
        }
        else // 若按钮被抬起
        {
          Gamepad.pressButton(i);
          vTaskDelay(pdMS_TO_TICKS(20));
          Gamepad.releaseButton(i); // 触发一次控制器按钮
          Serial.println("Button " + String(i) + " released");
        }
      }
      lastButtonState[i] = buttonState; // 更新上一次按钮状态
    }
    vTaskDelay(1); // 防止进程卡死
  }
}

void wiper(void *pvParameters) // 控制雨刮器进程
{
  while (1)
  {
    wiperState targetState = getWiperState();
    if (targetState != currentWiperState)
    {
      syncWiperState(targetState);
    }
    vTaskDelay(1); // 防止进程卡死
  }
}

void light(void *pvParameters) // 大灯控制进程
{
  while (1)
  {
    lightState targetLightState = getLightState();
    if (targetLightState != currentLightState)
    {
      syncLightState(targetLightState);
    }
    vTaskDelay(1); // 防止进程卡死
  }
}

void setup()
{
  Gamepad.begin();
  USB.begin();
  Serial.begin(115200);
  for (int i = 0; i < NUMBER_OF_INPUT_PINS; i++)
  {
    pinMode(INPUT_PINS[i], INPUT_PULLUP);
  }
  vTaskDelay(pdMS_TO_TICKS(2000));
  xTaskCreate(generalButton, "double trigger", 1024 * 10, NULL, 1, NULL);
  xTaskCreate(wiper, "control wiper", 1024 * 10, NULL, 1, NULL);
  xTaskCreate(light, "control light", 1024 * 10, NULL, 1, NULL);
  xTaskCreate(overtakingLamp, "control overtaking lamp", 1024 * 12, NULL, 1, NULL);
}

void loop()
{
  vTaskDelete(NULL);
}
