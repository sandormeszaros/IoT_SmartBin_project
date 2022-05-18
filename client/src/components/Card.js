import React from 'react'

const Card = ({name, icon, value}) => {

    return (
        <div className="card">
            <h2>{name}</h2>
            <div className="card-content">
                <i className={`${icon} ${value < 5 ? 'icon-red' : 'icon-blue'}`}/>
                <p><b>{value.toFixed(2)}</b></p>
            </div>
            <div className={`${value > 20 ? 'warningmsg-visible' : 'warningmsg'}`}>
                <p>Value out of range!</p>
                <p>Check sensor position!</p>
            </div> 
        </div>
    )
}

export default Card
