const bodyParser = require('body-parser');
const express = require('express');
const mongoose = require('mongoose');
const registeredUser = require('./routes/register.js');
const detectedUsers = require('./routes/detected.js');
const app = express();
const path = require('path');

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));
app.use("/", express.static(path.join(__dirname, "dist/web-interface")));
mongoose.connect('mongodb://admin:SUPERSECRETPASSWORD@192.168.0.14:27017/RFIDFYP', function (err) {
    if (err) {
        return console.log('Mongoose - connection error:', err);
    }
    console.log('Connect Successfully');
});

app.post('/registeredusers', registeredUser.createOne); 
app.get('/detectedusers',detectedUsers.getAll);

app.listen(8080);