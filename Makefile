INO := at2xtkbd.ino
OPTS := --board arduino:avr:uno --port /dev/ttyUSB0 --verbose

all: serial

serial: upload
	stty -F /dev/ttyUSB0 cs8 9600 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts
	cat /dev/ttyUSB0

verify:
	arduino --verify $(OPTS) $(INO)

upload:
	arduino --upload $(OPTS) $(INO)
