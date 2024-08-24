import React, { useState } from 'react';
import './assets/Login.css';
import axios from 'axios';
import { useNavigate } from 'react-router-dom'; // For redirecting
import { ToastContainer, toast } from 'react-toastify';
import 'react-toastify/dist/ReactToastify.css';
import NavBar from './NavBar';

function Login() {  
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');
  const [service, setService] = useState('doc');
  const navigate = useNavigate();

  const handleSubmit = async (e) => {
    e.preventDefault();
    try {
      const response = await axios.post('http://localhost:5000/api/login', { 
        username, 
        password, 
        service 
      });      
      console.log('Login successful', response.data.user);
      toast.success('Login successful!', {
        position: "top-center",
        autoClose: 2000,
        hideProgressBar: false,
        closeOnClick: true,
        pauseOnHover: true,
        draggable: true,
        progress: undefined,
      });
      setTimeout(() => {
        const usertype = response.data.user.role;
        if (usertype === "doc") {
          navigate('/dashboard');
        } else {
          navigate('/caretaker-dashboard');
        }
        
      }, 1000);

    } catch (error) {
      console.error('Error logging in:', error.response ? error.response.data : error.message);
      toast.error('Login failed. Please try again.', {
        position: "top-right",
        autoClose: 1000,
        hideProgressBar: false,
        closeOnClick: true,
        pauseOnHover: true,
        draggable: true,
        progress: undefined,
      });
    }
  };

  return (
    <div className="login-container">
      <NavBar/>
      <ToastContainer />
      <div className="login-form">
        <h2>Login to Home Care</h2>
        <form onSubmit={handleSubmit}>
          <select 
            id="services" 
            name="services" 
            className="custom-select"
            value={service}
            onChange={(e) => setService(e.target.value)}
          >
            <option value="doc">Doctor</option>
            <option value="care-taker">Care Taker</option>
          </select>
          <div className="form-group">
            <label htmlFor="username"><i className="fa-solid fa-user"></i> Username</label>
            <input 
              type="text" 
              id="username" 
              name="username" 
              placeholder="Enter your username" 
              value={username}
              onChange={(e) => setUsername(e.target.value)}
            />
          </div>
          <div className="form-group">
            <label htmlFor="password"><i className="fa-solid fa-lock"></i> Password</label>
            <input 
              type="password" 
              id="password" 
              name="password" 
              placeholder="Enter your password" 
              value={password}
              onChange={(e) => setPassword(e.target.value)}
            />
          </div>
          <button type="submit" className='login-btn'>Login</button>
        </form>
      </div>
    </div>
  );
}

export default Login;
