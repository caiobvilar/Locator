// main.c
#include "app_main.h"
#include "Display.h" // instead of f411E_lcd.h
#include "main.h"

void
App_Main(void)
{
    // Hardware init done before this
    Display_Init();

    Display_FillScreen(0xF800);

    while (1)
    {
    }
}
