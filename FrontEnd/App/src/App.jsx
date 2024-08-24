import React from 'react';
import { BrowserRouter as Router, Route, Routes } from 'react-router-dom';
import Home from './Home';
import Login from './Login';
import About from './About';
import NotFound from './NotFound';
import Report from './Report';
import CareTakerDashboard from './CareTakerDashboard';
import Dashboard from './Dashboard';// Note: Ensure the filename matches
import Patient from './Patient';
import PatientReport from './PatientsREport';
import Monitor from './MoniorThem';
import MedicineReport from './MedicineReport';
import GiveData from './GiveData';

function App() {
  return (
    <Router>
      <div>
        <Routes>
          <Route path="/" element={<Home />} />
          <Route path="/about-page" element={<About />} />
          <Route path="/login" element={<Login />} />
          <Route path="/dashboard" element={<Dashboard />} />
          <Route path="/patient" element={<Patient />} />
          <Route path="/report" element={<Report />} /> 
          <Route path="/medi-report" element={ <MedicineReport/> } />
          <Route path="/caretaker-dashboard" element={<CareTakerDashboard />} />
          <Route path="/caretaker-patient" element={<PatientReport />} />
          <Route path="/caretaker-report" element={<Monitor />} />
          <Route path="/caretaker-give-data" element={<GiveData />} />
          <Route path="*" element={<NotFound />} />
        </Routes>
      </div>
    </Router>
  );
}

export default App;
