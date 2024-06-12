#include <Arduino.h>
#include "wiring_private.h"

void setup() {

  // Analog comparator (AC) configuration

  // Enable clock sources
  REG_PM_APBCMASK |= PM_APBCMASK_AC; // Enable APB Clock for AC (CLK_AC_APB)
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_ID_AC_ANA | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0; // Enable comparator analog clock (GCLK_AC_ANA)
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_ID_AC_DIG | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0; // Enable comparator digital clock (GCLK_AC_DIG)

  //SET PA04 as comparator AIN0
  pinMode(A3, INPUT);
  pinPeripheral(A3, PIO_AC_CLK);

  // Configure comparator 0
  REG_AC_COMPCTRL0 = AC_COMPCTRL_SPEED_HIGH | AC_COMPCTRL_MUXPOS_PIN0 | AC_COMPCTRL_MUXNEG_BANDGAP | AC_COMPCTRL_SWAP | AC_COMPCTRL_INTSEL_RISING | AC_COMPCTRL_HYST | AC_COMPCTRL_FLEN_MAJ5 | AC_COMPCTRL_ENABLE; // Configure and enable comparator 0
  REG_AC_INTENSET = AC_INTENSET_COMP0; // Enable Comparator interrupt for comparator 0
  REG_AC_EVCTRL |= AC_EVCTRL_COMPEO0; // Enable event output for comparator 0
  REG_AC_CTRLA |= AC_CTRLA_ENABLE; // Enable comparators

  NVIC_EnableIRQ (AC_IRQn); // Enable AC interrupts in NVIC

  Serial.begin(115200);
  
}

void loop() {
    
}

unsigned long last = 0;

void AC_Handler (void) {
  unsigned long now = micros();
  Serial.print("RISE: ");
  Serial.println(now-last);
  Serial.print("Freq: ");
  Serial.println(1000000/(now-last));
  last = now;
  REG_AC_INTFLAG |= AC_INTFLAG_COMP0; // Clear interrupt
}
