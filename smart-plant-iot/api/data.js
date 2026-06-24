// /api/data.js
// POST  -> ESP8266 sends { moisture, raw, pump, uptime }
// GET   -> dashboard reads the latest stored reading
//
// NOTE: Vercel serverless functions are stateless between cold starts,
// so this in-memory store is fine for a demo/college project but will
// reset occasionally. For persistent storage, swap `store` for Vercel KV,
// Upstash Redis, or a database (see README "Going to production").

let store = { latest: null };

export default function handler(req, res) {
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type');

  if (req.method === 'OPTIONS') return res.status(200).end();

  if (req.method === 'POST') {
    const { moisture, raw, pump, uptime } = req.body || {};
    store.latest = {
      moisture: Number(moisture) || 0,
      raw: Number(raw) || 0,
      pump: Boolean(pump),
      uptime: Number(uptime) || 0,
      time: Date.now(),
    };
    return res.status(200).json({ ok: true });
  }

  if (req.method === 'GET') {
    if (!store.latest) return res.status(200).json({});
    return res.status(200).json(store.latest);
  }

  return res.status(405).json({ error: 'Method not allowed' });
}
