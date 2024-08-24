import React from 'react';
import './assets/Login.css';
import { Link } from 'react-router-dom';

function Register() {
  return (
    <div className="login-container">
      <div className="login-form">
        <h2>Register to ChatUp</h2>
        <form>
          <div className="form-group">
            <label htmlFor="username"><i class="fa-solid fa-user"></i> Username</label>
            <input type="text" id="username" name="username" placeholder="Enter your username" />
          </div>
          <div className="form-group">
            <label htmlFor="password"><i class="fa-solid fa-lock"></i> Password</label>
            <input type="password" id="password" name="password" placeholder="Enter the password" />
          </div>
          <div className="form-group">
            <label htmlFor="password"><i class="fa-solid fa-lock"></i> Confirm Password</label>
            <input type="password" id="password" name="password" placeholder="Confirm password" />
          </div>
          <button type="submit" className='login-btn'>Register</button>
        </form>
      </div>
    </div>
  );
}

export default Register;
