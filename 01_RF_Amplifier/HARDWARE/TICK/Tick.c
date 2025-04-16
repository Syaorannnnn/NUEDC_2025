#include "Tick.h"

volatile uint32_t Tick = 0;

// SysTick中断(1ms)
void SysTick_Handler(void) {
    Tick++;

    // 按键消抖倒计时
    if (!BTN_LEFT_PRESS && BTNTick.left) BTNTick.left--;
    if (!BTN_DOWN_PRESS && BTNTick.down) BTNTick.down--;
    if (!BTN_RIGHT_PRESS && BTNTick.right) BTNTick.right--;
    if (!BTN_UP_PRESS && BTNTick.up) BTNTick.up--;
    if (!BTN_MID_PRESS && BTNTick.mid) BTNTick.mid--;
}
