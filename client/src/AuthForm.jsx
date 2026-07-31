import { useState } from 'react'
import { login, register } from './api'

export default function AuthForm({ onAuth }) {
  const [mode, setMode] = useState('login')
  const [username, setUsername] = useState('')
  const [password, setPassword] = useState('')
  const [error, setError] = useState(null)
  const [loading, setLoading] = useState(false)

  const submit = async (e) => {
    e.preventDefault()
    setError(null)
    setLoading(true)
    try {
      const fn = mode === 'login' ? login : register
      const data = await fn(username, password)
      onAuth({ token: data.token, username: data.username })
    } catch (err) {
      setError(err.message)
    } finally {
      setLoading(false)
    }
  }

  return (
    <div className="screen auth-screen">
      <div className="blueprint-card">
        <div className="eyebrow">MAZE // RACE</div>
        <h1 className="display-title">First to the center wins</h1>
        <p className="subtitle">Race up to 4 players through a fresh maze, live, over WebSockets.</p>

        <form onSubmit={submit} className="stack-form">
          <label className="field">
            <span>callsign</span>
            <input
              value={username}
              onChange={(e) => setUsername(e.target.value)}
              placeholder="e.g. navigator_07"
              required
              minLength={3}
              maxLength={24}
              autoFocus
            />
          </label>
          <label className="field">
            <span>passcode</span>
            <input
              type="password"
              value={password}
              onChange={(e) => setPassword(e.target.value)}
              placeholder="••••••••"
              required
              minLength={4}
            />
          </label>

          {error && <div className="error-banner">{error}</div>}

          <button type="submit" className="btn-primary" disabled={loading}>
            {loading ? 'Working…' : mode === 'login' ? 'Enter the maze' : 'Create callsign'}
          </button>
        </form>

        <button className="link-button" onClick={() => setMode(mode === 'login' ? 'register' : 'login')}>
          {mode === 'login' ? "Don't have a callsign? Register" : 'Already registered? Log in'}
        </button>
      </div>
    </div>
  )
}
