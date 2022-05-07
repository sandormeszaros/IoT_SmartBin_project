import React, {useEffect, useState} from 'react'
import io from 'socket.io-client'

import Card from './Card'
import Chart from './Chart'
import C2DMessage from './C2DMessage'
import Progressbar from './Progressbar'

const socket = io('http://localhost:5000')

let distance = 0.0;

const Dashboard = () => {
    const [data, setData] = useState([])

    useEffect(() => {
        socket.on('payload', payload => {
            setData(currentData => [...currentData, payload])
            distance = payload.Distance;
            //humidity = payload.Humidity;
        })
    }, [])
    if (data.length > 10) {
        data.shift()
    }

    return <div>
        <h1 className="title">Azure Dashboard</h1>
         
        <div className="card-container">
            <Card name="Distance" icon="fas fa-trash fa-2x" value={distance}/>
        </div>
        <div className="progress-bar">
            <Progressbar value={distance}/>
        </div>
        <div className="chart-container">
            <Chart name="Distance" data={data} xDataKey="date" yDataKey="Distance" stroke="#8884d8" fill="#8884d8"/>
        </div>
    </div>
}

export default Dashboard
