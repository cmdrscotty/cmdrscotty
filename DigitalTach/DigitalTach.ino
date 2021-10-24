/*
 Name:		DigitalTach.ino
 Created:	10/3/2021 7:18:13 AM
 Author:	Cmdr_Scotty
*/

#include <LedControl.h>

#define dev 0
#define delaytime 25
#define r_num 2
#define input_pin 3
#define shift_pin 4
#define shift_point 6150

LedControl lc = LedControl(0, 2, 1, 1);

unsigned int rpm, r_readings[r_num], rpm_old;
unsigned long previousMillis = 0, currentMillis = 0;
float rpm_a = 0;
int maxRPM = 8000, num_led = 30, rpm_diff;


byte seg0[8] =
{
	B00000001, //0
	B00000011, //1
	B00000111, //2
	B00001111, //3
	B00011111, //4
	B00111111, //5
	B01111111, //6
	B11111111  //7
};


void setup() {
	lc.shutdown(0, false);
	lc.clearDisplay(0);
	lc.setIntensity(0, 8);
	

	//Serial.begin(9600);
	pinMode(input_pin, INPUT_PULLUP);
	pinMode(shift_pin, OUTPUT);
	digitalWrite(shift_pin, LOW);
	rpm = 0;
	
	//for (int i = 0; i < 4; i++) {
		//lc.setDigit(0, i, 8, true);
		//delay(50);
	//}

	int Rones = (shift_point % 10);
	int Rtens = (shift_point / 10) % 10;
	int Rhuns = (shift_point / 100) % 10;
	int Rthous = (shift_point / 1000) % 10;

	lc.setDigit(0, 3, Rones, false);
	lc.setDigit(0, 2, Rtens, false);
	lc.setDigit(0, 1, Rhuns, false);
	lc.setDigit(0, 0, Rthous, false);

	for (int i = 0; i < 8; i++) {
		lc.setRow(0, 4, seg0[i]);
		delay(delaytime);
	}
	for (int i = 0; i < 8; i++) {
		lc.setRow(0, 5, seg0[i]);
		delay(delaytime);
	}
	for (int i = 0; i < 8; i++) {
		lc.setRow(0, 6, seg0[i]);
		delay(delaytime);
	}
	for (int i = 0; i < 8; i++) {
		lc.setRow(0, 7, seg0[i]);
		delay(delaytime);
	}
	digitalWrite(4, HIGH);
	delay(200);
	digitalWrite(4, LOW);

  for (int i = 7; i >= 0; i--) {
    lc.setRow(0, 7, seg0[i]);
    delay(delaytime);
  }
  lc.setRow(0,7,0);
  delay(delaytime);
  
  for (int i = 7; i >= 0; i--) {
    lc.setRow(0, 6, seg0[i]);
    delay(delaytime);
  }
  lc.setRow(0,6,0);
  delay(delaytime);
  
  for (int i = 7; i >= 0; i--) {
    lc.setRow(0, 5, seg0[i]);
    delay(delaytime);
  }
  lc.setRow(0,5,0);
  delay(delaytime);
  
  for (int i = 7; i >= 0; i--) {
    lc.setRow(0, 4, seg0[i]);
    delay(delaytime);
  }
  lc.setRow(0,4,0);
  delay(delaytime);

  
	lc.clearDisplay(0);
	lc.setDigit(0, 3, 0, false);

#if (!dev)
	rpm_a = 0;
#endif
#if (dev)
	rpm_a = 100;
#endif
}


