import time
import serial
  
ser = serial.Serial('/dev/ttyUSB0', 115200)
 
def send_email():
    print("Sending Email")
    smtpserver = smtplib.SMTP("smtp.gmail.com",587)
    smtpserver.ehlo()
    smtpserver.starttls()
    smtpserver.ehlo
    smtpserver.login(GMAIL_USER, GMAIL_PASS)
    header = 'To:' + TO + '\n' + 'From: ' + GMAIL_USER
    header = header + '\n' + 'Subject:' + SUBJECT + '\n'
    print header
    msg = header + '\n' + TEXT + ' \n\n'
    smtpserver.sendmail(GMAIL_USER, TO, msg)
    smtpserver.close()

def handleLine(line):
    print line.split(' ');
    

print "start"
#time.sleep(2)
#print "writing"
#ser.write("r 0 3\n")
#ser.flush()

while True:
    time.sleep(1)

    ser.write("r 0 3\n")

    message = ser.readline()
    print(message)
    if message[0] == '$' :
        handleLine()

