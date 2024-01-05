/*--== Worm Nudger V 3.1 =-
  (c) 2020 by Bjn Weber, Wagner Steuer Costa, Martin Schneider and Ichiro Aoki
 
  3.1
  Intensity of LED is regulated with analogWrite.
  Connect Arduino directly to the dimming port of the powere supply.
  2.2
  TTLout: 10, TTLout2: 11
  Use with potentiometer when intensity more than 50% is needed.
  2.0
  Program was simplified with a single mode.
  0.52
  Contol both LED ring and tapper.

  00:
  01:
  02: TTLin
  03: 
  04: 
  05: 
  06: 
  07: 
  08: 
  09: 
  10: TTLout
  11: TTLout2
  12: 
  13: 
*/

const int LED_max = 0;     // Number of LED pulses to be applied.
const unsigned long LED_delay = 1200*1000ul;    // Wait time in ms between start and first LED pulse.
const unsigned long LED_dur = 2000ul;       // LED pulse length in ms
const unsigned long LED_int = 0ul; // Time between end and start of two sequential LED pulses.
const unsigned long aft_LED = 0ul;   // Wait time after the last LED pulse
const int tap_max = 3;     // Number of LED pulses to be applied.
const unsigned long tap_delay = 900*1000ul;    // Wait time in ms between start and first LED pulse.
const unsigned long tap_dur = 200ul;       // LED pulse length in ms
const unsigned long tap_int = 800ul; // Time between end and start of two sequential LED pulses.
const unsigned long Exp_dur = LED_delay + (LED_dur + LED_int) * LED_max + aft_LED;
//const int Timeout = 10 * 1000;
int mode = 1;
int LED_started = 0;
int LED_ended = 0;
int tap_started = 0;
int tap_ended = 0;
//const int TTLin = 2;
const int TTLout = 10; //LED
const int Brt = 25; //specify 0-255
const int TTLout2 = 11; //Tapper

void setup()
{
  // pinMode(TTLin, INPUT);
  pinMode(TTLout, OUTPUT);
  pinMode(TTLout2, OUTPUT);
  analogWrite(TTLout, 255);
  digitalWrite(TTLout2, LOW);
  Serial.begin(57600);
}

void loop()
{
  Serial.print(LED_started);
  Serial.println("Send signal to start program.");
  while (mode == 1) {
    if (Serial.read() > 0) {
      mode = 2;
      Serial.print(LED_started);
      Serial.println("Running. Send signal to abort.");
      unsigned long start_millis = millis();
      unsigned long maxTime = Exp_dur + start_millis;

      while (mode == 2 && millis() <= maxTime) {
        // Check start of LED pulse (time since previous pulse ended)
        long deltaTime = millis() - (start_millis + LED_delay + LED_ended * (LED_dur + LED_int));
        if ((deltaTime >= 0) && (LED_started == LED_ended) && (LED_ended < LED_max)) {
          LED_started++;
          analogWrite(TTLout, Brt);
        }
        // Check end of LED pulse
        long deltaTime2 = deltaTime - LED_dur;
        if ((deltaTime2 >= 0) && (LED_started > LED_ended)) {
          LED_ended++;
          analogWrite(TTLout, 0);
        }
        // Check start of tap pulse (time since previous pulse ended)
        long deltaTimeTap = millis() - (start_millis + tap_delay + tap_ended * (tap_dur + tap_int));
        if ((deltaTimeTap >= 0) && (tap_started == tap_ended) && (tap_ended < tap_max)) {
          tap_started++;
          digitalWrite(TTLout2, HIGH);
        }
        // Check end of tap pulse
        long deltaTimeTap2 = deltaTimeTap - tap_dur;
        if ((deltaTimeTap2 >= 0) && (tap_started > tap_ended)) {
          tap_ended++;
          digitalWrite(TTLout2, LOW);
        }
        if (Serial.read() > 0) {
          mode = 0;
          Serial.print(LED_started);
          Serial.println("Aborted. Send signal when ready!");
        }
      }
    }
    analogWrite(TTLout, 0);
    digitalWrite(TTLout2, LOW);
    LED_ended = 0;
    LED_started = 0;
    tap_started = 0;
    tap_ended = 0;
  }
mode = 1;
}
