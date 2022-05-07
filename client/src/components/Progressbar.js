import React from 'react'
import ProgressBar from "@ramonak/react-progress-bar";

const Progressbar = ({value}) => {

  return <ProgressBar 
    completed={value.toFixed(2)}
    height="40px" 
    />;
};

export default Progressbar
