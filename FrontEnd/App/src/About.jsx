import React from 'react';
import './assets/About.css';
import NavBar from './NavBar';

const About = () => {
  return (
    <div><NavBar/>
    <div className="about-container">
      <h1>About Us</h1>
      <p>
        Welcome to <strong>Home Care</strong> – your trusted partner in compassionate, personalized care. 
        We understand the importance of comfort, independence, and dignity in the healing process. That’s why we bring quality 
        healthcare services right to your doorstep.
      </p>

      <section className="about-section">
        <h2>Our Mission</h2>
        <p>
          Our mission is to enhance the quality of life for individuals by providing exceptional home health care services that 
          are tailored to meet the unique needs of each client. We are committed to delivering care with respect, empathy, and 
          professionalism, ensuring that our clients feel supported every step of the way.
        </p>
      </section>

      <section className="about-section">
        <h2>Our Services</h2>
        <p>
          We offer a comprehensive range of home health services, including skilled nursing, physical therapy, occupational therapy, 
          speech therapy, and personal care assistance. Whether recovering from surgery, managing a chronic illness, or needing daily 
          support, our team of licensed and experienced healthcare professionals is here to help.
        </p>
      </section>

      <section className="about-section">
        <h2>Why Choose Us?</h2>
        <ul>
          <li><strong>Personalized Care Plans:</strong> We develop care plans that are specifically designed to meet the individual needs of our clients.</li>
          <li><strong>Experienced Team:</strong> Our team consists of highly qualified professionals who are dedicated to providing the best possible care.</li>
          <li><strong>Compassionate Approach:</strong> We treat our clients like family, offering compassionate and respectful care that promotes independence and well-being.</li>
          <li><strong>Convenience and Comfort:</strong> By delivering healthcare services at home, we help our clients maintain their comfort and independence while receiving the care they need.</li>
        </ul>
      </section>

      <section className="about-section">
        <h2>Our Commitment</h2>
        <p>
          At [Your Home Health Company Name], we are committed to providing exceptional care that not only meets but exceeds the expectations of our clients. 
          We believe in the power of compassionate care and the difference it can make in the lives of those we serve.
        </p>
      </section>

      <section className="about-section">
        <h2>Get in Touch</h2>
        <p>
          If you or a loved one need home health care services, please don’t hesitate to contact us. Our friendly team is ready to answer any questions and help 
          you get started on the path to better health and well-being in the comfort of your own home.
        </p>
      </section>
    </div>
    </div>
  );
};

export default About;
