var RegisteredUser = require('../models/register');
const mongoose = require('mongoose');
module.exports = {

    createOne: function (req, res) {
        let newUserDetail = req.body;
        newUserDetail._id = new mongoose.Types.ObjectId();
        RegisteredUser.create(newUserDetail, function (err, user) {
            if (err) {
                return res.status(400).json(err);
            }
            else{
                res.json(user);
            }
        });
    },
};