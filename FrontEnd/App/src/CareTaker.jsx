// src/pages/CaretakerDashboard.jsx
import React from 'react';
import Dashboard from './Dashboard';

const CaretakerDashboard = () => {
  return (
    <Dashboard title="Caretaker's Dashboard">
      <div>
        {/* Add Caretaker-specific widgets and charts here */}
        <h2>Welcome, Caretaker</h2>
        <p>Here is your summary for today.</p>
        {/* Example content */}
      </div>
    </Dashboard>
  );
};

export default CaretakerDashboard;
