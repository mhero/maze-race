const API_BASE = 'http://localhost:5555'
export const WS_BASE = 'ws://localhost:5555'

export async function register(username, password) {
  return postJson('/api/register', { username, password })
}

export async function login(username, password) {
  return postJson('/api/login', { username, password })
}

export async function createRoom(token, size = 15) {
  const res = await fetch(`${API_BASE}/api/rooms`, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json', Authorization: `Bearer ${token}` },
    body: JSON.stringify({ size }),
  })
  return handle(res)
}

export async function getLeaderboard() {
  const res = await fetch(`${API_BASE}/api/leaderboard`)
  return handle(res)
}

async function postJson(path, body) {
  const res = await fetch(`${API_BASE}${path}`, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(body),
  })
  return handle(res)
}

export async function handle(res) {
  const data = await res.json().catch(() => ({}))
  if (!res.ok) throw new Error(data.error || `Request failed (${res.status})`)
  return data
}
