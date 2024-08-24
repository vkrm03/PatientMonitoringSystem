import React from "react";
import "./assets/DashboardContent.css"

function CareTakerContent() {
  return (
    <>
        <div class="main-contents">
            <div class="details">
                <div class="detail-row">
                    <div class="label">Care Taker Name:</div>
                    <div class="value">Mr. Jhon</div>
                </div>
                <div class="detail-row">
                    <div class="label">Patient Name:</div>
                    <div class="value">Ms. Pallavi</div>
                </div>
                <div class="detail-row">
                    <div class="label">Email:</div>
                    <div class="value">Jhon@gmail.com</div>
                </div>
                <div class="detail-row">
                    <div class="label">Mobile No:</div>
                    <div class="value">+91 9468312647</div>
                </div>
                <div class="detail-row">
                    <div class="label">Problem:</div>
                    <div class="value">Diabetes, Hypertension</div>
                </div>
                <div class="detail-row">
                    <div class="label">Gender:</div>
                    <div class="value">Female</div>
                </div>
                <div class="detail-row">
                    <div class="label">Patient Date Of Birth:</div>
                    <div class="value">18/12/1998</div>
                </div>
                <div class="detail-row">
                    <div class="label">Address:</div>
                    <div class="value">32 J/8 North st, Chennai.</div>
                </div>
            </div>
        </div>
    </>
  );
}

export default CareTakerContent;