// /api/irrigation.js
// POST -> dashboard sends { state: true/false } to command the pump
// GET  -> ESP8266 polls this to know whether to switch the relay

let command = { state: false };

export default function handler(req, res) {
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type');

  if (req.method === 'OPTIONS') return res.status(200).end();

  if (req.method === 'POST') {
    command.state = Boolean((req.body || {}).state);
    return res.status(200).json({ ok: true, state: command.state });
  }

  if (req.method === 'GET') {
    return res.status(200).json(command);
  }

  return res.status(405).json({ error: 'Method not allowed' });
}
