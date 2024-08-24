import { useState, useEffect } from 'react';
import { useNavigate } from 'react-router-dom';
const Logout = () => {
    const navigate = useNavigate();
    localStorage.removeItem('user');
    setTimeout(() => {
        navigate('/');
      }, 1000);
};

export default Logout;
