import React from 'react'
import {Area, AreaChart, CartesianGrid, ResponsiveContainer, Tooltip, XAxis, YAxis} from 'recharts';

const Chart = ({name, data, xDataKey, yDataKey, stroke, fill}) => {
    return <div>
        <h1>{name}</h1>
        <div className="row">

            <ResponsiveContainer width="95%" aspect={3}>
                <AreaChart
                    data={data}
                    className="areachart"
                >
                    <CartesianGrid strokeDasharray="3 3"/>
                    <XAxis dataKey={xDataKey}/>
                    <YAxis type="number" domain={[0, 100]} allowDataOverflow={true} />
                    <Tooltip/>

                    <Area type="linear" dataKey={yDataKey} stroke={stroke} fill={fill}/>
                </AreaChart>
            </ResponsiveContainer>
        </div>
    </div>
}

export default Chart
