import React from 'react'
import {Area, AreaChart, CartesianGrid, ResponsiveContainer, Tooltip, XAxis, YAxis} from 'recharts';

const Chart = ({name, data, xDataKey, yDataKey, stroke, fill}) => {
    return <div>
        <h1>{name}</h1>
        <div className="row">

            <ResponsiveContainer width="99%" aspect={3}>
                <AreaChart
                    data={data}
                    className="areachart"
                >
                    <CartesianGrid strokeDasharray="3 3"/>
                    <XAxis dataKey={xDataKey}/>
                    <YAxis/>
                    <Tooltip/>

                    <Area type="monotone" dataKey={yDataKey} stroke={stroke} fill={fill}/>
                </AreaChart>
            </ResponsiveContainer>
        </div>
    </div>
}

export default Chart
