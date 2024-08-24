const mongoose = require('mongoose');

// Define the schema for each medicine entry
const medicineSchema = new mongoose.Schema({
    name: { type: String, required: true },
    dosage: { type: String, required: true },
    frequency: { type: String, required: true },
});

// Define the schema for the overall report
const reportSchema = new mongoose.Schema({
    medicines: { type: [medicineSchema], required: true }, // Array of medicine entries
    date: { type: String, required: true }, // The date of the report
});

const Report = mongoose.model('report', reportSchema);

module.exports = Report;
