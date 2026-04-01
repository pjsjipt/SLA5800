#include "pwm_mqtt.hpp"

void serial_print_topic(const char *topic, const char *buf){
  Serial.print("Topic: ");
  Serial.print(topic);
  Serial.print(" -> ");
  Serial.println(buf);
}

void serial_print_topic(const char *topic, const char *buf, int len){
  Serial.print("Topic: ");
  Serial.print(topic);
  Serial.print(" -> ");
  for (int i = 0; i < len; ++i){
    Serial.print(buf[i]);
  }
  Serial.println();
}


PwmMqtt::PwmMqtt(const char *bname, PubSubClient *client, Adafruit_ADS1115 *daq, 
    uint8_t *pwm_chan, uint16_t avg, uint8_t pwm):
    _bname(bname), _client(client),_daq(daq), _pwm_chan(pwm_chan),
     _avg(avg), _pwm_val(pwm), _meas(false)
{
    _nb = strlen(bname);


}

void PwmMqtt::initialize(){
    for (uint8_t i = 0; i < NCHANS; ++i){
        _frame[i] = 0;
    }
    for (uint8_t i = 0; i < NCHANS; ++i){
        set_pwm(i, _pwm_val);
    }
}
void PwmMqtt::set_avg(int avg){
    if (avg < 1){
        _avg = 1;
    }else if (avg>2000){
        _avg = 2000;
    }else{
        _avg = (uint16_t) avg;
    }
}
void PwmMqtt::set_pwm(int ch, int num){
    uint16_t pwm;
    if (ch >= 0 && ch < NCHANS){
        if (num >= 0 && num < 256){
            if (num != _pwmio[ch]){
                analogWrite(_pwm_chan[ch], num);
                _pwmio[ch] = (uint8_t) num;
            }
        }

    }
}

void PwmMqtt::set_meas(int num){
    if (num != 0){
        _meas = true;
    } else {
        _meas = false;
    }
}

void PwmMqtt::scan_ai(){
    for (uint8_t i; i < NCHANS; ++i)
        _frame[i] = 0;
    
    for (uint16_t k = 0; k < _avg; ++k){
        for (uint8_t i = 0; i < 4; ++i){
            _frame[i] += _daq->readADC_SingleEnded(i);
        }
    }
    for (uint8_t i; i < NCHANS; ++i)
        _frame[i] = _frame[i] / _avg;

}
void PwmMqtt::_callback(char *topic, uint8_t *payload, unsigned int length){
    char c = topic[_nb];
    Serial.println("CALLBACK");
    int32_t num;
    if (length < BUFLEN){  // We don't want any trouble... 
        for (unsigned int i = 0; i < length; ++i){
            _buf0[i] = payload[i];
        }
        _buf0[length] = 0;
         num = atoi(_buf0);
        if (c == 'A'){ // AVG
            set_avg(num);
            Serial.println(_avg);
        }else if (c == 'P'){   // Specifying the value of PWM
            uint8_t ch = topic[_nb + 3] - '0';
            set_pwm(ch, num);
            Serial.print("PWM"); Serial.print(ch);
            Serial.print(" -> ");
            Serial.println(_pwmio[ch]);
        }else if (c == 'M'){
            set_meas(num);
            Serial.print("MEAS"); Serial.print(" - > ");
            Serial.println(_meas);

        }
    }
}


void PwmMqtt::set_callback(){
     _client->setCallback([this](char *topic, uint8_t *payload, unsigned int length) { 
    _callback(topic, payload, length);
  });
 
}

void PwmMqtt::publish_params(){
    
    _client->loop();
    // AVG
    snprintf(_buf0, BUFLEN-2, "%sAVG", _bname);
    snprintf(_buf1, BUFLEN-2, "%d", _avg);
    _client->publish(_buf0, _buf1, true);
    serial_print_topic(_buf0, _buf1);
    _client->subscribe(_buf0);
    _client->loop();
    // MEAS
    snprintf(_buf0, BUFLEN-2, "%sMEAS", _bname);
    snprintf(_buf1, BUFLEN-2, "%d", _meas);
    _client->publish(_buf0, _buf1, true);
    serial_print_topic(_buf0, _buf1);
    _client->subscribe(_buf0);
    _client->loop();

    // PWM
    for (uint8_t i=0; i < NCHANS; ++i){
        snprintf(_buf0, BUFLEN-2, "%sPWM%d", _bname, i);
        snprintf(_buf1, BUFLEN-2, "%d", _pwmio[i]);
        _client->publish(_buf0, _buf1, true);
        serial_print_topic(_buf0, _buf1);
        _client->subscribe(_buf0);
        _client->loop();
    }

}

void PwmMqtt::loop(){
    float x;
    _client->loop();

    if (_meas){
        // Read analog input:
        scan_ai();
        // Publish the data:
        for (uint8_t i = 0; i < NCHANS; ++i){
            x = _daq->computeVolts(_frame[i]);
            snprintf(_buf0, BUFLEN-2, "%sAI%d", _bname, i);
            snprintf(_buf1, BUFLEN-2, "%g", x);
            _client->publish(_buf0, _buf1);
            serial_print_topic(_buf0, _buf1);
        }
    }
        
}