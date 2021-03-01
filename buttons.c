/*

	Program: 	wiringPi Buttons (buttons.c)
			 
    Author:  M. Heidenreich, (c) 2021

    Description:
    This code is provided in support of the following YouTube tutorial:
   	https://www.youtube.com/watch?v=9Znf0CnsbXI 
    
	This 2-part Raspberry Pi GPIO tutorial shows how to correctly use buttons with wiringPi.
	While I use the buttons to control LEDs, you can use the same approach with any other
	electronic components (or code in a subroutine/function in general).

    THIS SOFTWARE AND LINKED VIDEO TOTORIAL ARE PROVIDED "AS IS" AND THE
    AUTHOR DISCLAIMS ALL WARRANTIES INCLUDING ALL IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <wiringPi.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

// GPIO Pins, BCM numbering
#define RED_BUTTON 16
#define GREEN_BUTTON 20
#define BLUE_BUTTON 21
#define RED_LED 13
#define GREEN_LED 19
#define BLUE_LED 26

void cleanup(int signo) {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(BLUE_LED, LOW);
    pinMode(RED_LED, INPUT);
    pinMode(GREEN_LED, INPUT);
    pinMode(BLUE_LED, INPUT);
    pullUpDnControl(RED_BUTTON, PUD_DOWN);
    pullUpDnControl(GREEN_BUTTON, PUD_DOWN);
    pullUpDnControl(BLUE_BUTTON, PUD_DOWN);

    exit(0);
}

void goRed() {
    digitalWrite(RED_LED, !digitalRead(RED_BUTTON));
    delay(50);
}

unsigned short int isPressed(unsigned short int button) {
    static struct timespec lastCall;
    struct timespec thisCall;
    float timeDiff;

    clock_gettime(CLOCK_REALTIME, &thisCall);
    timeDiff = (thisCall.tv_sec + thisCall.tv_nsec/1E9 - lastCall.tv_sec - lastCall.tv_nsec/1E9)*5;
    lastCall = thisCall;

    return timeDiff > 1 ? 1 : 0;
}

void goGreen() {
    if(isPressed(GREEN_BUTTON)) {
        digitalWrite(GREEN_LED, !digitalRead(GREEN_LED));
    }
}

void blink(unsigned short int led, unsigned short int interval) {
    for(;;) {
        digitalWrite(led, HIGH);
        delay(interval);
        digitalWrite(led, LOW);
        delay(interval);
    }
}

void *callback(void *args) {
    blink(BLUE_LED, 250);
    return 0;
}

unsigned short int isHeld(unsigned short int button, unsigned short int holdTime) {
    unsigned short int sample;
    unsigned short int sampleCount = holdTime/25;
    unsigned short int delayInterval = holdTime/40;

    for(sample=0; sample<sampleCount; sample++) {
        if (digitalRead(button)) {
            break;
        }

        delay(delayInterval);
    }

    return sample == sampleCount ? 1 : 0;
}

void goBlue() {
    static pthread_t threadID;

    if(isHeld(BLUE_BUTTON, 1000)) {
        if(threadID) {
            pthread_cancel(threadID);
            threadID = 0;
            digitalWrite(BLUE_LED, LOW);
        }
        else {
            pthread_create(&threadID, NULL, callback, NULL);
        }
    }
}

int main(void) {
    signal(SIGINT, cleanup);
    signal(SIGTERM, cleanup);
    signal(SIGHUP, cleanup);

    wiringPiSetupGpio();

    pinMode(RED_BUTTON, INPUT);
    pinMode(GREEN_BUTTON, INPUT);
    pinMode(BLUE_BUTTON, INPUT);
    pullUpDnControl(RED_BUTTON, PUD_UP);
    pullUpDnControl(GREEN_BUTTON, PUD_UP);
    pullUpDnControl(BLUE_BUTTON, PUD_UP);

    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(BLUE_LED, LOW);

    wiringPiISR(RED_BUTTON, INT_EDGE_BOTH, goRed);
    wiringPiISR(GREEN_BUTTON, INT_EDGE_FALLING, goGreen);
    wiringPiISR(BLUE_BUTTON, INT_EDGE_FALLING, goBlue);
    
    pause();
    return 0;
}
