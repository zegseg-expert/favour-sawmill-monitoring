// This file is for App Mint to convert the dashboard into a mobile app
const express = require('express');
const path = require('path');
const app = express();

app.use(express.static(__dirname));

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'dashboard.html'));
});

const PORT = 8080;
app.listen(PORT, () => {
  console.log(`App running on port ${PORT}`);
});
