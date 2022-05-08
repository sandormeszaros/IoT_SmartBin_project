import React from 'react'

const Card = ({name, icon, value}) => {

    return (
        <div className="card">
            <h2>{name}</h2>
            <div className="card-content">
                <i className={`${icon} ${value < 20 ? 'icon-red' : 'icon-blue'}`}/>
                <p>{value.toFixed(2)}</p>
            </div>
        </div>
    )
}

export default Card
