import React, { useState } from "react";
import axios from "axios";
import "./assets/IotDatas.css";

function IotDatas() {
  const [glucose, setGlucose] = useState(null);
  const [weight, setWeight] = useState(null);
  const [heartRate, setHeartRate] = useState(null);
  const [spo2, setSpo2] = useState(null);

  const getCurrentData = async () => {
    try {
      const response = await axios.get("http://localhost:5000/api/get-sensor-data");
      setGlucose(response.data[1].glucose);
      setWeight(response.data[1].weight);
      setHeartRate(response.data[0].heart_rate);
      setSpo2(response.data[0].spo2);
    } catch (error) {
      console.error("Error fetching sensor data:", error);
    }
  };

  return (
    <div className="iot-datas-container">
      <h2>Real-Time Sensor Readings</h2>
      <div className="sensor-reading">
        <div>Glucose Level: {glucose ? `${glucose} mg` : "Connecting...."}</div>
        <div>Weight: {weight ? `${weight} kg` : "Connecting...."}</div>
        <div>Heart Rate: {heartRate ? `${heartRate} bpm` : "Connecting...."}</div>
        <div>SpO2: {spo2 ? `${spo2} %` : "Connecting...."}</div>
        <button onClick={getCurrentData} className="btn">Get Current Data</button>
      </div>
    </div>
  );
}

export default IotDatas;
