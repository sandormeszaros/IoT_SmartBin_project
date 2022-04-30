import React, {useEffect, useState} from 'react'
import io from 'socket.io-client'

import Card from './Card'
import Chart from './Chart'
import C2DMessage from './C2DMessage'

const socket = io('http://localhost:5000')

let temperature = 0.0;
let humidity = 0.0;

const Dashboard = () => {
    const [data, setData] = useState([])

    useEffect(() => {
        socket.on('payload', payload => {
            setData(currentData => [...currentData, payload])
            temperature = payload.Distance;
            //humidity = payload.Humidity;
        })
    }, [])
    if (data.length > 10) {
        data.shift()
    }

    return <div>
        <h1 className="title">Azure Dashboard</h1>
        <C2DMessage/>
        <div className="card-container">
            <Card name="Temperature" icon="fas fa-tachometer-alt fa-2x" value={temperature}/>
            <Card name="Humidity" icon="fas fa-water fa-2x" value={temperature}/>
        </div>
        <div className="chart-container">
            <Chart name="Temperature" data={data} xDataKey="date" yDataKey="Temperature" stroke="#8884d8"
                   fill="#8884d8"/>
            <Chart name="Humidity" data={data} xDataKey="date" yDataKey="Humidity" stroke="#82ca9d" fill="#82ca9d"/>
        </div>
    </div>
}

export default Dashboard
