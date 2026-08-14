const express = require('express');
const cors = require('cors');
const admin = require('firebase-admin');

const app = express();
app.use(cors());
app.use(express.json());

// --- GOOGLE FIRESTORE CONFIGURATION ---
// IMPORTANT: You will replace this with your actual JSON key from Google Console
const serviceAccount = {
  "type": "service_account",
  "project_id": "YOUR_PROJECT_ID",
  "private_key_id": "YOUR_PRIVATE_KEY_ID",
  "private_key": "-----BEGIN PRIVATE KEY-----\nYOUR_PRIVATE_KEY_HERE\n-----END PRIVATE KEY-----\n",
  "client_email": "YOUR_CLIENT_EMAIL",
  "client_id": "YOUR_CLIENT_ID",
  "auth_uri": "https://accounts.google.com/o/oauth2/auth",
  "token_uri": "https://oauth2.googleapis.com/token",
  "auth_provider_x509_cert_url": "https://www.googleapis.com/oauth2/v1/certs",
  "client_x509_cert_url": "https://www.googleapis.com/robot/v1/metadata/x509/..."
};

// Initialize Firebase
admin.initializeApp({
  credential: admin.credential.cert(serviceAccount)
});

const db = admin.firestore();

// --- API ENDPOINTS ---

// 1. POST: Receive data from ESP32
app.post('/api/data', async (req, res) => {
  try {
    const data = req.body;
    console.log('📥 Data received:', data);
    
    await db.collection('sawmill_readings').add({
      sawmill: data.sawmill || "FAVOUR SAWMILL",
      pm25: data.pm25 || 0,
      fan_active: data.fan_active || 0,
      alert: data.alert || 0,
      timestamp: admin.firestore.FieldValue.serverTimestamp()
    });
    
    res.json({ status: 'success', message: 'Data saved to Firestore' });
  } catch (error) {
    console.error('❌ Error:', error);
    res.status(500).json({ status: 'error', message: error.message });
  }
});

// 2. GET: Get the latest reading
app.get('/api/latest', async (req, res) => {
  try {
    const snapshot = await db.collection('sawmill_readings')
      .orderBy('timestamp', 'desc')
      .limit(1)
      .get();
    
    if (snapshot.empty) {
      return res.json({ status: 'no_data' });
    }
    
    let latest = {};
    snapshot.forEach(doc => {
      latest = { id: doc.id, ...doc.data() };
    });
    
    res.json(latest);
  } catch (error) {
    res.status(500).json({ status: 'error', message: error.message });
  }
});

// 3. GET: Get last 20 readings (for the graph)
app.get('/api/history', async (req, res) => {
  try {
    const snapshot = await db.collection('sawmill_readings')
      .orderBy('timestamp', 'desc')
      .limit(20)
      .get();
    
    let history = [];
    snapshot.forEach(doc => {
      history.push({ id: doc.id, ...doc.data() });
    });
    
    res.json(history);
  } catch (error) {
    res.status(500).json({ status: 'error', message: error.message });
  }
});

// Start server
const PORT = 3000;
app.listen(PORT, () => {
  console.log(`🚀 Favour Sawmill API running on port ${PORT}`);
});
