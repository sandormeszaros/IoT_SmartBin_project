import React from 'react'
import axios from 'axios'

const C2DMessage = () => {

    const sendText = (id) => () => {
        axios.get(`http://localhost:5000/c2d/${id}`)
            .then(res => console.log(res.data))
    }

    return (
        <>
            <h1 className="c2d-title">Cloud 2 Device Messaging</h1>
            <div className="c2d-container">
                <button onClick={sendText(1)}>Send 1</button>
                <button onClick={sendText(2)}>Send 2</button>
            </div>
        </>
    )

}

export default C2DMessage
