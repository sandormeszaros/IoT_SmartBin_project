import axios from 'axios'
import React, { useEffect, useState } from "react";
import DatePicker from "react-datepicker";
import "react-datepicker/dist/react-datepicker.css";

const C2DMessage = () => {

    const sendText = (date) => () => {
        axios.get(`http://localhost:5000/c2d/${date}`)
            .then(res => {
                console.log(res.data)
                console.log(date)
            })
    }

    const [startDate, setStartDate] = useState(new Date());

    return (
        <>
            <h1 className="c2d-title">Send the next emptying date to device</h1>
            <div className="center">
                <DatePicker id="datepicker" selected={startDate} onChange={(date) => setStartDate(date)} />
            </div>
            <div className="c2d-container">
                <button onClick={sendText(startDate)}>Send date</button>
            </div>
        </>
    )

}

export default C2DMessage