void loop() {
	currentMillis = millis();
	static long r_periodon = 0, r_periodoff = 0;
	static int r_index = 0;
	static unsigned long r_total = 0, r_avg = 0;
	static float r_pulse = 0;
//  static int rpm_diff = 0;
	



#if (!dev)
	r_periodoff = pulseIn(input_pin, LOW);
	r_periodon = pulseIn(input_pin, HIGH);
	r_pulse = r_periodon + r_periodoff;
	
	//converting to HZ from uS is f = 1000000 / T
	//converting HZ to RPM is rpm = HZ*60 (IE 60hz is 3600 rpm)
	//Spark fires total cylinders / 2 for every RPM (2 for 4 cylinder, 3 for 6, 4 for 8, etc...)
	//therefore the formula should be: rpm = ((1000000/r_pulse)/2)*60
	rpm_old = rpm;
	rpm = ((1000000 / r_pulse) / 2) * 60;
	rpm_diff = rpm - rpm_old;
	
		if (rpm_diff < 650)
		  {
      r_total = r_total - r_readings[r_index];
       r_readings[r_index] = rpm;
       r_total = r_total + r_readings[r_index];
        			
			}

 
       r_index++;		
       rpm_a = r_total / r_num;
       
	if (r_index >= r_num) {
		r_index = 0;
	}
#endif // dev

#if (dev)
	if (currentMillis - previousMillis >= 50) {
		previousMillis = currentMillis;
		static boolean rpmswitch = false;

		if (rpmswitch) {
			rpm_a = rpm_a - 50;
		}
		if (!rpmswitch) {
			rpm_a = rpm_a + 50;
		}

		if (rpm_a >= 9900) {
			rpmswitch = !rpmswitch;
		}
		if (rpm_a <= 100) {
			rpmswitch = !rpmswitch;
		}

	}
	

	//delay(50);

#endif



	
	
	rpm_Digits();
	rpm_LEDS();
	rpm_Shift();
}

void rpm_Digits() {
	//lc.setIntensity(0, 15);
	int Rones = ((int)rpm_a % 10);
	int Rtens = ((int)rpm_a / 10) % 10;
	int Rhuns = ((int)rpm_a / 100) % 10;
	int Rthous = ((int)rpm_a / 1000) % 10;

	lc.setDigit(0, 3, Rones, false);

	if (Rtens == 0 && Rhuns == 0 && Rthous == 0) { lc.setRow(0, 2, 0); }
	else lc.setDigit(0, 2, Rtens, false);

	if (Rhuns == 0 && Rthous == 0) { lc.setRow(0, 1, 0); }
	else lc.setDigit(0, 1, Rhuns, false);

	if (Rthous == 0) { lc.setRow(0, 0, 0); }
	else lc.setDigit(0, 0, Rthous, false);

}

void rpm_LEDS() {
	//lc.setIntensity(0, 5);
	int led_val = (rpm_a / ((maxRPM - 500) / num_led)) - 2;

	if (rpm_a < 499) {
		lc.setRow(0, 4, 0);
		lc.setRow(0, 5, 0);
		lc.setRow(0, 6, 0);
		lc.setRow(0, 7, 0);
	}

	//led_val 0-8
	if (rpm_a >=500 && led_val <= 7) {
		lc.setRow(0, 4, seg0[led_val]);
		lc.setRow(0, 5, 0);
		lc.setRow(0, 6, 0);
		lc.setRow(0, 7, 0);
	}
	
	//led_val 9-16
	if (led_val >= 8 && led_val <= 15) {
		lc.setRow(0, 4, seg0[7]);
		lc.setRow(0, 5, seg0[led_val-8]);
		lc.setRow(0, 6, 0);
		lc.setRow(0, 7, 0);
	}

	//led_val 17-24
	if (led_val >= 16 && led_val <= 23) {
		lc.setRow(0, 4, seg0[7]);
		lc.setRow(0, 5, seg0[7]);
		lc.setRow(0, 6, seg0[led_val-16]);
		lc.setRow(0, 7, 0);
	}

	//led_val 25-30
	if (led_val >= 24 && led_val <= 30) {
		lc.setRow(0, 4, seg0[7]);
		lc.setRow(0, 5, seg0[7]);
		lc.setRow(0, 6, seg0[7]);
		lc.setRow(0, 7, seg0[led_val - 24]);
	}

	if (led_val >30) {
		lc.setRow(0, 4, seg0[7]);
		lc.setRow(0, 5, seg0[7]);
		lc.setRow(0, 6, seg0[7]);
		lc.setRow(0, 7, seg0[6]);
	}

}

void rpm_Shift() {
	static unsigned long previousMillis = millis();
	static unsigned long previousMillis2 = millis();
	static int flashCount = 0;
	static boolean pinstate = true;
	if (rpm_a >= shift_point) 
	{
		if (millis() - previousMillis > 100) {
			previousMillis = millis();
			flashCount++;

			if (flashCount >= 10) {
				if (millis() - previousMillis2 > 50) {
					previousMillis2 = millis();

					if (pinstate) {
						pinstate = false;
					}
					else
					{
						pinstate = true;
					}
				}
			}
			
		}
		digitalWrite(shift_pin, pinstate);

	}
	else
	{
		digitalWrite(shift_pin, LOW);
		pinstate = true;
		flashCount = 0;
		previousMillis = millis();
		previousMillis2 = millis();
	}

}
