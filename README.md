Diabetes and Hypertension Patient Monitoring System:
This project is a comprehensive patient monitoring system designed for managing diabetes and hypertension. The system integrates IoT technology with a web application, enabling real-time monitoring of patients' vital signs and seamless communication between doctors and caretakers. The system is divided into two main components: the IoT device for real-time data collection and the web application for data management and interaction.

Features
Real-time Monitoring: Measure and display real-time heart rate and SpO2 levels using a pulse oximetry sensor connected to an Arduino R3.
Web Application: A MERN stack web app with separate dashboards for doctors and caretakers.
Data Visualization: Real-time data visualization on the web app to monitor patient vitals.
Emergency Alerts: A panic button that triggers an emergency alert to notify the caretaker.
Medicine Management: Doctors can prescribe medicines based on patient data, and the prescription is sent to the caretaker via email.
Abnormal Health Alerts: Automated notifications are sent to the caretaker if the patient's health shows abnormal readings.
Tech Stack
IoT
Arduino R3: Microcontroller used for data collection from sensors.
Pulse Oximetry Sensor: Measures heart rate and SpO2 levels.
LCD Display: Shows real-time sensor data.
Panic Button: Sends emergency alerts.
Web Application
MongoDB: Database to store patient data, prescriptions, and user information.
Express.js: Backend framework for building the server.
React.js: Frontend library for building the user interface.
Node.js: JavaScript runtime for building the backend server.
System Architecture
IoT Device: The Arduino R3 collects data from the pulse oximetry sensor and displays it on an LCD screen. The data is also transmitted to the web app.
Web Application: The MERN stack-based web app handles user authentication, data visualization, and communication between doctors and caretakers.
Data Flow:
The caretaker logs in to the web app and inputs the patient's glucose levels.
The doctor receives this data, analyzes it, and prescribes the necessary medications.
The prescribed medication details are emailed to the caretaker.
If abnormal health data is detected, the system sends an alert to the caretaker.
Installation & Setup
Clone the repository:

bash
Copy code
git clone https://github.com/vkrm03/PatientMonitoringSystem.git
cd project-name
Backend Setup:

Install dependencies:
bash
Copy code
cd backend
npm install
Create a .env file and add your MongoDB connection string, email credentials, and other environment variables.
Start the server:
bash
Copy code
npm start
Frontend Setup:

Install dependencies:
bash
Copy code
cd frontend
npm install
Start the React app:
bash
Copy code
npm start
Arduino Setup:

Upload the provided Arduino code to the Arduino R3 board.
Connect the pulse oximetry sensor, LCD, and panic button according to the circuit diagram provided in the repository.
Usage
Doctor Login: Access the doctor's dashboard to view patient data and prescribe medications.
Caretaker Login: Access the caretaker's dashboard to input patient glucose levels and receive medication instructions.
Monitoring: Use the web app to monitor the patient's real-time health data.
Emergency Alert: Press the panic button on the IoT device to send an emergency alert.
Contributing
Contributions are welcome! Please submit a pull request or open an issue to discuss any changes.

License
This project is licensed under the MIT License.

Contact
For any queries, please contact your-email@example.com.
