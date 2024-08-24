import React from "react";
import "./assets/Home.css";
import NavBar from './NavBar';

function Home() {
    return (
        
        <div className="main-content">
            <NavBar/>
            <div className="welcome-section">
                <div className="welcome-text">
                    <h1 >Welcome to Home Care !</h1>
                    <p>Home health care is tailored to meet the specific needs of each patient.</p>
                    <button className="start-chat-btn">Login Now</button>
                </div>
                <div className="welcome-image">
                    <img src="./HomeHealth.png" alt="Welcome to ChatUp" />
                </div>
            </div>
            <h1 className="test-text">Features</h1>
            <div className="features-section">
            
                <div className="feature">
                    <i class="fa-solid fa-disease"></i>
                    <h2>Real-time Detection</h2>
                    <p>Experience real-time Eco friendly communication with Our "Dr.IOT" Device to Doctor-and-CareTaker. Instant delivery, instant replies.</p>
                </div>
                <div className="feature">
                    <i className="fas fa-user-shield"></i>
                    <h2>UI & UX</h2>
                    <p>An User friendly Web App with patient's real time datas.</p>
                </div>
                <div className="feature">
                    <i className="fas fa-smile"></i>
                    <h2>Messaging Alerts</h2>
                    <p>Express SMS messaging Alerts when patients Health wents abnormal.</p>
                </div>
                <div className="feature">
                    <i class="fa-solid fa-capsules"></i>
                    <h2>Medicine Sharing</h2>
                    <p>By real time analysis of the patient's condition Care Taker can easily get Medicines by mail.</p>
                </div>
            </div>

            <section className="testimonials">
                <h2 className="test-text">What Our Users Say</h2>
                <div className="testimonial">
                    <p>"The user-friendly interface and reliable features make it more interative !"</p>
                    <p className="author">- Sachin</p>
                </div>
                <div className="testimonial">
                    <p>"I love the Evaluate sensor's accuracy and reliability."</p>
                    <p className="author">- Arun Mathew</p>
                </div>
                <div className="testimonial">
                    <p>"Highly recommended for anyone who values their Lives."</p>
                    <p className="author">- Prabha</p>
                </div>
            </section>
        </div>
    );
}

export default Home;
