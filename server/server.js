const express = require('express');
const bodyParser = require('body-parser');
const router = express.Router();
const socketio = require('socket.io');
const http = require('http');
const Client = require('azure-iothub').Client;
const Message = require('azure-iot-common').Message;
const EventHubReader = require('./event-hub-reader.js');
const iotHubConnectionString = "HostName=smart-bin-project.azure-devices.net;SharedAccessKeyName=iothubowner;SharedAccessKey=bNoxUV3/8TG7do3R8KSxFwvXzaPr0/ntP1ST9eKyyPQ=";
const eventHubConsumerGroup = "$default";
const targetDevice = "myESP32";
const cors = require('cors');

const PORT = process.env.PORT || 5000;

const app = express();
const server = http.createServer(app);
const io = socketio(server);

const client = Client.fromConnectionString(iotHubConnectionString);

function printResultFor(op) {
    return function printResult(err, res) {
        if (err) {
            console.log(op + ' error: ' + err.toString());
        } else {
            console.log(op + ' status: ' + res.constructor.name);
        }
    };
}

io.sockets.on('connection', (socket) => {
    console.log(`new connection id: ${socket.id}`);
    sendData(socket);
    socket.on('disconnect', () => {
        console.log('Disconnected');
    })
})

router.get('/c2d/:id', (req, res) => {
    client.open(function (err) {
        if (err) {
            console.error('Could not connect: ' + err.message);
            res.send('Not OK!')
        } else {
            console.log('Client connected');
            let data;
            switch (req.params.id) {
                case '1':
                    // TODO Implement your own message
                    data = "Message One";
                    break;
                case '2':
                    data = "Message Two";
                    break;
                default:
                    data = "No message";
                    break;
            }
            const message = new Message(data);
            console.log('Sending message: ' + message.getData());
            client.send(targetDevice, message, printResultFor('send'));
            res.send('OK!')
        }
    });
})

router.get('*', (req, res) => {
    res.send('Server is running');
});

app.use(bodyParser.urlencoded({extended: true}))
app.use(bodyParser.json())
app.use(cors());
app.use(router);
server.listen(PORT, () => console.log('Server is running on port', PORT));

function sendData(socket) {
    const eventHubReader = new EventHubReader(iotHubConnectionString, eventHubConsumerGroup);

    /*
    {
      messageId: 84,
      Temperature: 3.9,
      Humidity: 47.099998,
      date: '2021-03-09T20:07:39.491Z'
    }
    */

    (async () => {
        await eventHubReader.startReadMessage((message, date) => {
            try {
                const payload = {
                    ...message,
                    date: date.toISOString()
                };

                console.log(payload);
                socket.emit('payload', payload);
            } catch (err) {
                console.error('Error broadcasting: [%s] from [%s].', err, message);
            }
        });
    })().catch();
}
