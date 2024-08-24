import serial
import time
from pymongo import MongoClient, errors
import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart


# MongoDB connection details
mongo_uri = "mongodb+srv://vikram:vkrm@csoft.b9q3blg.mongodb.net/"
db_name = "test"
collection_name = "vital_signs"

# Serial port configuration
serial_port = 'COM6' 
baud_rate = 9600

# Open serial port
try:
    ser = serial.Serial(serial_port, baud_rate, timeout=1)
    time.sleep(2)  
except serial.SerialException as e:
    print(f"Error opening serial port: {e}")
    exit()


while True:
    try:
        client = MongoClient(mongo_uri, serverSelectionTimeoutMS=5000)
        db = client[db_name]
        collection = db[collection_name]
        print("Connected to MongoDB.")
        break
    except errors.ServerSelectionTimeoutError as err:
        print(f"MongoDB connection error: {err}")
        time.sleep(5) 

# Unique identifier for the document to be updated
sensor_id = "unique_sensor_id"

def send_sms():
    to_number = 'vkrmtemp@gmail.com'

    # Email credentials
    smtp_server = 'smtp.gmail.com'
    smtp_port = 587
    email = 'vkrmtemp@gmail.com'
    password = 'mmfr apud ignj jkoh'

    subject = "Emergency Alert! Patient is in trouble!"
    body = "Patient Heart is High"

    msg = MIMEMultipart()
    msg['From'] = email
    msg['To'] = to_number
    msg['Subject'] = subject

    msg.attach(MIMEText(body, 'plain'))

    try:
        server = smtplib.SMTP(smtp_server, smtp_port)
        server.starttls()
        server.login(email, password)
        server.sendmail(email, to_number, msg.as_string())
        print("Message sent successfully!")
    except Exception as e:
        print(f"Failed to send message: {e}")
    finally:
        server.quit()


def update_mongodb(hr, sp):
    """Updates the heart rate and SpO2 data in MongoDB."""
    data = {
        "sensor_id": sensor_id,
        "heart_rate": int(hr),
        "spo2": int(sp),
        "timestamp": time.time()
    }
    try:
        result = collection.update_one(
            {"sensor_id": sensor_id},
            {"$set": data},
            upsert=True 
        )
        if result.matched_count > 0:
            print("Data updated in MongoDB.")
        else:
            print("New document created in MongoDB.")

        if data["heart_rate"] > 60:
            send_sms()

    except errors.PyMongoError as e:
        print(f"MongoDB update error: {e}")

output_count = 0
max_outputs = 40

try:
    while output_count < max_outputs:
        if ser.in_waiting > 0:
            try:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line and ',' in line:
                    hr, sp = line.split(',')
                    if hr.isdigit() and sp.isdigit():
                        print(f"Heart Rate: {hr} bpm, SpO2 Levels: {sp}%")
                        update_mongodb(hr, sp)
                        output_count += 1 
                    else:
                        print(f"Invalid data: {line}")
                else:
                    print(f"Unexpected line format: {line}")
            except ValueError:
                print(f"Error splitting line: {line}")
            except UnicodeDecodeError as e:
                print(f"Unicode decode error: {e}")
        time.sleep(1)
except KeyboardInterrupt:
    print("Program interrupted by user.")
finally:
    ser.close()
    client.close()
    print(f"Stopped after {output_count} updates.")
