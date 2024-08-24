import React from "react";
import { Link } from "react-router-dom";
import "./assets/NotFound.css";

function NotFound() {
    return (
        <div className="not-found">
            <h1>404 Not Found</h1>
            <p>Oops! The page you are looking for does not exist.</p>
            <Link to="/"><button className="back-btn">Go Back</button></Link>
            
        </div>
    );
}

export default NotFound;
