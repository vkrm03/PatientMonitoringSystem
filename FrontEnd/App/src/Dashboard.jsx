import React from "react";
import DashboardContent from "./DashContent";
import Sidebar from "./Sidebar";

function Dashboard() {
  return (
    <>
        <Sidebar />
        <DashboardContent />
    </>
  );
}

export default Dashboard;