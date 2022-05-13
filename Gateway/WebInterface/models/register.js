const mongoose = require('mongoose');

const RegisteredUser = new mongoose.Schema({
    _id: mongoose.Schema.Types.ObjectId,
    fName: {
        type: String,
        required: true
    },
    lName: {
        type: String,
        required: true
    },
    phoneNum: {
        type: String,
        required: true
    },
    email: {
        type: String,
        required: true
    },
    RFID: {
        type: String,
        required: true
    },

});
module.exports = mongoose.model('RegisteredUser', RegisteredUser);