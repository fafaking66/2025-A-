#include "UI.h"

#include "lcd.h"
#include "lcd_init.h"

//初始化菜单显示
void Show_UI(void)
{
    LCD_Fill(0, 0, LCD_W, LCD_H, WHITE);
    LCD_ShowString(20, 15, (u8 *)"Crystal_Freq Measurement Device", BLACK, WHITE, 16, 1);
    LCD_DrawLine(0, 45, LCD_W, 45, BLACK);
    LCD_ShowString(60, 70, (u8 *)"Freq1:", RED, WHITE, 16, 1);
    // LCD_ShowString(60, 90, (u8 *)"Delta:", RED, WHITE, 16, 1);
    // LCD_ShowString(60, 110, (u8 *)"LoadValue:", RED, WHITE, 16, 1);
    // LCD_ShowString(60, 130, (u8 *)"TrigCnt:", RED, WHITE, 16, 1);
    LCD_ShowString(60, 90, (u8 *)"Freq2:", RED, WHITE, 16, 1);
    LCD_ShowString(60, 110, (u8 *)"Freq:", RED, WHITE, 16, 1);
}
