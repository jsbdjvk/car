class CarControl:
    IN1 = 23
    IN1 = 24

    def __init__(self):
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(IN5, GPIO.OUT)
        GPIO.setup(IN6, GPIO.OUT)

    def __del__(self):
        GPIO.cleanup()

    def start(self):
        GPIO.output(IN5, GPIO.HIGH)
        GPIO.output(IN6, GPIO.LOW)
    
    def stop(self):
        GPIO.output(IN5, GPIO.LOW)
        GPIO.output(IN6, GPIO.LOW)
