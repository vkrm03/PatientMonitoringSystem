const express = require('express');
const cors = require('cors');
const bodyParser = require('body-parser');
const app = express();
const mongoose = require('mongoose');
const Reports = require("./reports");
const patient = require("./patientdata");
const sensordata = require("./sensordata");



mongoose.connect("mongodb+srv://vikram:vkrm@csoft.b9q3blg.mongodb.net/").then(() => {
    console.log("Connected to DB");
  }).catch(err => {
    console.log(err);
  });


app.use(cors());
app.use(bodyParser.json());

const users = [
  { username: 'asdf', password: 'asdf', role: 'doc' },
  { username: 'asdf', password: 'asdf', role: 'care-taker' },
];

app.post('/api/login', (req, res) => {
  const { username, password, service } = req.body;
  const user = users.find(u => u.username === username && u.password === password && u.role === service);
  if (user) {
    res.status(200).json({ message: 'Login successful', user: { username: user.username, role: user.role } });
  } else {
    res.status(401).json({ message: 'Invalid username, password, or service role' });
  }
  console.log(`Login attempt: Username - ${username}, Service - ${service}`);
});

app.post('/api/medicines', async (req, res) => {
  const medi_data = req.body;
  console.log(medi_data);
  const newMedicineData = new Reports(req.body);
  await newMedicineData.save();
  res.status(200).json({ message: 'Email sent successful'});
});

app.post('/api/givedata', async (req, res) => {
  try {
    const {glucose, weight } = req.body;
    const updatedPatient = await patient.findOneAndUpdate(
      { _id: "66c8d06dc26cef45e62715c3" },
      { glucose, weight },
      { new: true, upsert: true }
    );

    res.status(200).json({ message: 'Patient data updated successfully', data: updatedPatient });
  } catch (error) {
    res.status(500).json({ message: 'Error updating patient data', error: error.message });
  }
});

app.get('/api/medi-report', async (req, res) => {
  try {
    const report = await Reports.find();    
    res.status(200).json(report);
} catch (err) {
    res.status(500).json({ error: 'Failed to retrieve reports' });
}
});

app.get('/api/get-sensor-data', async (req, res) => {
  try {
    const report = await sensordata.find();
    const alsoreport = await patient.find();
    const combinedReport = [...report, ...alsoreport];
    res.status(200).json(combinedReport);
} catch (err) {
    res.status(500).json({ error: 'Failed to retrieve reports' });
}
});

app.use((err, req, res, next) => {
  console.error(err.stack);
  res.status(500).send('Something went wrong!');
});

const PORT = process.env.PORT || 5000;
app.listen(PORT, () => {
  console.log(`Server is running on port ${PORT}`);
});
