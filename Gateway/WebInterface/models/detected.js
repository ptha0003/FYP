const mongoose = require('mongoose');

const detecedSchema = new mongoose.Schema({
    __id: mongoose.Schema.Types.ObjectId,
    uniqueID:{
        type: String,
        required: true
    },
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
    roomName: {
        type: String,
        required: true
    },
    enterTime:[{
        type: String,
        required: true
    }],
    exitTime: [{
        type: String,
        required: true
    }],
    RSSI: {
        type: String,
        required: true
    },
    test: {
        type: String,
        required:true
    },
});
module.exports = mongoose.model('DetectedUsers', detecedSchema);