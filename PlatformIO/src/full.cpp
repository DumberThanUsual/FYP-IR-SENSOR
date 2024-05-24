#include <Arduino.h>

void setup() {

  // Event system (EVSYS) configuration

  // Enable clock sources
  REG_PM_APBCMASK |= PM_APBCMASK_EVSYS; // Enable APB clock for EVSYS (CLK_EVSYS_APB)
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_ID_EVSYS_0 | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0; // Enable generic clock for channel 0 (GCLK_EVSYS_CHANNEL_0)

  // Configure Channel 0
  REG_EVSYS_USER = EVSYS_USER_CHANNEL(1U) | EVSYS_ID_USER_TC4_EVU; // Connnect channel 0 to TC4
  REG_EVSYS_CHANNEL = EVSYS_CHANNEL_CHANNEL(0U) | EVSYS_CHANNEL_EDGSEL_BOTH_EDGES | EVSYS_CHANNEL_PATH_SYNCHRONOUS | EVSYS_CHANNEL_EVGEN(0x44U); // Configure channel source

  // Enbale interrupts for debugging
  REG_EVSYS_INTENSET = EVSYS_INTENSET_EVD0;
  //NVIC_EnableIRQ(EVSYS_IRQn);

  // Timer coutner 4 (TC4) configuration

  //Enable clock sources
  REG_PM_APBCMASK |= PM_APBCMASK_TC4; //Enable APB clock for TC4 (CLK_TC4_APB)
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_ID_TC4_TC5 | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0; // Enable generic clock for TC4 and TC5 (GCLK_TC4_TC5)

  // Configure TC4
  REG_TC4_CTRLA |= TC_CTRLA_MODE_COUNT16 | TC_CTRLA_WAVEGEN_NFRQ | TC_CTRLA_PRESCALER_DIV8 | TC_CTRLA_PRESCSYNC_PRESC; // Configure CTRLA register
  //REG_TC4_CTRLBSET |= TC_CTRLBSET_ONESHOT;
  REG_TC4_CTRLC |= TC_CTRLC_CPTEN0 | TC_CTRLC_CPTEN1; // Enable capture channels 1 & 2
  REG_TC4_EVCTRL |= TC_EVCTRL_TCEI | TC_EVCTRL_EVACT_PPW;
  REG_TC4_CTRLA |= TC_CTRLA_ENABLE;

  // Analog comparator (AC) configuration

  // Enable clock sources
  REG_PM_APBCMASK |= PM_APBCMASK_AC; // Enable APB Clock for AC (CLK_AC_APB)
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_ID_AC_ANA | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0; // Enable comparator analog clock (GCLK_AC_ANA)
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_ID_AC_DIG | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0; // Enable comparator digital clock (GCLK_AC_DIG)

  /*
  // Configure PA18 as comparator output
  pinMode(10, OUTPUT);
  // Register addresses for SAM D21
  *(RwReg*)(0x41004458U) |= PORT_PINCFG_PMUXEN; // Enable MUX
  *(RwReg*)(0x41004439U) |= PORT_PMUX_PMUXE_H; // Set MUX to channel H (AC_COUT0)
  */

  //SET PA04 as comparator AIN0
  pinMode(A3, INPUT);
  // Register addresses for SAM D21 as aliases are not provided
  *(RwReg*)(0x41004444U) |= PORT_PINCFG_PMUXEN; // Enable MUX
  *(RwReg*)(0x41004432U) |= PORT_PMUX_PMUXE_B; // Set MUX to channel B (AC_AIN0)

  // Configure comparator 0
  REG_AC_COMPCTRL0 = AC_COMPCTRL_SPEED_HIGH | AC_COMPCTRL_MUXPOS_PIN0 | AC_COMPCTRL_MUXNEG_BANDGAP | AC_COMPCTRL_SWAP | AC_COMPCTRL_INTSEL_RISING | AC_COMPCTRL_HYST | AC_COMPCTRL_FLEN_MAJ5 | AC_COMPCTRL_ENABLE; // Configure and enable comparator 0
  REG_AC_INTENSET = AC_INTENSET_COMP0; // Enable Comparator interrupt for comparator 0
  REG_AC_EVCTRL |= AC_EVCTRL_COMPEO0; // Enable event output for comparator 0
  REG_AC_CTRLA |= AC_CTRLA_ENABLE; // Enable comparators

  NVIC_EnableIRQ (AC_IRQn);

  Serial.begin(115200);
  
}

void loop() {
  //Serial.println(REG_TC4_COUNT16_CC0);
  //Serial.println(REG_TC4_COUNT16_CC1);
  //Serial.println(REG_TC4_INTFLAG, BIN);
  /*
  if (REG_TC4_INTFLAG >> 1 & 1) {
    Serial.println(6000000/REG_TC4_COUNT16_CC0);
    Serial.println();
  }
  delay(10);
  */
 
}

unsigned long last = 0;

void AC_Handler (void) {
  unsigned long now = micros();
  Serial.print("RISE: ");
  Serial.println(now-last);
  Serial.print("Freq: ");
  Serial.println(1000000/(now-last));
  last = now;
  REG_AC_INTFLAG |= AC_INTFLAG_COMP0;
}

void EVSYS_Handler (void) {
  Serial.print("EVENT TC4 value: ");
  Serial.println(REG_TC4_COUNT16_COUNT);
  Serial.println(REG_TC4_COUNT16_CC0);
  REG_EVSYS_INTFLAG |= EVSYS_INTFLAG_EVD0;
}