import React, { useState } from "react";
import CareTakerSideBar from "./CareTakerSideBar";
import axios from "axios";
import "./assets/GiveData.css"

function GiveData() {
    const [glucose, setGlucose] = useState('');
    const [weight, setWeight] = useState('');

    const handleSubmit = async (event) => {
        event.preventDefault();

        try {
            const response = await axios.post('http://localhost:5000/api/givedata', {
                glucose: glucose,
                weight: weight
            });

            console.log('Data submitted successfully:', response.data);
            setGlucose("");
            setWeight("");
        } catch (error) {
            console.error('Error submitting data:', error);
        }
    };

    return( 
        <div>
            <CareTakerSideBar />
            <div className="form-container">
                <form onSubmit={handleSubmit} className="form">
                    <div>
                        <label htmlFor="glucose">Patient's Glucose:</label>
                        <input 
                            type="number" 
                            id="glucose" 
                            value={glucose} 
                            onChange={(e) => setGlucose(e.target.value)} 
                            required 
                        />
                    </div>
                    <div>
                        <label htmlFor="weight">Patient's Weight:</label>
                        <input 
                            type="number" 
                            id="weight" 
                            value={weight} 
                            onChange={(e) => setWeight(e.target.value)} 
                            required 
                        />
                    </div>
                    <button type="submit">Submit</button>
                </form>
            </div>
        </div>
     );
}

export default GiveData;
