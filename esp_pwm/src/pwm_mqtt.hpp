#ifndef __PWM_MQTT__
#define __PWM_MQTT__

#include <Arduino.h>
#include <Adafruit_ADS1X15.h>
#include <PubSubClient.h>

constexpr int BUFLEN=64;
constexpr int NCHANS=4;

class PwmMqtt{
protected:
    const char *_bname;
    PubSubClient *_client;
    Adafruit_ADS1115 *_daq;
    uint16_t _avg;


    uint8_t* _pwm_chan;  // PWM pins used for generating the PWM
    uint8_t _pwm_val;  // Default value for PWM
    uint8_t _nb; // Number of characters on the bname
    int32_t _frame[NCHANS];
    uint8_t _pwmio[NCHANS]; // PWM channels
    
    void _callback(char *topic, uint8_t *payload, unsigned int length);

    char _buf0[BUFLEN];
    char _buf1[BUFLEN];
public:
    
    PwmMqtt(const char *bname, PubSubClient *client, Adafruit_ADS1115 *daq, 
        uint8_t *pwm_chan, uint16_t avg=50, uint8_t pwm=255);
    
    void initialize();

    void set_pwm(int ch, int num);

    void set_avg(int avg);
    void scan_ai();
    void set_callback();
    void publish_params();
    void loop();


};

#endif //__PWM_MQTT__