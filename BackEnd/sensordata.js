const mongoose = require('mongoose');

const sensordataschema = new mongoose.Schema({
    sensor_id : { type: String, required: true },
    heart_rate: { type: Number, required: true },
    spo2: { type: Number, required: true },
});

const sensordata = mongoose.model('vital_signs', sensordataschema);

module.exports = sensordata;
