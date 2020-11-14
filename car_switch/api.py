# _*_ coding: utf-8 _*_

import RPi.GPIO as GPIO
import sys
from django.http import HttpResponse
import time


class CarControl:
    IN1 = 23
    IN2 = 24
    IN3 = 27
    IN4 = 17
    IN5 = 5
    IN6 = 6
    IN7 = 13
    IN8 = 19

    HIGH = 1
    LOW = 0

    def __init__(self):
        GPIO.setwarnings(False)
        GPIO.setmode(GPIO.BCM)

    def setPinStatus(self, pin, status):
        GPIO.cleanup(int(pin))
        GPIO.setup(int(pin), GPIO.OUT)
        GPIO.output(int(pin), GPIO.HIGH if status else GPIO.LOW)

    def start(self):
        self.setPinStatus(self.IN1, self.HIGH)
        self.setPinStatus(self.IN2, self.LOW)
        self.setPinStatus(self.IN3, self.HIGH)
        self.setPinStatus(self.IN4, self.LOW)
        self.setPinStatus(self.IN5, self.HIGH)
        self.setPinStatus(self.IN6, self.LOW)
        self.setPinStatus(self.IN7, self.HIGH)
        self.setPinStatus(self.IN8, self.LOW)
    
    def stop(self):
        self.setPinStatus(self.IN1, self.LOW)
        self.setPinStatus(self.IN2, self.LOW)
        self.setPinStatus(self.IN3, self.LOW)
        self.setPinStatus(self.IN4, self.LOW)
        self.setPinStatus(self.IN5, self.LOW)
        self.setPinStatus(self.IN6, self.LOW)
        self.setPinStatus(self.IN7, self.LOW)
        self.setPinStatus(self.IN8, self.LOW)

    def down(self):
        self.setPinStatus(self.IN1, self.LOW)
        self.setPinStatus(self.IN2, self.HIGH)
        self.setPinStatus(self.IN3, self.LOW)
        self.setPinStatus(self.IN4, self.HIGH)
        self.setPinStatus(self.IN5, self.LOW)
        self.setPinStatus(self.IN6, self.HIGH)
        self.setPinStatus(self.IN7, self.LOW)
        self.setPinStatus(self.IN8, self.HIGH)

    def test(self):
        GPIO.cleanup(self.IN1)
        GPIO.cleanup(self.IN2)
        GPIO.setup(self.IN1, GPIO.OUT)
        GPIO.setup(self.IN2, GPIO.OUT)
        pwm = GPIO.PWM(self.IN1, 500)
        pwm.start(0)
        GPIO.output(self.IN1, GPIO.HIGH)
        GPIO.output(self.IN2, GPIO.LOW)
        for sp in range(20, 100, 10):
            pwm.ChangeDutyCycle(sp)
            time.sleep(2)
        pwm.stop()
        GPIO.cleanup(self.IN1)
        GPIO.cleanup(self.IN2)

def index(request):
    action = request.GET.get("action")
    
    cc = CarControl()
    getattr(cc, action)()

    response = HttpResponse()
    response["status_code"] = 200
    return response

