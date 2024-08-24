import React from 'react';
import { Link } from 'react-router-dom';
import './assets/NavBar.css';

function NavBar() {
  return (
    <div className="nav">
      <h2>
        <Link to="/" className="nav-logo">
          <i className="fa-solid fa-stethoscope"></i> Home Care
        </Link>
      </h2>
      <nav>
        <ul className="nav-links">
          <li><Link to="/" className="nav-item">Home</Link></li>
          <li><Link to="/about-page" className="nav-item">About</Link></li>
        </ul>
      </nav>
      
        <button className="nav-btn">
          <Link to="/login" className="nav-btn-link">Login Now</Link>
        </button>
    </div>
  );
}

export default NavBar;
