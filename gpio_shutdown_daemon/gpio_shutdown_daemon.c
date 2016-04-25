 //======================
 //  Raspberry Pi GPIO Reboot & Shutdown daemon
 //  http://www.neko.ne.jp/~freewing/
 //  Copyright (c)2016 FREE WING, Y.Sakamoto
 //
 //  gcc -o gpio_shutdown_daemon gpio_shutdown_daemon.c -I/usr/local/include -L/usr/local/lib -lwiringPi
 //======================
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <wiringPi.h>

 // LED 0="ON" 1="OFF"
#define GPIO_LED 22
 // BUTTON 0="PUSH" 1="OPEN"
#define GPIO_BTN 27


void shutdown(void)
{
    system("shutdown -h now");
}

void reboot(void)
{
    system("reboot");
}

int check_button(int pat[])
{
    int i;

    for (i=0; i<10; ++i)
    {
        digitalWrite(GPIO_LED, pat[i]);
        delay(100);

        if (digitalRead(GPIO_BTN) != 0)
        {
            digitalWrite(GPIO_LED, 1);
            return 0;
        }
    }

    return 1;
}

void interrupt(void)
{
    // チャタリング避け（これが無いと割り込み処理が誤動作した）
    delay(50); // 50ms
    if (digitalRead(GPIO_BTN) != 0)
    {
        return;
    }

    // 0～2秒は再起動
    int pat1[10] = { 0,1,0,1,1,1,1,1,1,1 };
    if (!check_button(pat1)) { reboot(); return; }
    if (!check_button(pat1)) { reboot(); return; }

    // 2～4秒はシャットダウン
    int pat2[10] = { 0,0,1,1,1,0,0,1,1,1 };
    if (!check_button(pat2)) { shutdown(); return; }
    if (!check_button(pat2)) { shutdown(); return; }

    // 4秒以降は何もしない
    int pat3[10] = { 0,1,1,1,1,1,1,1,1,1 };
    while(1)
    {
        if (!check_button(pat3)) break;
    }

    delay(100);

    return;
}

int main_loop(void)
{
    if (wiringPiSetupGpio() < 0)
    {
        fprintf(stderr, "ERROR: WiringPi initialize (%s)\n", strerror(errno));
        return 1;
    }

    pinMode(GPIO_BTN, INPUT);
    pullUpDnControl(GPIO_BTN, PUD_UP);

    pinMode(GPIO_LED, OUTPUT);
    digitalWrite(GPIO_LED, 1);

    // interrupt H to L edge
    if (wiringPiISR(GPIO_BTN, INT_EDGE_FALLING, &interrupt) < 0)
    {
        fprintf(stderr, "ERROR: WiringPi set interrupt (%s)\n", strerror(errno));
        return 1;
    }

    while(1)
    {
        delay(1000); // 1000ms
    }

    return 0;
}


int main(int argc, char *argv[])
{
    // 通常モード
    return main_loop();

 /*
    // 通常モード
    if (argc < 2)
    {
        return main_loop();
    }

    // 何か引数が有れば daemonモード
    if (daemon(0, 0) == 0)
    {
        return main_loop();
    }
    else
    {
        fprintf(stderr, "ERROR: daemon\n");
    }

    return 1;
 */
}
