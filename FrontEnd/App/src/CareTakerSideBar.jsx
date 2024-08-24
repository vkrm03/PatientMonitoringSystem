import React from "react";
import "./assets/Sidebar.css";
import { ToastContainer, toast } from 'react-toastify';

function CareTakerSideBar() {
  return (
    <>
    <div class="cont">
        <div class="sidebar">
            <h1 class="dash">Dashboard</h1>
            <div class="admin-profile">
                <div class="admin-icon"><img src="./patient.png" alt="img" /></div>
                <p>Care Taker</p>
            </div>
            <ul>
                <li><a href="/caretaker-dashboard"><i class="fas fa-home"></i>  My Dash</a></li>
                <li><a href="/caretaker-patient"><i class="fa-solid fa-bed-pulse"></i> Monitor Them</a></li>
                <li><a href="/caretaker-report"><i class="fa-solid fa-notes-medical"></i>  My Report</a></li>
                <li><a href="/caretaker-give-data"><i class="fa-solid fa-notes-medical"></i>  Give Data</a></li>
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

export default CareTakerSideBar;