import React, { useState } from 'react';
import axios from 'axios';
import emailjs from 'emailjs-com';
import { ToastContainer, toast } from 'react-toastify';
import 'react-toastify/dist/ReactToastify.css';
import { useNavigate } from 'react-router-dom';

// Inline styles
const formStyle = {
  maxWidth: '800px',
  margin: 'auto auto',
  padding: '30px',
  borderRadius: '10px',
  boxShadow: '0 5px 15px rgba(0,0,0,0.1)',
  backgroundColor: '#ffffff',
  fontFamily: 'Arial, sans-serif',
};

const inputContainerStyle = {
  display: 'flex',
  alignItems: 'center',
  marginBottom: '15px',
};

const inputStyle = {
  flex: 1,
  padding: '12px',
  margin: '0 10px 0 0',
  borderRadius: '6px',
  border: '1px solid #ccc',
  fontSize: '16px',
  boxSizing: 'border-box',
  transition: 'border-color 0.3s ease-in-out',
};

const inputFocusStyle = {
  borderColor: '#007bff',
};

const addButtonStyle = {
  backgroundColor: '#007bff',
  color: '#fff',
  padding: '10px 15px',
  border: 'none',
  borderRadius: '6px',
  cursor: 'pointer',
  fontSize: '18px',
  marginLeft: '0px',
  transition: 'background-color 0.3s ease-in-out',
};

const addButtonHoverStyle = {
  backgroundColor: '#0056b3',
};

const submitButtonStyle = {
  backgroundColor: '#28a745',
  color: '#fff',
  padding: '12px 20px',
  border: 'none',
  borderRadius: '6px',
  cursor: 'pointer',
  fontSize: '18px',
  marginTop: '20px',
  width: '100%',
  transition: 'background-color 0.3s ease-in-out',
};

const submitButtonHoverStyle = {
  backgroundColor: '#218838',
};

const MedicineForm = () => {
  const [medicines, setMedicines] = useState([{ name: '', dosage: '', frequency: '' }]);
  const [addButtonHover, setAddButtonHover] = useState(false);
  const [submitButtonHover, setSubmitButtonHover] = useState(false);

  const navigate = useNavigate();

  const handleChange = (e, index) => {
    const { name, value } = e.target;
    const newMedicines = [...medicines];
    newMedicines[index][name] = value;
    setMedicines(newMedicines);
  };

  const handleAddMedicine = () => {
    setMedicines([...medicines, { name: '', dosage: '', frequency: '' }]);
  };

  const handleSubmit = async (e) => {
    e.preventDefault();
    const currentDate = new Date().toLocaleDateString();

    try {
        // Send data to your backend
        await axios.post('http://localhost:5000/api/medicines', { medicines, date: currentDate });

        // Send email using EmailJS
        const templateParams = {
            medicines: JSON.stringify(medicines, null, 2),
            date: currentDate,
        };

        emailjs.send('service_5ft0ttb', 'template_53d8vp8', templateParams, 'A8JHeJGbREZaFwJCG')
            .then((response) => {
                console.log('Email sent successfully!', response.status, response.text);
                
                // Show success toast notification
                toast.success('Medicine details submitted successfully!', {
                    position: "top-center",
                    autoClose: 2000,
                    hideProgressBar: false,
                    closeOnClick: true,
                    pauseOnHover: true,
                    draggable: true,
                    progress: undefined,
                });
                setTimeout(() => {
                    setMedicines([{ name: '', dosage: '', frequency: '' }]);
                    navigate('/medi-report');
                  }, 1000);
                
            })
            .catch((err) => {
                console.error('Failed to send email. Error: ', err);

                // Show error toast notification
                toast.error('Failed to send email. Please try again.', {
                    position: "top-center",
                    autoClose: 2000,
                    hideProgressBar: false,
                    closeOnClick: true,
                    pauseOnHover: true,
                    draggable: true,
                    progress: undefined,
                });
            });
    } catch (error) {
        console.error('Error submitting medicine details:', error);

        // Show error toast notification
        toast.error('Failed to submit medicine details. Please try again.', {
            position: "top-center",
            autoClose: 2000,
            hideProgressBar: false,
            closeOnClick: true,
            pauseOnHover: true,
            draggable: true,
            progress: undefined,
        });
    }
};


  return (
    <form onSubmit={handleSubmit} style={formStyle}>
      {medicines.map((medicine, index) => (
        <div key={index} style={inputContainerStyle}>
          <input
            type="text"
            name="name"
            placeholder="Medicine Name"
            value={medicine.name}
            onChange={(e) => handleChange(e, index)}
            style={{ ...inputStyle, ...(medicine.name && inputFocusStyle) }}
            required
          />
          <input
            type="text"
            name="dosage"
            placeholder="Dosage"
            value={medicine.dosage}
            onChange={(e) => handleChange(e, index)}
            style={{ ...inputStyle, ...(medicine.dosage && inputFocusStyle) }}
            required
          />
          <input
            type="text"
            name="frequency"
            placeholder="Description"
            value={medicine.frequency}
            onChange={(e) => handleChange(e, index)}
            style={{ ...inputStyle, ...(medicine.frequency && inputFocusStyle) }}
            required
          />
        </div>
      ))}
      <button
        type="button"
        onClick={handleAddMedicine}
        style={{ ...addButtonStyle, ...(addButtonHover && addButtonHoverStyle) }}
        onMouseEnter={() => setAddButtonHover(true)}
        onMouseLeave={() => setAddButtonHover(false)}
      >
        +
      </button>
      <button
        type="submit"
        style={{ ...submitButtonStyle, ...(submitButtonHover && submitButtonHoverStyle) }}
        onMouseEnter={() => setSubmitButtonHover(true)}
        onMouseLeave={() => setSubmitButtonHover(false)}
      >
        Submit
      </button>
    </form>
  );
};

export default MedicineForm;
