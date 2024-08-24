import React, { useState, useEffect } from "react";
import "./assets/MediReportContent.css";
import axios from 'axios';

function MedicineReportContent() {
    const [reports, setReports] = useState([]);

    useEffect(() => {
        const fetchData = async () => {
            try {
                const response = await axios.get('http://localhost:5000/api/medi-report');
                setReports(response.data);
            } catch (err) {
                console.log(err);
            }
        };
        fetchData();
    }, []);

    return (
        <>
            <table className="marks-table">
                <thead>
                    <tr>
                        <th>Medicine Name</th>
                        <th>Dosage</th>
                        <th>Frequency</th>
                        <th>Date</th>
                    </tr>
                </thead>
                <tbody>
                    {reports.map((report, index) => (
                        report.medicines.map((medicine, idx) => (
                            <tr key={`${index}-${idx}`}>
                                <td>{medicine.name}</td>
                                <td>{medicine.dosage}</td>
                                <td>{medicine.frequency}</td>
                                {idx === 0 && <td rowSpan={report.medicines.length}>{report.date}</td>}
                            </tr>
                        ))
                    ))}
                </tbody>
            </table>
        </>
    );
}

export default MedicineReportContent;
