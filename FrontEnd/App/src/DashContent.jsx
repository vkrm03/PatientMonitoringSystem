import React from "react";
import "./assets/DashboardContent.css"

function DashboardContent() {
  return (
    <>
        <div class="main-contents">
            <div class="details">
                <div class="detail-row">
                    <div class="label">Name:</div>
                    <div class="value">Dr. Paul</div>
                </div>
                <div class="detail-row">
                    <div class="label">Email:</div>
                    <div class="value">Paul123@gmail.com</div>
                </div>
                <div class="detail-row">
                    <div class="label">Mobile No:</div>
                    <div class="value">+91 9468312647</div>
                </div>
                <div class="detail-row">
                    <div class="label">Specialization:</div>
                    <div class="value">Diabetes, Hypertension</div>
                </div>
                <div class="detail-row">
                    <div class="label">Gender:</div>
                    <div class="value">Male</div>
                </div>
                <div class="detail-row">
                    <div class="label">Date Of Birth:</div>
                    <div class="value">18/12/1998</div>
                </div>
                <div class="detail-row">
                    <div class="label">Address:</div>
                    <div class="value">32 J/8 North st, Chennai.</div>
                </div>
                <div class="detail-row">
                    <div class="label">Patients</div>
                    <div class="value">8</div>
                </div>
            </div>
        </div>
    </>
  );
}

export default DashboardContent;