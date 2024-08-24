const mongoose = require('mongoose');

const patientdata = new mongoose.Schema({
    glucose : { type: String, required: true },
    weight: { type: String, required: true },
});

const patient = mongoose.model('patientdata', patientdata);

module.exports = patient;
