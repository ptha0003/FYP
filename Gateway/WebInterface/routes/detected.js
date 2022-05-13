var DetectedUser = require('../models/detected');

module.exports = {
    getAll: function (req, res) {
        DetectedUser.find(function (err, users) {
            if (err) {
                return res.status(404).json(err);
            } else {
                res.json(users);
            }
        });
    },
};