import React from "react";
import "./assets/Sidebar.css";
import { ToastContainer, toast } from 'react-toastify';

function Sidebar() {
  return (
    <>
    <div class="cont">
        <div class="sidebar">
            <h1 class="dash">Dashboard</h1>
            <div class="admin-profile">
                <div class="admin-icon"><img src="./doc.jpeg" alt="img" /></div>
                <p>Doctor</p>
            </div>
            <ul>
                <li><a href="/dashboard"><i class="fas fa-home"></i>  My Dash</a></li>
                <li><a href="/patient"><i class="fa-solid fa-bed-pulse"></i> Monitor Patient</a></li>
                <li><a href="/report"><i class="fa-solid fa-notes-medical"></i>  Report ++</a></li>
                <li><a href="/medi-report"><i class="fa-solid fa-notes-medical"></i> Medicine Report</a></li>
                <li><a href="/" onClick={toast.success('Logout successful!', {
        position: "top-center",
        autoClose: 2000,
        hideProgressBar: false,
        closeOnClick: true,
        pauseOnHover: true,
        draggable: true,
        progress: undefined,
      })}><i class="fas fa-sign-out-alt"></i> Log Out</a></li>
            </ul>
        </div>
    </div>
    </>
  );
}

export default Sidebar;